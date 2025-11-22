#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Delay.h"

typedef enum {
    CTRL_A = GPIO_Pin_4,
    CTRL_B = GPIO_Pin_5, 
    CTRL_C = GPIO_Pin_6,
	RECEIVE = GPIO_Pin_7
} Pin; //方便后续替换 因为只是临时的占位

/*

当数字灰度传感器 接收到 反射光的时候，数字灰度传感器通道上的LED灯会 亮 ，同时输出 低 电平 0
当数字灰度传感器     被遮挡   的时候，数字灰度传感器通道上的LED灯会 灭 ，同时输出 高 电平 1


Control the channel by A B C （PA4 PA5 PA6）
| INH | C | B | A | Channel |
|  0  | 0 | 0 | 0 |    0    |
|  0  | 0 | 0 | 1 |    1    |
|  0  | 0 | 1 | 0 |    2    |
|  0  | 0 | 1 | 1 |    3    |
|  0  | 1 | 0 | 0 |    4    |
|  0  | 1 | 0 | 1 |    5    |
|  0  | 1 | 1 | 0 |    6    |
|  0  | 1 | 1 | 1 |    7    |
|  1  | x | x | x |   无    |
*/
const uint16_t CONTROL[3] = { CTRL_A, CTRL_B, CTRL_C }; 
uint8_t digital_signals[7] = {0}; //存放数字灰度传感器的信号 

void Trace_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	
	//配置GPIO A B C
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Initstructure.GPIO_Pin = CTRL_A | CTRL_B | CTRL_C;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Initstructure);
	
	//配置GPIO 接收数字信号
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IPU;  // 数字灰度传感器的背面写了要IPU输入模式
	GPIO_Initstructure.GPIO_Pin = RECEIVE;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Initstructure);
	
	
}
    

void Control_Channel(uint8_t channel, uint8_t index){
	uint8_t number = channel;
	number = channel % 2;
	channel = channel / 2;
	if(channel >= 1 ){
		Control_Channel(channel, index + 1);
	};
	GPIO_WriteBit(GPIOA, CONTROL[index], number == Bit_RESET ? Bit_RESET : Bit_SET);
	//先把 Channel 解码成 二进制代码 比如 7 -> 1 1 1
	//然后按照 1(GPIO_Pin_6) -> 0 -> 0 (GPIO_Pin_4)的顺序给C B A写入相应的位 以打开我们将要读取的通道
	
	/* 
	运行的结果：
										   number :1 GPIO :4 ------ Channel 1
						number :1 GPIO :5  number :0 GPIO :4 ------ Channel 2
						number :1 GPIO :5  number :1 GPIO :4 ------ Channel 3
	 number :1 GPIO :6  number :0 GPIO :5  number :0 GPIO :4 ------ Channel 4
	 number :1 GPIO :6  number :0 GPIO :5  number :1 GPIO :4 ------ Channel 5
	 number :1 GPIO :6  number :1 GPIO :5  number :0 GPIO :4 ------ Channel 6
	 number :1 GPIO :6  number :1 GPIO :5  number :1 GPIO :4 ------ Channel 7
	*/
}

void Reset_Control(void){
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
	GPIO_ResetBits(GPIOA, GPIO_Pin_6);
	// 把 ABC都置 0 （复位） 
	/* 虽然我还是想不太明白为什么要这样写 因为是想到 可能上一个通道的位控制 可能会影响到 下一个通道的控制
	所以写了一个所有位的复位，但是我还是不清楚为什么，反正实测下来少了这个函数会发生重位的情况：就是数字灰度传感器1-3 没反应，
	然后4-7有重位的情况。
	*/
}

uint8_t* Trace_Get_Digit(void){
	//返回一个指针，指涉 digital_signals 的地址方便主函数查询
	//通道1-7
	for (int i = 1; i < 8; i++){
		Reset_Control();
		Control_Channel(i, 0);
		digital_signals[i - 1] = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
	}
	
	return digital_signals;
}
