#ifndef __MOTOR_H
#define __MOTOR_H
void Motor_Init(void);
void Motor_Move_Foward(int speed);
void Motor_Stop(void);
uint8_t Motor_Encoder_Left(int left_encoder);
uint8_t Motor_Encoder_Right(int right_encoder);
uint8_t Motor_tracing_return_MOTORSTATE(uint8_t when_to_stop_white0_black1);
uint8_t Motor_Find_Black(void);
uint8_t Motor_Turn_Right_To_Black(void);
uint8_t Motor_Turn_Left_To_Black(void);
uint8_t Motor_Back_To_Black(void);
void Motor_tracing( int speed);
typedef enum { HOLD = 0, LEFT = 1, RIGHT = 2, FORWARD = 3, UNFINISED, FINISHED} MOTOR_STATE;

#endif 

