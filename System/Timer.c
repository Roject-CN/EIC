#include "stm32f10x.h"                  // Device header


uint32_t timer = 0; // 1000为1s
void Timer_Init(void){

	//用TIM2的时基单元计时 TIM2已经在 PWM.c 初始化完成了 所以这里不初始化 而是直接配置中断函数

	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//配置NVIC为分组2
																//即抢占优先级范围：0~3，响应优先级范围：0~3
																//此分组配置在整个工程中仅需调用一次
																//若有多个中断，可以把此代码放在main函数内，while循环之前
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;						
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;				
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			
	NVIC_Init(&NVIC_InitStructure);							
	
}


void TIM2_IRQHandler(void){
	
	if (TIM_GetITStatus(TIM2,TIM_IT_Update) == SET){
		timer += 3;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}

uint32_t Get_timer(void){
	return timer;
}
