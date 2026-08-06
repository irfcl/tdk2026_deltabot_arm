//#include "UART_servo.h"
////#include "mainpp.h"
//#include <math.h>
//#include <stdint.h>
//#include "stm32f4xx_hal.h"
////reflect time of servo,轉道指定角度的時間,預設2秒
//uint16_t reflectime = 1000;
//uint16_t Checksum_Calc = 0;
////extern UART
//extern UART_HandleTypeDef huart3;
////servo control variale
////declare struct
////servo control function
////servo control variale
////servo control function
//void UART_servo::UART_Send(uint8_t u8_data) {
//	uint8_t *u8_pointer = &u8_data;
//	HAL_UART_Transmit(&huart3, u8_pointer, 1, 100);
//	Checksum_Calc += u8_data;
//}
//void UART_servo::UART_Send_SetMotorPosition(uint16_t motorId, uint16_t Position, uint16_t Time) {
//	Checksum_Calc = 0;
//	UART_Send(0x80 + motorId);    //header mark & broadcast ID
//	UART_Send(0x83);              //header mark & command code
//	UART_Send(0x05);              //total data length
//	UART_Send((Position / 256) & 0x7F);  //Servo Pos_H
//	UART_Send(Position % 256);           //Servo Pos_L
//	UART_Send((Time / 256) & 0x7F); //Servo Time_H
//	UART_Send(Time % 256);          //Servo Time_L
//	UART_Send(Checksum_Calc);     //data length (one servo with time and speed)
//}
//void UART_servo::run(){//put in timer IT
//	UART_Send_SetMotorPosition(motorId,(uint16_t)(800+7*pos),reflectime);
//}
//void UART_servo::update_pos(int _pos){
//	pos = _pos;
//}
