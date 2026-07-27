#include "mission.hpp"
#include "arm.h"
#include "cmsis_os.h"

int mission_type = 0;
int prev_mission_type = 0;
int mission_status = 0;
int task_created = 0;

//0724
volatile ArmMode arm_mode = ARM_MISSION;
//

//0728
extern volatile bool lower_homing;
extern volatile bool upper_homing;
extern volatile bool intake_homing;

void mission_init(void)
{
    arm_init();
}

void mission_ctrl(void)
{
    if (prev_mission_type == mission_type)
        return;

    prev_mission_type = mission_type;

    if (task_created)
        return;

    task_created = 1;

    switch (mission_type)
    {
    BaseType_t ret;
    case 1:
    	arm_mode = ARM_MISSION;
    	ret = xTaskCreate(mission_1, "mission_1", 512, NULL, 2, NULL);
    	if(ret != pdPASS)
    	{
    	    task_created = -1;
    	}
    	break;

    case 2:
    	arm_mode = ARM_MISSION;
    	ret = xTaskCreate(mission_2, "mission_2", 512, NULL, 2, NULL);
    	if(ret != pdPASS)
    	{
    	    task_created = -1;
    	}
    	break;

    case 3:
    	arm_mode = ARM_MISSION;
    	ret = xTaskCreate(mission_3, "mission_3", 512, NULL, 2, NULL);
    	if(ret != pdPASS)
    	{
    	    task_created = -1;
    	}
    	break;

    case 99:
    	arm_mode = ARM_MISSION;
    	ret = xTaskCreate(mission_99, "mission_99", 512, NULL, 2, NULL);
    	if(ret != pdPASS)
    	{
    	    task_created = -1;
    	}
    	break;
    }
}

static void finishMission()
{
	arm_mode = ARM_MANUAL;
    mission_status = mission_type;
    task_created = 0;
    vTaskDelete(NULL);
}

//homing
void mission_99(void *pvParameters)
{
    mission_status = 0;
    arm_homing();
    while(lower_homing || upper_homing || intake_homing){
        osDelay(20);
    }
    finishMission();
}

void mission_1(void *pvParameters)
{
    mission_status = 0;
    lower_test = 0;
    upper_test = 0;
    servo2_wrist_deg = 60;
    servo3_claw_deg  = 60;
    servo1_gobilda_pulse = 500;
    osDelay(2000);
    lower_test = -60;
    osDelay(1500);
    upper_test = 120;
    osDelay(1500);
    servo3_claw_deg = 50;
    osDelay(1000);
    servo2_wrist_deg = 50;
    osDelay(1000);
    servo1_gobilda_pulse = 550;
    osDelay(2000);

    finishMission();
}

void mission_2(void *pvParameters)
{
    mission_status = 0;

    // TODO

    finishMission();
}

void mission_3(void *pvParameters)
{
    mission_status = 0;

    // TODO

    finishMission();
}



