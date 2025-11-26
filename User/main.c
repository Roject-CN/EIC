#include "stm32f10x.h"                  // Device header
#include "Motor.h"
#include "OLED.h"
#include "Trace.h"
#include "Timer.h"
#include "LEDB.h"

#define TOTAL_TIME 100
#define SPEED_MOVE_FORWARD 550
#define SPEED_TURN_I 250
#define SPEED_TURN_K 150

float trace_k = 0.0;
uint8_t time = 0;
uint8_t start_time = 0; // 用于C点停车
uint32_t time_ms = 0;

typedef enum { CLOSED = -2, EMPTY = -1, START = 0, MOVING = 1, STOP = 2, CARRY1 = 3, CARRY2, CARRY3}STATE;
STATE initial_state = START;
STATE current_state = EMPTY;

void Init(void); //存放各个模块的初始化函数的初始化函数
void State_entered( STATE state);
void State_transition( STATE from_state, STATE to_state);
void State_exited( STATE state);
void State_running( STATE state);
void Tracing_Get_status(void);
void Show_time(void);
void Tracing_Basic(void);

int main(void)
{
	Init();
	OLED_ShowString(4,1,"Time:");
	while(1){ 
		//State_Machine Code
		while (current_state != CLOSED){
			Show_time();
			if (current_state == EMPTY) State_transition(current_state, initial_state);
			if (time >= TOTAL_TIME) {
				current_state = CLOSED;
				OLED_ShowChar(3,1,'C');
				break;
			}
			Tracing_Get_status();
			
			State_running(current_state);
		
		}
	};
}

//存放各个模块的初始化函数的初始化函数
void Init(void){
	Motor_Init();
	Trace_Init();
	OLED_Init();
	Timer_Init();
	LEDB_Init();
}

void Show_time(void){
	time_ms = Get_timer();
	time = time_ms / 1000;
	OLED_ShowNum(4,6, time, 3);
	OLED_ShowNum(4,10, time_ms, 6);
}

void Tracing_Get_status(void){ 
	//获取当前的循迹状态
	trace_k = Trace_Get_Status();
	OLED_ShowNum(1, 1, digital_signals[0], 1);
	OLED_ShowNum(1, 2, digital_signals[1], 1);
	OLED_ShowNum(1, 3, digital_signals[2], 1);
	OLED_ShowNum(1, 4, digital_signals[3], 1);
	OLED_ShowNum(1, 5, digital_signals[4], 1);
	OLED_ShowNum(1, 6, digital_signals[5], 1);
	OLED_ShowNum(1, 7, digital_signals[6], 1);
	OLED_ShowNum(2,1,trace_k * 100, 6);
	OLED_ShowNum(3,3,GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7), 1);
}

void Tracing_Basic(void){
	//基本的循迹逻辑 
	if (trace_k > 0 && trace_k < 100) {
		Motor_Turn_Left( SPEED_TURN_I + SPEED_TURN_K * trace_k);
		OLED_ShowChar(3,1,'L');
	}
	else if (trace_k < 0) {
		Motor_Turn_Right( SPEED_TURN_I + SPEED_TURN_K * -trace_k );
		OLED_ShowChar(3,1,'R');
	}
	else if (trace_k == 0) {
		Motor_Move_Foward( SPEED_MOVE_FORWARD );
		OLED_ShowChar(3,1,'M');
	}
}
void State_entered( STATE state){
	switch (state){
		case EMPTY:
			break;
		case START:
			Motor_Move_Foward(400);
		    //后面可以改成编码器
			break;
		case STOP:
			start_time = time;
			break;
		default:
			break;
	}
}

void State_exited( STATE state){
	return;
}

void State_transition( STATE from_state, STATE to_state){
	if (from_state == to_state) return;
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
			OLED_ShowChar(3,1,'T');
			if (trace_k < 100) {
				State_transition(current_state, MOVING);
			}
			break;
		//-------------------------------------------------
		case MOVING:
			Tracing_Basic();
			if (trace_k >= 100.0) {
				State_transition(current_state, STOP);
				OLED_ShowChar(3,1,'S');
			}
			break;
		//-------------------------------------------------
		case STOP:
			Motor_Stop();
			State_transition(current_state, CARRY1);
			break;
		//-------------------------------------------------
		case CARRY1: // 在C点停车并发出断续的声光信号的任务
			if ( (time - start_time) <= 2){
				uint8_t flag = (time_ms - time * 1000) / 100;
				if ( flag % 2 == 0) LEDB_turn_around();// Turn 电平转换
			}
			else{
				LEDB_OFF();
				State_transition(current_state, CARRY2);
			}
			break;
		//-------------------------------------------------
		case CARRY2: //准备八字绕弯
			if (trace_k >= 100.0){
				OLED_ShowChar(3,1,'A');
			}
			Motor_Move_Foward(500);
			break;
		default:
			break;
		//-------------------------------------------------
	}
}
