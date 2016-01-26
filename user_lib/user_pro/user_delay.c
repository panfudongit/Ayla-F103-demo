
/***********************************
*	author: Dong
*	specification: delay
************************************/
#include "stm32f10x_rcc.h"
#include "misc.h"

static u8  fac_us = 0; /* us 延时倍乘数 fac_us = SYSCLK/8 */
static u16 fac_ms = 0; /* ms 延时倍乘数 */

void delay_init(void)
{
	SysTick->CTRL &= 0xfffffffb;
	fac_us = 72/8;
	fac_ms = fac_us * 1000;
}

void delay_us(u32 nus)
{
	SysTick->LOAD = nus * fac_us;
	SysTick->VAL = 0x00;  //清空计数器
	SysTick->CTRL = 0x01; //使用定时器
	
	while(!(SysTick->CTRL & (0x1 << 16)))
		;
	
	SysTick->CTRL = 0x00; //关闭定时器
	SysTick->VAL = 0x00;  //清空计数器
}

void delay_ms(u32 nms)
{
	SysTick->LOAD = nms * fac_ms;
	SysTick->VAL = 0x00;  //清空计数器
	SysTick->CTRL = 0x01; //使用定时器
	
	while(!(SysTick->CTRL & (0x1 << 16)))
		;
	
	SysTick->CTRL = 0x00; //关闭定时器
	SysTick->VAL = 0x00;  //清空计数器
}
