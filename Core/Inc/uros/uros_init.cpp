/*
 * uros_init.cpp
 *
 * Created on: Apr 9, 2025
 * Author: stanly
 */

#include "uros_init.h"
#include <math.h>
#include <string.h>
#include "arm.h"
#include "mission.hpp"
#include "mission_ctrl.h"

#include <std_msgs/msg/bool.h>
#include <std_msgs/msg/int32.h>

rcl_publisher_t           arm_pub;
std_msgs__msg__Int32      arm_msg;
rcl_publisher_t           mission_pub;
std_msgs__msg__Int32      mission_msg;
rcl_subscription_t        cmd_arm_sub;
std_msgs__msg__Int32      cmd_arm_msg;
rcl_timer_t               pub_timer;

rcl_subscription_t        cmd_intake_sub;
std_msgs__msg__Bool       cmd_intake_msg;
rcl_subscription_t        cmd_claw_sub;
std_msgs__msg__Bool       cmd_claw_msg;

rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_init_options_t init_options;
rclc_executor_t executor;

agent_status_t status = AGENT_WAITING;

int ping_fail_count = 0;
#define MAX_PING_FAIL_COUNT 5

extern UART_HandleTypeDef USARTx;

void uros_init(void) {
  // Initialize micro-ROS
  rmw_uros_set_custom_transport(
    true,
    (void *) &USARTx,
    cubemx_transport_open,
    cubemx_transport_close,
    cubemx_transport_write,
    cubemx_transport_read);
  
  rcl_allocator_t freeRTOS_allocator = rcutils_get_zero_initialized_allocator();

  freeRTOS_allocator.allocate = microros_allocate;
  freeRTOS_allocator.deallocate = microros_deallocate;
  freeRTOS_allocator.reallocate = microros_reallocate;
  freeRTOS_allocator.zero_allocate =  microros_zero_allocate;

  if (!rcutils_set_default_allocator(&freeRTOS_allocator)) {
    printf("Error on default allocators (line %d)\n", __LINE__);
  }
}

void uros_agent_status_check(void) {
  switch (status) {
    case AGENT_WAITING:
      handle_state_agent_waiting();
      break;
    case AGENT_AVAILABLE:
      handle_state_agent_available();
      break;
    case AGENT_CONNECTED:
      handle_state_agent_connected();
      break;
    case AGENT_TRYING:
      handle_state_agent_trying();
      break;
    case AGENT_DISCONNECTED:
      handle_state_agent_disconnected();
      break;
    default:
      break;
  }
}

void handle_state_agent_waiting(void) {
  __HAL_UART_CLEAR_OREFLAG(&USARTx);
  status = (rmw_uros_ping_agent(100, 10) == RMW_RET_OK) ? AGENT_AVAILABLE : AGENT_WAITING;
}

void handle_state_agent_available(void) {
  __HAL_UART_CLEAR_OREFLAG(&USARTx);
  uros_destroy_entities();
  uros_create_entities();

  if(status != AGENT_WAITING) {
      status = AGENT_CONNECTED;
  }
}

void handle_state_agent_connected(void) {
  if(rmw_uros_ping_agent(150, 3) == RMW_RET_OK){
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(50));
    ping_fail_count = 0; // Reset ping fail count
  } else {
    ping_fail_count++;
    if(ping_fail_count >= MAX_PING_FAIL_COUNT){
      status = AGENT_TRYING;
    }
  }
}

void handle_state_agent_trying(void) {
  if(rmw_uros_ping_agent(150, 3) == RMW_RET_OK){
    status = AGENT_CONNECTED;
    ping_fail_count = 0; // Reset ping fail count
  } else {
    ping_fail_count++;
    if(ping_fail_count >= MAX_PING_FAIL_COUNT){
      status = AGENT_DISCONNECTED;
      ping_fail_count = 0;
    }
  }
}

void handle_state_agent_disconnected(void) {
  uros_destroy_entities();
  status = AGENT_WAITING;
}


