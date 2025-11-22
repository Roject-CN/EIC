#include "stm32f10x.h"                  // Device header
#include "Motor.h"
#include "OLED.h"
#include "Trace.h"

uint8_t *signals_pt;
int main(void)
{
//	Motor_Init();
	Trace_Init();
	OLED_Init();
	
	
	
	while(1){ 
	signals_pt = Trace_Get_Digit();
	OLED_ShowNum(1, 1, signals_pt[0], 1);
	OLED_ShowNum(1, 3, signals_pt[1], 1);
	OLED_ShowNum(1, 5, signals_pt[2], 1);
	OLED_ShowNum(1, 7, signals_pt[3], 1);
	OLED_ShowNum(2, 1, signals_pt[4], 1);
	OLED_ShowNum(2, 3, signals_pt[5], 1);
	OLED_ShowNum(2, 5, signals_pt[6], 1);
	};
}
