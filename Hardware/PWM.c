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
   规定 PA8 - 左上轮C  B右上轮 - PA1
	    PA9 - 左下轮D  A右下轮 - PA0
	   
	   
  占用 TIM1 定时器的端口：
  TIM1_CH1 - PA8
  TIM1_CH2 - PA9 
  占用 TIM2 定时器的端口：
  TIM2_CH1 - PA0
  TIM2_CH2 - PA1
*/

void PWM_Init(void){
	
	//开启 GPIOA 和 TIM1 的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	//配置GPIO
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_1 | GPIO_Pin_0;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Initstructure);
	
	//配置时基单元
	TIM_TimeBaseInitTypeDef TIM_TIimBaseInitStructure;
	TIM_TIimBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TIimBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TIimBaseInitStructure.TIM_Period = 1000 - 1; 
	TIM_TIimBaseInitStructure.TIM_Prescaler = 72 - 1;
	//输出1KHZ的PWN 步长为1/1000
	TIM_TIimBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TIimBaseInitStructure);
	TIM_TimeBaseInit(TIM2, &TIM_TIimBaseInitStructure);
	
	//设置 TIM 定时器的时钟来源
	TIM_InternalClockConfig(TIM1);
	TIM_InternalClockConfig(TIM2);
	
	//配置输出比较器
	TIM_OCInitTypeDef TIM_OCInitstructure;
	TIM_OCStructInit(&TIM_OCInitstructure);
	TIM_OCInitstructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitstructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitstructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitstructure.TIM_Pulse = 0;
	
	TIM_OC1Init(TIM1, &TIM_OCInitstructure); 
	TIM_OC2Init(TIM1, &TIM_OCInitstructure); 
	TIM_OC1Init(TIM2, &TIM_OCInitstructure); 
	TIM_OC2Init(TIM2, &TIM_OCInitstructure); 
	
	//使能 TIM1 和 TIM2 定时器
	TIM_Cmd(TIM1, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE); //必须加入这一行 高级定时器会默认禁用输出
}

void PWM_SetCompareLT(uint16_t Compare)
{
	// 左上轮 
	TIM_SetCompare1(TIM1, Compare);		
}

void PWM_SetCompareLD(uint16_t Compare)
{
	// 左下轮 
	TIM_SetCompare2(TIM1, Compare);		
}

void PWM_SetCompareRT(uint16_t Compare)
{
	// 右上轮 
	TIM_SetCompare2(TIM2, Compare);		
}

void PWM_SetCompareRD(uint16_t Compare)
{
	// 右下轮 
	TIM_SetCompare1(TIM2, Compare);		
}

