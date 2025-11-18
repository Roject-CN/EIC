#ifndef __PWM_H
#define __PWM_H

void PWM_Init(void);
void PWM_SetCompareLT(uint16_t Compare);		
void PWM_SetCompareLD(uint16_t Compare);	
void PWM_SetCompareRT(uint16_t Compare);
void PWM_SetCompareRD(uint16_t Compare);

#endif 
