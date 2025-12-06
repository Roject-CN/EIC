#include "stm32f10x.h"                  // Device header

/*
  输出PWN波形的封装函数库： 
  PWM的初始化函数
  void PWM_Init(void)
  
  
  设置各PWN占空比的四个函数
  void PWM_SetCompareLT(uint16_t Compare)  // 左上轮 

  void PWM_SetCompareLD(uint16_t Compare)  // 左下轮 
 
  void PWM_SetCompareRT(uint16_t Compare)  // 右上轮 

  void PWM_SetCompareRD(uint16_t Compare)  // 右下轮 
  
 */
 
 /*
   规定 PB6 - 左上轮C  B右上轮 - PA1
	    PB7 - 左下轮D  A右下轮 - PA0
	   
	   
  TIM4_CH1 - PB6
  TIM4_CH2 - PB7 
  
  TIM2_CH1 - PA0
  TIM2_CH2 - PA1
*/

#define CORRECT_LTD 2
#define CORRECT_RTD 4
#define CORREC_LR 8
void PWM_Init(void){
	
	//开启 GPIOA 和 TIM2 4 的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	//配置GPIO
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Initstructure);
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_Initstructure);
	
	//配置时基单元
	TIM_TimeBaseInitTypeDef TIM_TIimBaseInitStructure;
	TIM_TIimBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TIimBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TIimBaseInitStructure.TIM_Period = 1000 - 1; 
	TIM_TIimBaseInitStructure.TIM_Prescaler = 72 - 1;
	//输出1KHZ的PWN 步长为1/1000
	TIM_TIimBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4, &TIM_TIimBaseInitStructure);
	TIM_TimeBaseInit(TIM2, &TIM_TIimBaseInitStructure);
	
	//设置 TIM 定时器的时钟来源
	TIM_InternalClockConfig(TIM4);
	TIM_InternalClockConfig(TIM2);
	
	//配置输出比较器
	TIM_OCInitTypeDef TIM_OCInitstructure;
	TIM_OCStructInit(&TIM_OCInitstructure);
	TIM_OCInitstructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitstructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitstructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitstructure.TIM_Pulse = 0;
	
	TIM_OC1Init(TIM4, &TIM_OCInitstructure); 
	TIM_OC2Init(TIM4, &TIM_OCInitstructure); 
	TIM_OC1Init(TIM2, &TIM_OCInitstructure); 
	TIM_OC2Init(TIM2, &TIM_OCInitstructure); 

	

	//使能 TIM4 和 TIM2 定时器
	TIM_Cmd(TIM4, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	
	
}

void PWM_SetCompareLT(uint16_t Compare)
{
	// 左上轮 
	TIM_SetCompare1(TIM4, Compare);		
}

void PWM_SetCompareLD(uint16_t Compare)
{
	// 左下轮 
	TIM_SetCompare2(TIM4, Compare + CORRECT_LTD);		
}

void PWM_SetCompareRT(uint16_t Compare)
{
	// 右上轮 
	TIM_SetCompare2(TIM2, Compare + CORREC_LR);		
}

void PWM_SetCompareRD(uint16_t Compare)
{
	// 右下轮 
	TIM_SetCompare1(TIM2, Compare + CORREC_LR + CORRECT_RTD);		
}

