#include "stm32f10x.h"                  // Device header
#include "Motor.h"

int main(void)
{
	Motor_Init();
	Motor_Move_Foward(500);
	while(1);
}
