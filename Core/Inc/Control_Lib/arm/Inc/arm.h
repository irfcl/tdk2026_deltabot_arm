#ifndef INC_ARM_H_
#define INC_ARM_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

//extern UART_HandleTypeDef huart3;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim3;


// C 函數宣告

void arm_init(void);                        // init arm
void arm_timer_callback(void);              // arm timer callback
void arm_cascade_set_to_zero(void* pvParameters);         // set arm to zero position

extern volatile int lower_pwm;
extern volatile int upper_pwm;
extern volatile int intake_pwm;

extern volatile int lower_cnt;
extern volatile int upper_cnt;
extern volatile int intake_cnt;

extern volatile float lower_deg;
extern volatile float upper_deg;
extern volatile float intake_deg;

extern volatile float lower_test;
extern volatile float upper_test;
extern volatile float intake_test;

extern volatile int servo1_gobilda_pulse;
extern volatile int servo2_wrist_deg;
extern volatile int servo3_claw_deg;
extern volatile int servo4_fork_deg;

extern volatile int roller_pwm;

#ifdef __cplusplus
}

// C++ 標頭檔和類別定義
#include "UART_servo.h"

#endif /* __cplusplus */

#endif /* INC_ARM_H_ */
