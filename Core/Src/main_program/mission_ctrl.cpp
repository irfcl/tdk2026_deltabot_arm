#include "mission_ctrl.h"


bool mis_1, mis_2, mis_3, mis_4, mis_LR;

int mis_num = 0;
int mis_dir = 0;
int mis_set = 0; // send to x1
int x1_reset = 0; //send to x1 0:normal  1:reset
bool mis_set_flag = false;
bool x1_reset_flag= false;
extern int mis_set_time;
extern int x1_reset_time;
extern int sec;
extern int sec_x1;

void send_mission()
{
    if (mis_set_flag){
        if (sec - mis_set_time > 500){
            mis_set_time = 0;
            sec = 0;
            mis_set_flag = false;
        }
    }else{
        mis_set = 0;
    }
}
void reset_x1() {
	if (x1_reset_flag) {
		if (sec_x1 - x1_reset_time > 500) {
			x1_reset_time = 0;
			sec_x1 = 0;
			x1_reset_flag = false;
		} else {
			x1_reset = 1;
		}
	} else {
		x1_reset = 0;
	}
}
void mission_set(){
    // 軟體去抖
        static uint32_t last_tick = 0;
        uint32_t now = HAL_GetTick();
        if (now - last_tick < 60) return;  // 30ms 去抖
        last_tick = now;

        // 這裡放按鈕觸發行為
        mis_set_flag = true;
        read_btn_sta();
        if (mis_1){
        	mis_num = 1;
        }else if (mis_2){
        	mis_num = 2;
        }else if (mis_3){
        	mis_num = 3;
        }else if (mis_4){
        	mis_num = 4;
        }else{
        	mis_num = 0;
        }
        if (mis_LR){
        	mis_dir = 1;
        }else{
        	mis_dir = 0;
        }
        mis_set = 10 * (mis_dir) + mis_num;
}
void read_btn_sta(){
	mis_1 = (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1) == GPIO_PIN_RESET);
	mis_2 = (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0) == GPIO_PIN_RESET);
	mis_3 = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == GPIO_PIN_RESET);
	mis_4 = (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_RESET);
	mis_LR = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == GPIO_PIN_RESET);
}

