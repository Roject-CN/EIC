#include "stm32f10x.h"                  // Device header
#include "Motor.h"
#include "OLED.h"
#include "Trace.h"
#include "Timer.h"
#include "LEDB.h"
#include "Encoder.h"
#include "HCSR04.h"

#define TOTAL_TIME 100
#define FIRST_ENCODER 1600
#define SECOND_ENCODER 1800
#define SPEED_MOVE_FORWARD 600
#define DISTANCE 5
uint8_t time = 0;
uint8_t start_time = 0; // 用于C点停车
uint32_t start_time_ms = 0; // 用于C点停车
uint32_t time_ms = 0;


typedef enum { CLOSED = -2, EMPTY = -1, START = 0, MOVING = 1, STOP = 2, CARRY1 = 3, CARRY2, CARRY3, CARRY4, CARRY5, CARRY6, CARRY0, }STATE;
STATE initial_state = START; 
STATE current_state = EMPTY;

void Init(void); //存放各个模块的初始化函数的初始化函数
void State_entered( STATE state);
void State_transition( STATE from_state, STATE to_state);
void State_exited( STATE state);
void State_running( STATE state);
void Show_time(void);
void Show_status(void);
void test_speed(void);

int main(void)
{
	Init();
	OLED_ShowString(4,1,"Time:");
				
	while(1){ 
		//State_Machine Code
		/*
		Timer_Init();
		Motor_Move_Foward(550);
		test_speed();
		*/
		while (current_state != CLOSED){
			Show_time();
			Show_status();
			if (current_state == EMPTY) State_transition(current_state, initial_state);
			if (time >= TOTAL_TIME) {
				State_transition(current_state, CLOSED);
			}
			
			State_running(current_state);
			
		}
		
	};
}

//存放各个模块的初始化函数的初始化函数
void Init(void){
	Motor_Init();
	Trace_Init();
	OLED_Init();
	LEDB_Init();
	Encoder_Init();
	HCSR04_Init();
}
void test_speed(void){
	Show_time();
	if (time == 4){
		int speed1 = Encoder1_Get() /  time ;
		int speed2 = Encoder2_Get() / time ;
		OLED_ShowNum(1,1, speed1, 7);
		OLED_ShowNum(2,1, speed2, 7);
		
	}
	//测的是每秒的转速
	// 左上轮 4024 3967 = 3996 左下轮 3941 4021 = 3981
	// 右上轮 3977 3924 = 3951 左后轮 3884 3971 = 3928
	
}
void Show_time(void){
	time_ms = Get_timer();
	time = time_ms / 1000;
	OLED_ShowNum(4,6, time, 3);
}

void Show_status(void){ 
	OLED_ShowNum(1, 1, digital_signals[0], 1);
	OLED_ShowNum(1, 2, digital_signals[1], 1);
	OLED_ShowNum(1, 3, digital_signals[2], 1);
	OLED_ShowNum(1, 4, digital_signals[3], 1);
	OLED_ShowNum(1, 5, digital_signals[4], 1);
	OLED_ShowNum(1, 6, digital_signals[5], 1);
	OLED_ShowNum(1, 7, digital_signals[6], 1);
	OLED_ShowNum(1, 10, Trace_Get_Status() * 100, 6);
	OLED_ShowNum(3,3,GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11), 1);
}


void State_entered( STATE state){
	switch (state){
		case EMPTY:
			break;
		case START:
			Motor_Move_Foward(SPEED_MOVE_FORWARD);
			OLED_ShowChar(3,1,'T');
			//START用来预留给视觉识别部分 
			break;
		case MOVING:
			OLED_ShowChar(3,1,'M');
			break;
		case STOP:
			OLED_ShowChar(3,1,'S');
			start_time = time;
			break;
		case CARRY0:
			OLED_ShowChar(3,1,'0');
			Encoder_Clear();
			break;
		case CARRY1:
			OLED_ShowChar(3,1,'1');
			Encoder_Clear();
			break;
		case CARRY2:
			OLED_ShowChar(3,1,'2');
			Encoder_Clear();
			break;
		case CARRY3:
			OLED_ShowChar(3,1,'3');
			Encoder_Clear();
			break;
		case CARRY4:
			OLED_ShowChar(3,1,'4');
			Encoder_Clear();
			break;
		case CARRY5:
			HCSR05_Timer_Init();
			OLED_ShowChar(3,1,'5');
			HCSR04_Start();
			start_time_ms = time_ms;
			break;
		case CARRY6:
			OLED_ShowChar(3,1,'6');
			Encoder_Clear();
			break;
		case CLOSED:
			OLED_ShowChar(3,1,'C');
			Motor_Stop();
		default:
			break;
	}
}