//#include "mission.hpp"
//#include "arm.h"
//#include "cmsis_os.h"
//
///*
//Please put all the mission related function here.
//*/
//int mission_type = 0; // 1: mission 1, 2: mission 2
//int prev_mission_type = 0;
//int mission_status = 0; // mission_type+status: 10*mission_type + status (0: working, 1: completed)
//int task_created = 0; // Flag to indicate if the tasks
//
//
//void mission_init(void){
//    arm_init();
//}
//
//void mission_ctrl(void){
//    if(prev_mission_type != mission_type){
//        prev_mission_type = mission_type;
//        switch (mission_type)
//        {
//        case 1:
//            if (!task_created) {
//                task_created = 1;
//                xTaskCreate(mission_1, "mission_1", 512, NULL, 2, NULL);
//            }
//            break;
//        case 2:
//			if (!task_created) {
//				task_created = 1;
//				xTaskCreate(mission_2, "mission_2", 512, NULL, 2, NULL);
//			}
//			break;
//        case 3:
//			if (!task_created) {
//				task_created = 1;
//				xTaskCreate(mission_3, "mission_3", 512, NULL, 2, NULL);
//			}
//			break;
//        case 4:
//			if (!task_created) {
//				task_created = 1;
//				xTaskCreate(mission_4, "mission_4", 512, NULL, 2, NULL);
//			}
//			break;
//        case 5:
//			if (!task_created) {
//				task_created = 1;
//				xTaskCreate(mission_5, "mission_5", 512, NULL, 2, NULL);
//			}
//			break;
//        case 6:
//			if (!task_created) {
//				task_created = 1;
//				xTaskCreate(mission_6, "mission_6", 512, NULL, 2, NULL);
//			}
//			break;
//        default:
//            break;
//        }
//    }
//}
//
//void mission_1(void *pvParameters){
//    mission_status = 0;
//
//    /* add motion here */
//    cascade_height = 300;
//    servo1_pos = standard_pos_1 + 70;
//    servo2_pos = standard_pos_2 + 75;
//    servo3_pos = gripper_open;
//	camera_servo_pos = camera_front;
//	osDelay(1500);
////	osDelay(5000);
//    /* add motion here */
//
//    mission_status = mission_type;
//    task_created = 0;
//    vTaskDelete(NULL);  // Delete current task when mission is complete
//}
//
//void mission_2(void *pvParameters){
//    mission_status = 0;
//
//    /* add motion here */
//    cascade_height = 447;
//    osDelay(2000);
//	servo1_pos = standard_pos_1 - 20;
//	servo2_pos = standard_pos_2 - 5;
//	servo3_pos = gripper_open;
//	camera_servo_pos = camera_front;
//    osDelay(1500);
//	servo3_pos = gripper_close;
//    osDelay(1200);
//    cascade_height = 497;
//    servo1_pos = standard_pos_1 + 77;
//    servo2_pos = standard_pos_2 + 77;
//	osDelay(1200);
//    /* add motion here */
//
//    mission_status = mission_type;//第一關終點
//    // goals_.push_back(create_goal(0, 0.0, 616.0, 4.71, 21, 0, 0, 20.0, 0.5));
//    // goals_.push_back(create_goal(0, 0.0, 616.0, 4.71, 10, 0, 0, 20.0, 0.5));
//    task_created = 0;
//    vTaskDelete(NULL);  // Delete current task when mission is complete
//}
//void mission_3(void *pvParameters){ //夾咖啡杯47
//    mission_status = 0;
//
//    /* add motion here */
//    cascade_height = 370;
//	servo1_pos = standard_pos_1 - 23;
//	servo2_pos = standard_pos_2 - 20;
//	servo3_pos = gripper_close;
//	camera_servo_pos = camera_down;
//	osDelay(1500);
//    /* add motion here */
//
//    mission_status = mission_type;
//    task_created = 0;
//    vTaskDelete(NULL);  // Delete current task when mission is complete
//}
//
//void mission_4(void *pvParameters){
//    mission_status = 0;
//
//    /* add motion here */
//    cascade_height = 323;
//	servo1_pos = standard_pos_1 - 50;
//	servo2_pos = standard_pos_2 - 42;
//	servo3_pos = gripper_close;
//	camera_servo_pos = camera_front;
//	osDelay(1200);
//	servo3_pos = gripper_open;
//	cascade_height = 473;
//	osDelay(1200);
//	cascade_height = 250;
//	servo1_pos = standard_pos_1 + 77;
//	servo2_pos = standard_pos_2 + 77;
//	servo3_pos = gripper_close;
//    /* add motion here */
//
//    mission_status = mission_type;
//    task_created = 0;
//    vTaskDelete(NULL);  // Delete current task when mission is complete
//}
//
//void mission_5(void *pvParameters){ //竹簍關
//    mission_status = 0;
//
//    /* add motion here */
//    basket_right_pos = 500+200/180*(basket_pos1-basket_grab);
//    basket_left_pos = 500+1200/180*(basket_pos2+basket_grab);
//	osDelay(1200);
//    /* add motion here */
//
//    mission_status = mission_type;
//    task_created = 0;
//    vTaskDelete(NULL);  // Delete current task when mission is complete
//}
//
//void mission_6(void *pvParameters){ //竹簍開
//    mission_status = 0;
//
//    /* add motion here */
//    basket_right_pos = 500+200/180*basket_pos1;
//    basket_left_pos = 500+1200/180*basket_pos2;
//	osDelay(1200);
//    /* add motion here */
//
//    mission_status = mission_type;
//    task_created = 0;
//    vTaskDelete(NULL);  // Delete current task when mission is complete
//}
//
//void mission_7(void *pvParameters){ //娃娃伸長然後放
//    mission_status = 0;
//
//    /* add motion here */
//    doll_arm_pos = 500+1200/180*doll_extend;
//    osDelay(2000);
//    doll_claw_pos = 500+1200/180*doll_open;
//    /* add motion here */
//
//    mission_status = mission_type;
//    task_created = 0;
//    vTaskDelete(NULL);  // Delete current task when mission is complete
//}
