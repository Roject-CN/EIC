#include "stm32f10x.h"                  // Device header
#include "PWM.h"

 /*
   规定 PA8 - 左上轮C  B右上轮 - PA1
	    PA9 - 左下轮D  A右下轮 - PA0
		
		PA10 - 左IN1   右IN1 - PA3
		PA11 - 左IN2   右IN2 - PA2

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

//void Motor_Set_Speed_L(int32_t speed);
//void Motor_Set_Speed_R(int32_t speed);
//// 提前声明


void Motor_Init(void){
	PWM_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Initstructure);
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_2 | GPIO_Pin_3);
}

void Motor_Set_Speed_L(int32_t speed){
	if(speed > 0) GPIO_SetBits(GPIOA, GPIO_Pin_11);
	else GPIO_SetBits(GPIOA, GPIO_Pin_10);
	
    if (speed < 0) speed = -speed; //PWM只接受正值
	if (speed > 1000) speed = 1000; // 若超出范围则将其设置为最大值
	
	PWM_SetCompareLT(speed);
	PWM_SetCompareLD(speed);
	
}

void Motor_Set_Speed_R(int32_t speed){
	if(speed > 0) GPIO_SetBits(GPIOA, GPIO_Pin_2);
	else GPIO_SetBits(GPIOA, GPIO_Pin_3);
	
    if (speed < 0) speed = -speed; //PWM只接受正值
	if (speed > 1000) speed = 1000; // 若超出范围则将其设置为最大值
	
	PWM_SetCompareRT(speed);
	PWM_SetCompareRD(speed);
}

void Motor_Move_Foward(int32_t speed){
	Motor_Set_Speed_L(speed);
	Motor_Set_Speed_R(speed);
}