void State_exited( STATE state){
	switch (state){
		case START:
			Timer_Init();
			Motor_Move_Foward(SPEED_MOVE_FORWARD);
			break;
		default:
			break;
	}
	return;
}

void State_transition( STATE from_state, STATE to_state){
	State_exited(current_state);
	current_state = to_state;
	State_entered(current_state);
}

void State_running( STATE state){
	switch (state){
		case EMPTY:
			break;
		//-------------------------------------------------
		case START:
			if (Trace_Get_Status() != 100.0) State_transition(current_state, MOVING);
			else Motor_Move_Foward(SPEED_MOVE_FORWARD);
			break;
		//-------------------------------------------------
		case MOVING:
			// 参数 1 为遇到黑线时停止 0则是遇到全白区域则停止
			if ( Motor_tracing_return_MOTORSTATE(1) == HOLD) {
				State_transition(current_state, STOP);
			}
			break;
		//-------------------------------------------------
		case STOP:  // 在C点停车并发出断续的声光信号的任务
			if ( (time - start_time) <= 2){
				uint8_t flag = (time_ms - time * 1000) / 100;
				if ( flag % 2 == 0) LEDB_turn_around();// Turn 电平转换
			}
			else{
				LEDB_OFF();
				State_transition(current_state, CARRY0);
			}
			break;
		case CARRY0:
			if (Motor_Back_To_Black() == FINISHED){
				State_transition(current_state, CARRY1);
			}
			break;
		//-------------------------------------------------
		case CARRY1: //准备八字绕弯 首先转弯然后跑到下一条黑线处
			if (Motor_Encoder_Left(FIRST_ENCODER) == FINISHED){
				if (Motor_Find_Black() == FINISHED){
					State_transition(current_state, CARRY2);
				}
			}	
			OLED_ShowNum(2,1,Encoder1_Get(), 6);
			OLED_ShowNum(2,8,Encoder2_Get(), 6);
			break;
		//-------------------------------------------------
		case CARRY2: //准备循迹 先用编码器转一部分 再启用转弯（直到有黑线才停下）
			OLED_ShowNum(2,1,Encoder1_Get(), 6);
			OLED_ShowNum(2,8,Encoder2_Get(), 6);
		
			if (Motor_Encoder_Right(FIRST_ENCODER) == FINISHED){
				if ( Motor_Turn_Left_To_Black() == FINISHED){
					State_transition(current_state, CARRY3);
				}
			}
			
			break;
		case CARRY3://循迹
			OLED_ShowNum(2,1,Encoder1_Get(), 6);
			OLED_ShowNum(2,8,Encoder2_Get(), 6);
			if (Motor_tracing_return_MOTORSTATE(0) == HOLD){
				State_transition(current_state, CARRY4);
			}
			break;
		case CARRY4://循迹完转弯准备跑到下一条黑线
			if (Motor_Encoder_Right(SECOND_ENCODER) == FINISHED){
				if (Motor_Find_Black() == FINISHED){
					State_transition(current_state, CARRY5);
				}
			}
			break;
		case CARRY5:
			Motor_tracing(SPEED_MOVE_FORWARD);
			if (start_time_ms - time_ms > 100){
				OLED_ShowNum(4,10, HCSR04_GetValue() * 100, 6);
				if (HCSR04_GetValue() <= 5) State_transition(current_state, CARRY6);
				else State_transition(current_state, CARRY5);
			}
			break;
		case CARRY6:
			Motor_Stop();
			break;
		default:
			break;
		//-------------------------------------------------
	}
}
