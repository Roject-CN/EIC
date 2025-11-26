#include "stm32f10x.h"                  // Device header
#include "PWM.h"
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

//void Motor_Set_Speed_L(int speed);
//void Motor_Set_Speed_R(int speed);
//// 提前声明


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
