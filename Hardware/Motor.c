#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include "Encoder.h"
#include "Trace.h"
 /*
   规定 PB6 - 左上轮C  B右上轮 - PA1
	    PB7 - 左下轮D  A右下轮 - PA0
		
		PB4(IN1) - 左CIN1 CIN2  |  右AIN1 BIN2 - PA3(IN1)
		PB5(IN2) - 左DIN2 DIN1  |  右AIN2 BIN1 - PA2(IN2)
		注:  IN1 IN2 控制电机转动方向 A/B/C/D INx 对应端口实际连接的电机驱动模块的引脚名称

   控制电机转动方向对照表：
   | 端口 | IN1 | IN2 |  输出结果 |
   |      |  -  |  +  |   正向    |
   |	  |  +  |  -  |   逆向    |
   |	  |  +  |  +  |    无     |
   |	  |  -  |  -  |    无     |
   其中 + 为 SET(即高电平)  - 为 RESET(即低电平)
   规定正向速度取值为 正值
       负向速度取值为 负值
   speed 范围 ： 0 ~ 1000 

  占用 TIM1 定时器的端口：
  TIM1_CH3 - PA10 
  TIM1_CH4 - PA11 
  占用 TIM2 定时器的端口：
  TIM2_CH3 - PA2 
  TIM2_CH4 - PA3 
*/

//int Motor_encoder1 = 0;
//int Motor_encoder2 = 0;
#define SPEED_MOVE_FORWARD 600
#define SPEED_TURN_I 450
#define SPEED_TURN_K 25



typedef enum { HOLD = 0, LEFT = 1, RIGHT = 2, FORWARD = 3, UNFINISED, FINISHED, EMPTY} MOTOR_STATE;

void Motor_Init(void){
	PWM_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	// 使能AFIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// 释放JTAG的PB4（NJTRST）和PB3（JTDO），但保留SWD
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Initstructure);
	
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_Initstructure);
	
	GPIO_ResetBits(GPIOA,  GPIO_Pin_2);
	GPIO_ResetBits(GPIOA,  GPIO_Pin_3);
	GPIO_ResetBits(GPIOB,  GPIO_Pin_4);
	GPIO_ResetBits(GPIOB,  GPIO_Pin_5);
}

void Motor_Set_Speed_L(int speed){
	if(speed > 0) {
		GPIO_SetBits(GPIOB, GPIO_Pin_5);
		GPIO_ResetBits(GPIOB, GPIO_Pin_4);
	
	}
	else {
		GPIO_SetBits(GPIOB, GPIO_Pin_4);
		GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	}
	
    if (speed < 0) speed = -speed; //PWM只接受正值
	if (speed > 1000) speed = 1000; // 若超出范围则将其设置为最大值
	
	PWM_SetCompareLT(speed);
	PWM_SetCompareLD(speed);
	
}

void Motor_Set_Speed_R(int speed){
	if(speed > 0) {
		GPIO_SetBits(GPIOA, GPIO_Pin_2);
		GPIO_ResetBits(GPIOA, GPIO_Pin_3);
	
	}
	else {
		GPIO_SetBits(GPIOA, GPIO_Pin_3);
		GPIO_ResetBits(GPIOA, GPIO_Pin_2);
	}
	
	
    if (speed < 0) speed = -speed; //PWM只接受正值
	if (speed > 1000) speed = 1000; // 若超出范围则将其设置为最大值
	
	PWM_SetCompareRT(speed);
	PWM_SetCompareRD(speed);
}

void Motor_Move_Foward(int speed){
	
	Motor_Set_Speed_L(speed);
	Motor_Set_Speed_R(speed);
}

void Motor_Turn_Right(int speed){
	Motor_Set_Speed_L(speed);
	Motor_Set_Speed_R(0);
}

void Motor_Turn_Left(int speed){
	Motor_Set_Speed_L(0);
	Motor_Set_Speed_R(speed);
}

void Motor_Stop(void){
	Motor_Set_Speed_L(0);
	Motor_Set_Speed_R(0);
}

uint8_t Motor_Encoder_Left(int left_encoder){
	int encoder = Encoder1_Get();
	if ( encoder <= left_encoder){
		Motor_Turn_Right(450);
		return UNFINISED;
	}
	else{
		Motor_Stop();
		return FINISHED;
	}
}

uint8_t Motor_Encoder_Right(int right_encoder){
	int encoder = Encoder2_Get();
	if (encoder <= right_encoder){
		Motor_Turn_Left(450);
		return UNFINISED;
	}
	else{
		Motor_Stop();
		return FINISHED;
	}
}


uint8_t Motor_tracing_return_MOTORSTATE(uint8_t when_to_stop_white0_black1){
	
	float chose_to_stop = 300.0;
	chose_to_stop = when_to_stop_white0_black1 == 1 ? 100.0 : 200.0;
	
	float trace_k = Trace_Get_Status();
	
	if (trace_k > 0 && trace_k < 100) {
		Motor_Turn_Left( SPEED_TURN_I + SPEED_TURN_K * trace_k);
		return LEFT;
	}
	else if (trace_k < 0) {
		Motor_Turn_Right( SPEED_TURN_I + SPEED_TURN_K * -trace_k );
		return RIGHT;
	}
	else if (trace_k == 0) {
		Motor_Move_Foward( SPEED_MOVE_FORWARD );
		return FORWARD;
	}
	else if (trace_k == chose_to_stop){
		Motor_Stop();
		return HOLD;
	}
	else{
		Motor_Move_Foward( SPEED_MOVE_FORWARD );
		return FORWARD;
	}

}

void Motor_tracing( int speed){
	
	float trace_k = Trace_Get_Status();
	
	if (trace_k > 0 && trace_k < 100) {
		Motor_Turn_Left( speed + SPEED_TURN_K * trace_k);
	}
	else if (trace_k < 0) {
		Motor_Turn_Right( speed + SPEED_TURN_K * -trace_k );
	}
	else if (trace_k == 0 || trace_k == 100.0) {
		Motor_Move_Foward( speed );
	}

}

uint8_t Motor_Find_Black(void){
	
	float trace_k = Trace_Get_Status();
	
	if (trace_k > 100.0){
		Motor_Move_Foward(SPEED_MOVE_FORWARD);
		return UNFINISED;
	}
	else{
		Motor_Stop();
		return FINISHED;
	}
	
}

uint8_t Motor_Turn_Right_To_Black(void){
	float trace_k = Trace_Get_Status();
	
	if (trace_k > 100.0){
		Motor_Turn_Right(SPEED_TURN_I);
		return UNFINISED;
	}
	else{
		Motor_Stop();
		return FINISHED;
	}
	
}

uint8_t Motor_Turn_Left_To_Black(void){
	float trace_k = Trace_Get_Status();
	
	if (trace_k > 100.0){
		Motor_Turn_Left(SPEED_TURN_I);
		return UNFINISED;
	}
	else{
		Motor_Stop();
		return FINISHED;
	}
}

uint8_t Motor_Back_To_Black(void){
	float trace_k = Trace_Get_Status();
	
	if (trace_k > 100.0){
		Motor_Move_Foward(-150);
		return UNFINISED;
	}
	else{
		Motor_Stop();
		return FINISHED;
	}
}
