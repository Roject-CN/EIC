#include "stm32f10x.h"                  // Device header

typedef enum {
    CTRL = GPIO_Pin_0,
} Pin; //方便后续替换 因为只是临时的占位

void LEDB_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	//配置GPIO 
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Initstructure.GPIO_Pin = CTRL;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_Initstructure);
	
	GPIO_SetBits(GPIOB, CTRL);
}
	
void LEDB_turn_around(void){
	if(GPIO_ReadInputDataBit(GPIOB, CTRL) == RESET){
		GPIO_SetBits(GPIOB, CTRL);
	}
	else{
		GPIO_ResetBits(GPIOB, CTRL);
	}
	
}

void LEDB_OFF(void){
	GPIO_SetBits(GPIOB, CTRL);
}