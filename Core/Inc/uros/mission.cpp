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
extern volatile bool fork_homing;


void mission_init(void)
{
    arm_init();
}

void mission_ctrl(void)
{
//    if (prev_mission_type == mission_type)
//        return;
//
//    prev_mission_type = mission_type;

    if (task_created)
        return;

    if(mission_type == 0)
            return;

        BaseType_t ret;

    task_created = 1;

    switch (mission_type)
    {
//    BaseType_t ret;
    case 301:
    	arm_mode = ARM_MISSION;
    	ret = xTaskCreate(mission_301, "mission_301", 512, NULL, 2, NULL);
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
    mission_type = 0;
    task_created = 0;
    vTaskDelete(NULL);
}

//homing
void mission_99(void *pvParameters)
{
    mission_status = 0;
    arm_homing();
    while(lower_homing || upper_homing){
        osDelay(20);
    }
    finishMission();
}

void mission_301(void *pvParameters)
{
    mission_status = 0;

    //初始
    servo2_wrist_deg = 53;
    servo3_claw_deg = 100;
    servo4_slewing_deg = 850;
    osDelay(2000);
    lower_test = 10;
    upper_test = -70;
    osDelay(1000);

    //轉到intake方向
    for (int i = 1; i <= 50; i++){
    	servo4_slewing_deg = 850 + ((945 - 850) * i / 50);
        osDelay(50);
    }
    servo3_claw_deg = 120;
    lower_test = -20;
    upper_test = -50;
    osDelay(1000);

    //往稻草卷伸
    lower_test = 0;
    osDelay(1000);
    lower_test = 30;
    upper_test = -30;
    osDelay(1000);
    lower_test = 50;
    upper_test = -50;
    osDelay(1000);
    lower_test = 75;
    upper_test = -75;
    osDelay(2000);

    //夾+抬
    servo3_claw_deg = 85;
    osDelay(1000);
    roller_pwm = 1000;
	osDelay(1000);
    lower_test = 20;
	osDelay(1000);
	roller_pwm = 0;
	upper_test = -90;
	osDelay(2000);

	//橫放到對面叉子上
	lower_test = 0;
	osDelay(500);
	upper_test = -190;
	osDelay(2000);
	lower_test = -10;
	osDelay(1000);
	for (int i = 1; i <= 50; i++){
	    servo2_wrist_deg = 53 + ((108 - 53) * i / 50);
	    osDelay(30);
	}
	osDelay(500);
    servo3_claw_deg = 110;
    osDelay(1000);
    upper_test = -150;


//    //轉回去
//    for (int i = 1; i <= 50; i++){
//    	servo4_fork_deg = 945 - ((945 - 850) * i / 50);
//        osDelay(100);
//    }

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
