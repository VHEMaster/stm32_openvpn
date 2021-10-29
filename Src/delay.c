#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "delay.h"

TIM_HandleTypeDef htim5;

volatile uint32_t DelStart[COUNTERS];

void DelayInit(void)
{
    __TIM5_CLK_ENABLE();
    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_MasterConfigTypeDef sMasterConfig;

    htim5.Instance = TIM5;

    htim5.Init.Prescaler = (HAL_RCC_GetPCLK2Freq()/(1000000/DelayDivider))-1;
    htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim5.Init.Period = 0xFFFFFFFF;
    htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&htim5);

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig);

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig);

    HAL_TIM_Base_Start(&htim5);

}
 
void DelayUs(uint32_t val)
{
	uint32_t tickstart = Delay_Tick;
	while(1)
	{
		if((tickstart-1<Delay_Tick))
		{
			if(!((Delay_Tick - tickstart)*DelayDivider < val)) return;
		}
		else
		{
			if(!(((0xFFFFFFFF-tickstart)+Delay_Tick)*DelayDivider < val)) return;
		}
	}
}
 
void DelayMs(uint32_t val)
{
	uint32_t tickstart = Delay_Tick;
	val *= 1000;
	while(1)
	{
		if((tickstart-1<Delay_Tick))
		{
			if(!((Delay_Tick - tickstart)*DelayDivider < val)) return;
		}
		else
		{
			if(!(((0xFFFFFFFF-tickstart)+Delay_Tick)*DelayDivider < val)) return;
		}
	}
}

uint32_t DelayStopCount(uint32_t counter)
{
	uint32_t nowtime = Delay_Tick;
	
	if((DelStart[counter]-1<nowtime))
		return (nowtime-DelStart[counter])*DelayDivider;
	return ((0xFFFFFFFF-DelStart[counter])+nowtime)*DelayDivider;
	
}

