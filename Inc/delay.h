#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"

#define DelayDivider 1

#define Delay_Tick (TIM5->CNT)
#define COUNTERS 50
extern volatile uint32_t DelStart[COUNTERS];

void DelayInit(void);					//�������������
void DelayUs(uint32_t micros);			//�������� � ���
void DelayMs(uint32_t millis);			//�������� � ��
#define DelayStartCount(counter) (DelStart[counter]=Delay_Tick)
uint32_t DelayStopCount(uint32_t counter);	//����, ������� �����, ����������� �� ���������� � ���

