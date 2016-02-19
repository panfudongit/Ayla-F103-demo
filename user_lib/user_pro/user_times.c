

/***************************************
*	author: Dong
*	specification: Timer
****************************************/

#include "stm32f10x_gpio.h"
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "misc.h"
#include "func.h"
#include <stm32.h>

#ifdef TIM3_ENABLE

#define Prescaler 36000
#define Period 2
void RCC_Timer(void)
{	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
}

/***************************************
*	specification: f: clock,  v: prescaler, t: period
* 1s:  f / v == period
* 1ms:  72 * 1000  /  72000 == 1000
****************************************/
void TIMER_cfg(int ms)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	TIM_DeInit(TIM3);
	
	TIM_InternalClockConfig(TIM3);
	
	TIM_TimeBaseStructure.TIM_Prescaler = Prescaler - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = (Period * ms) - 1;
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ARRPreloadConfig(TIM3, DISABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
}

void NVCC_Timer_cfg(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
}

void TIM3_Init(int ms)
{
	RCC_Timer();
	TIMER_cfg(ms);
	NVCC_Timer_cfg();	
// 	TIM_Cmd(TIM3, ENABLE);
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);
		conf_wifi_mode_poll();
	}
}

#endif

#ifdef TIM2_ENABLE

#define T2_Prescaler 36000
#define T2_Period 2
void RCC_Timer2(void)
{	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

/***************************************
*	specification: f: clock,  v: prescaler, t: period
* 1s:  f / v == period
* 1ms:  72 * 1000  /  72000 == 1000
****************************************/
void TIMER2_cfg(int ms)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	TIM_DeInit(TIM2);
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseStructure.TIM_Prescaler = T2_Prescaler - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = (T2_Period * ms) - 1;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ARRPreloadConfig(TIM2, DISABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
}

void NVCC_Timer2_cfg(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
}

int TIM2_Is_Enable(void)
{
	return ( (TIM2->CR1) & 0x0001);
}
void TIM2_Init(int ms)
{
	RCC_Timer2();
	TIMER2_cfg(ms);
	NVCC_Timer2_cfg();	
	
// 	TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
		Timeout_ReSend_4B_Ctrl_Packet();
	}
}

#endif