void uros_create_entities(void) {
  rcl_ret_t rc;

  __HAL_UART_CLEAR_OREFLAG(&USARTx);
  volatile uint32_t tmpreg = 0;
  while (__HAL_UART_GET_FLAG(&USARTx, UART_FLAG_RXNE)) {
      tmpreg = USARTx.Instance->DR;
  }

  allocator = rcl_get_default_allocator();

  init_options = rcl_get_zero_initialized_init_options();
  rcl_init_options_init(&init_options, allocator);
  rcl_init_options_set_domain_id(&init_options, DOMAIN_ID);

  rc = rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator);
  if(rc != RCL_RET_OK){
      status = AGENT_WAITING;
      rcl_init_options_fini(&init_options);
      return;
  }

  rcl_init_options_fini(&init_options);
  
  rc = rclc_node_init_default(&node, NODE_NAME, "", &support);
  if(rc != RCL_RET_OK){ status = AGENT_WAITING; return; }

  // Initialize publisher for arm_status
  rc = rclc_publisher_init_default(
    &arm_pub,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "robot/arm_status");
  if(rc != RCL_RET_OK){ status = AGENT_WAITING; return; }
  arm_msg.data = -1;

  rmw_uros_set_publisher_session_timeout(rcl_publisher_get_rmw_handle(&arm_pub), 10);

  // Initialize publisher for mission_starter
  rc = rclc_publisher_init_default(
    &mission_pub,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "mission_starter");
  if(rc != RCL_RET_OK){ status = AGENT_WAITING; return; }
  mission_msg.data = 0;

  rmw_uros_set_publisher_session_timeout(rcl_publisher_get_rmw_handle(&mission_pub), 10);

  // Initialize subscriber for arm command
  rc = rclc_subscription_init_default(
    &cmd_arm_sub,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "robot/cmd_arm");
  if(rc != RCL_RET_OK){ status = AGENT_WAITING; return; }
  cmd_arm_msg.data = -1;

  //0704新增
  rc = rclc_subscription_init_default(
	&cmd_intake_sub,
	&node,
	ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
	"robot/cmd_intake");
    if(rc != RCL_RET_OK){ status = AGENT_WAITING; return; }
    cmd_intake_msg.data = false;

    rc = rclc_subscription_init_default(
    &cmd_claw_sub,
	&node,
	ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
	"robot/cmd_claw");
    if(rc != RCL_RET_OK){ status = AGENT_WAITING; return; }
    cmd_claw_msg.data = false;
  //

  // Initialize timer for publishing
  rc = rclc_timer_init_default(
    &pub_timer,
    &support,
    RCL_MS_TO_NS(100),
    pub_timer_cb);
  if(rc != RCL_RET_OK){ status = AGENT_WAITING; return; }

  // Create executor (1 timer + 1 subscription)
  rc = rclc_executor_init(&executor, &support.context, 4, &allocator);
  if(rc != RCL_RET_OK){ status = AGENT_WAITING; return; }

  rclc_executor_add_subscription(&executor, &cmd_arm_sub, &cmd_arm_msg, &cmd_arm_sub_cb, ON_NEW_DATA);
  rclc_executor_add_subscription(&executor, &cmd_intake_sub, &cmd_intake_msg, &cmd_intake_sub_cb, ON_NEW_DATA);
   rclc_executor_add_subscription(&executor, &cmd_claw_sub, &cmd_claw_msg, &cmd_claw_sub_cb, ON_NEW_DATA);
  rclc_executor_add_timer(&executor, &pub_timer);
}

void uros_destroy_entities(void) {
  rmw_context_t* rmw_context = rcl_context_get_rmw_context(&support.context);
  (void) rmw_uros_set_context_entity_destroy_session_timeout(rmw_context, 0);

  // Destroy publisher
  rcl_publisher_fini(&arm_pub, &node);
  rcl_publisher_fini(&mission_pub, &node);

  // Destroy subscription
  rcl_subscription_fini(&cmd_arm_sub, &node);

  // Destroy timer
  rcl_timer_fini(&pub_timer);

  // Destroy executor
  rclc_executor_fini(&executor);

  // Destroy node
  rcl_node_fini(&node);
  rclc_support_fini(&support);
}


void cmd_arm_sub_cb(const void* msgin) {
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
  cmd_arm_msg = *msg;
  mission_type = cmd_arm_msg.data;
  mission_ctrl();
}

