///*
// * UART_servo.h
// *
// *  Created on: Aug 20, 2025
// *      Author: 88698
// */
//
//#ifndef INC_UART_SERVO_H_
//#define INC_UART_SERVO_H_
//
//#include <stdint.h>
//#include "stm32f4xx_hal.h"
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
//#ifdef __cplusplus
//}
//
//class UART_servo{
//
//public:
//	void UART_Send(uint8_t u8_data);
//	void UART_Send_SetMotorPosition(uint16_t motorId, uint16_t Position, uint16_t Time);
//	void update_pos(int pos);
//	void run();
//    UART_servo(uint16_t id, uint16_t _reflect_time, UART_HandleTypeDef* _servo_uart)
//        : motorId(id), reflectime(_reflect_time), servo_uart(_servo_uart) {
//        // 初始化其他成員變數
//        Position = 0;
//        pos = 0;
//    }
//
//private:
//	uint16_t motorId;
//	uint16_t Position;
//	int pos;
//	uint16_t reflectime;
//	UART_HandleTypeDef* servo_uart;
//};
//
//#endif /* __cplusplus */
//#endif /* INC_UART_SERVO_H_ */
