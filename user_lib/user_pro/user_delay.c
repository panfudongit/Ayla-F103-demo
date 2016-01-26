
/***********************************
*	author: Dong
*	specification: delay
************************************/
#include "stm32f10x_rcc.h"
#include "misc.h"

static u8  fac_us = 0; /* us ��ʱ������ fac_us = SYSCLK/8 */
static u16 fac_ms = 0; /* ms ��ʱ������ */

void delay_init(void)
{
	SysTick->CTRL &= 0xfffffffb;
	fac_us = 72/8;
	fac_ms = fac_us * 1000;
}

void delay_us(u32 nus)
{
	SysTick->LOAD = nus * fac_us;
	SysTick->VAL = 0x00;  //��ռ�����
	SysTick->CTRL = 0x01; //ʹ�ö�ʱ��
	
	while(!(SysTick->CTRL & (0x1 << 16)))
		;
	
	SysTick->CTRL = 0x00; //�رն�ʱ��
	SysTick->VAL = 0x00;  //��ռ�����
}

void delay_ms(u32 nms)
{
	SysTick->LOAD = nms * fac_ms;
	SysTick->VAL = 0x00;  //��ռ�����
	SysTick->CTRL = 0x01; //ʹ�ö�ʱ��
	
	while(!(SysTick->CTRL & (0x1 << 16)))
		;
	
	SysTick->CTRL = 0x00; //�رն�ʱ��
	SysTick->VAL = 0x00;  //��ռ�����
}