void cmd_intake_sub_cb(const void* msgin) {
  const std_msgs__msg__Bool * msg = (const std_msgs__msg__Bool *)msgin;
  cmd_intake_msg = *msg;

  //TODO
}

void cmd_claw_sub_cb(const void* msgin) {
  const std_msgs__msg__Bool * msg = (const std_msgs__msg__Bool *)msgin;
  cmd_claw_msg = *msg;

  //TODO
}

void pub_timer_cb(rcl_timer_t * timer, int64_t last_call_time){
  arm_msg.data = mission_status;
  rcl_publish(&arm_pub, &arm_msg, NULL);
  if(x1_reset){
      mission_msg.data = -1;
  }else{
      mission_msg.data = mis_set;
  }
  rcl_publish(&mission_pub, &mission_msg, NULL);
}



///*
// * uros_init.cpp
// *
// *  Created on: Apr 9, 2025
// *      Author: stanly
// */
//
//
//#include "uros_init.h"
//#include <math.h>
//#include <string.h>
//#include "arm.h"
//#include "mission.hpp"
//#include "mission_ctrl.h"
//
//
//rcl_publisher_t           arm_pub;
//std_msgs__msg__Int32      arm_msg;
//rcl_publisher_t           mission_pub;
//std_msgs__msg__Int32      mission_msg;
//rcl_subscription_t        cmd_arm_sub;
//std_msgs__msg__Int32      cmd_arm_msg;
//rcl_timer_t               pub_timer;
//
//
//rclc_support_t support;
//rcl_allocator_t allocator;
//rcl_node_t node;
//rcl_init_options_t init_options;
//rclc_executor_t executor;
//
//agent_status_t status = AGENT_WAITING;
//
//
//int ping_fail_count = 0;
//#define MAX_PING_FAIL_COUNT 5
//
//
//extern UART_HandleTypeDef USARTx;
//
//void uros_init(void) {
//  // Initialize micro-ROS
//  rmw_uros_set_custom_transport(
//    true,
//    (void *) &USARTx,
//    cubemx_transport_open,
//    cubemx_transport_close,
//    cubemx_transport_write,
//    cubemx_transport_read);
//
//  rcl_allocator_t freeRTOS_allocator = rcutils_get_zero_initialized_allocator();
//
//  freeRTOS_allocator.allocate = microros_allocate;
//  freeRTOS_allocator.deallocate = microros_deallocate;
//  freeRTOS_allocator.reallocate = microros_reallocate;
//  freeRTOS_allocator.zero_allocate =  microros_zero_allocate;
//
//  if (!rcutils_set_default_allocator(&freeRTOS_allocator)) {
//  printf("Error on default allocators (line %d)\n", __LINE__);
//  }
//}
//
//void uros_agent_status_check(void) {
//  switch (status) {
//    case AGENT_WAITING:
//      handle_state_agent_waiting();
//      break;
//    case AGENT_AVAILABLE:
//      handle_state_agent_available();
//      break;
//    case AGENT_CONNECTED:
//      handle_state_agent_connected();
//      break;
//    case AGENT_TRYING:
//      handle_state_agent_trying();
//      break;
//    case AGENT_DISCONNECTED:
//      handle_state_agent_disconnected();
//      break;
//    default:
//      break;
//  }
//}
//
//void handle_state_agent_waiting(void) {
//	__HAL_UART_CLEAR_OREFLAG(&USARTx);//新增
//  status = (rmw_uros_ping_agent(100, 10) == RMW_RET_OK) ? AGENT_AVAILABLE : AGENT_WAITING;
//}
//void handle_state_agent_available(void) {
//	__HAL_UART_CLEAR_OREFLAG(&USARTx);//新增
//  uros_create_entities();
//  status = AGENT_CONNECTED;
//}
//void handle_state_agent_connected(void) {
////  if(rmw_uros_ping_agent(20, 5) == RMW_RET_OK){
//  if(rmw_uros_ping_agent(150, 3) == RMW_RET_OK){
//    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(50));
//    ping_fail_count = 0; // Reset ping fail count
//  } else {
//    ping_fail_count++;
//    if(ping_fail_count >= MAX_PING_FAIL_COUNT){
//      status = AGENT_TRYING;
//    }
//  }
//}
//void handle_state_agent_trying(void) {
////  if(rmw_uros_ping_agent(20, 5) == RMW_RET_OK){
// if(rmw_uros_ping_agent(150, 3) == RMW_RET_OK){
//	status = AGENT_CONNECTED;
//    ping_fail_count = 0; // Reset ping fail count
//  } else {
//    ping_fail_count++;
//    if(ping_fail_count >= MAX_PING_FAIL_COUNT){
//      status = AGENT_DISCONNECTED;
//      ping_fail_count = 0;
//    }
//  }
//}
//void handle_state_agent_disconnected(void) {
//  uros_destroy_entities();
//  status = AGENT_WAITING;
//}
//
//
//void uros_create_entities(void) {
//  allocator = rcl_get_default_allocator();
//
//  init_options = rcl_get_zero_initialized_init_options();
//  rcl_init_options_init(&init_options, allocator);
//  rcl_init_options_set_domain_id(&init_options, DOMAIN_ID);
//
//  rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator); // Initialize support structure
//
//  rcl_init_options_fini(&init_options);
//
//  rclc_node_init_default(&node, NODE_NAME, "", &support);                       // Initialize node
//
//  rclc_publisher_init_default(                                                  // Initialize publisher for pose
//    &arm_pub,
//    &node,
//    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
//    "robot/arm_status");
//  arm_msg.data = -1;
//
//  rmw_uros_set_publisher_session_timeout(                                       // Set session timeout for publisher
//    rcl_publisher_get_rmw_handle(&arm_pub),
//    10);
//
//  rclc_publisher_init_default(                                                  // Initialize publisher for pose
//    &mission_pub,
//    &node,
//    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
//    "mission_starter");
//  mission_msg.data = 0;
//
//  rmw_uros_set_publisher_session_timeout(                                       // Set session timeout for publisher
//    rcl_publisher_get_rmw_handle(&mission_pub),
//    10);
//
//  rclc_subscription_init_default(                                               // Initialize subscriber for arm command
//    &cmd_arm_sub,
//    &node,
//    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
//    "robot/cmd_arm");
//  cmd_arm_msg.data = -1;
//
//  rclc_timer_init_default(                                                      // Initialize timer for publishing pose
//    &pub_timer,
//    &support,
//    RCL_MS_TO_NS(100),
//    pub_timer_cb);
//
//  rclc_executor_init(&executor, &support.context, 2, &allocator);               // Create executor (1 timer + 2 subscriptions)
//  rclc_executor_add_subscription(&executor, &cmd_arm_sub, &cmd_arm_msg, &cmd_arm_sub_cb, ON_NEW_DATA); // Add arm subscriber to executor
//  rclc_executor_add_timer(&executor, &pub_timer); // Add pose publisher timer to executor
//}
//void uros_destroy_entities(void) {
//  rmw_context_t* rmw_context = rcl_context_get_rmw_context(&support.context);
//  (void) rmw_uros_set_context_entity_destroy_session_timeout(rmw_context, 0);
//
//  // Destroy publisher
//  rcl_publisher_fini(&arm_pub, &node);
//
//  rcl_publisher_fini(&mission_pub, &node);
//
//  // Destroy subscription
//  rcl_subscription_fini(&cmd_arm_sub, &node);
//
//  // Destroy timer
//  rcl_timer_fini(&pub_timer);
//
//  // Destroy executor
//  rclc_executor_fini(&executor);
//
//  // Destroy node
//  rcl_node_fini(&node);
//  rclc_support_fini(&support);
//}
//
//
//void cmd_arm_sub_cb(const void* msgin) {
//  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
//  cmd_arm_msg = *msg;
//  mission_type = cmd_arm_msg.data;
//  mission_ctrl();
//}
//
//void pub_timer_cb(rcl_timer_t * timer, int64_t last_call_time){
//  arm_msg.data = mission_status;
//  rcl_publish(&arm_pub, &arm_msg, NULL);
//  if(x1_reset){
//	  mission_msg.data = -1;
//  }else{
//	  mission_msg.data = mis_set;
//  }
//  rcl_publish(&mission_pub, &mission_msg, NULL);
//}
