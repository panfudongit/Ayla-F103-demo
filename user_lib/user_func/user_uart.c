/***************************************
*	author: Dong
*	specification: uart test
****************************************/


#include "stm32f10x_gpio.h"
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
#include <func.h>

extern int rd;
/***************************************
* Name   : NVIC_Configuration
* Deion  : Configures NVIC and Vector Table location
* Input  : None
* Output : None
* Return : None
*****************************************/
void NVIC_Configuration(void)
{
#ifdef UART1_IRQ
 		NVIC_InitTypeDef NVIC_InitStructure_uart1;
#endif
#ifdef ENABLE_UART2
	  NVIC_InitTypeDef NVIC_InitStructure_uart2;
#endif

			/* Configure the NVIC Preemption Priority Bits */
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
#ifdef UART1_IRQ	
	
		/* Enable the USART1 Interrupt */
		NVIC_InitStructure_uart1.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure_uart1.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure_uart1.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure_uart1);
#endif
	
#ifdef ENABLE_UART2	
			/* Enable the USART2 Interrupt */
		NVIC_InitStructure_uart2.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure_uart2.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure_uart2.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure_uart2);
#endif
}

/***************************************
* Name  : USART1_GPIO_Configuration
* Deion : Configures the uart1 gpio ports
* Input : None
* Output: None
* Return: None
****************************************/
void USART1_GPIO_Configuration(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE);
	
		/* Configure USART1_Tx as alternate push-pull */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		/* Configure USART1_Rx as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
		GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/***************************************
* Name  :  USART1_Configuration
* Deion :  Configures the uart1
* Input :  None
* Output:  None
* Return:  None
****************************************/
void USART1_Configuration(void)
{
		USART_InitTypeDef USART_InitStructure;
	
		USART_InitStructure.USART_BaudRate = 9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;
	
		USART_InitStructure.USART_Mode	=	USART_Mode_Rx | USART_Mode_Tx;
	
		/* Init the uart1 */
		USART_Init(USART1, &USART_InitStructure);

#ifdef UART1_IRQ	
		/* Enable uart1 receive and send interrupts */
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 
#endif	
	
		/* Enable uart1 */
		USART_Cmd(USART1, ENABLE);
}

void USART1_send_char(char ch)
{
		USART_SendData(USART1, ch);
		while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
}

void USART1_send_buf(char *buf, int len)
{
		char *send_buf = buf;
	
		while(len)
		{
				USART1_send_char(*send_buf++);
				len--;
		}
}

char USART1_receive_char(void)
{
		while(!USART_GetFlagStatus(USART1, USART_FLAG_RXNE));
		return USART_ReceiveData(USART1);
}

void USART1_Init(void)
{
	USART1_GPIO_Configuration();	
	USART1_Configuration();	
	NVIC_Configuration();
	USART_ClearFlag(USART2,USART_FLAG_TC);
}

#ifdef UART1_IRQ
/****************************************
* Name   : USART1_IRQHandler
* Deion  : USART1 irq fuc
* Input  : None
* Output : None
* Return : None
*****************************************/
void USART1_IRQHandler(void)
{
		/* USART_SR:5 , 0: no date, 1: date read */
		if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
		{
			USART_ClearITPendingBit(USART1, USART_IT_RXNE);
			Receive_Host_Byte( USART_ReceiveData(USART1) );
		}
		
		if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {
			USART_ClearITPendingBit(USART1,USART_IT_TXE);
		}
		
		if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
    {
			USART_ClearITPendingBit(USART1,USART_IT_TC);
		}
}
#endif

#ifdef ENABLE_UART2	
/***************************************
* Name  : USART2_GPIO_Configuration
* Deion : Configures the uart1 gpio ports
* Input : None
* Output: None
* Return: None
****************************************/
void USART2_GPIO_Configuration(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
		/* Configure USART2_Tx as alternate push-pull */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		/* Configure USART2_Rx as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
		GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/***************************************
* Name  :  USART2_Configuration
* Deion :  Configures the uart1
* Input :  None
* Output:  None
* Return:  None
****************************************/
void USART2_Configuration(void)
{
		USART_InitTypeDef USART_InitStructure;
	
		USART_InitStructure.USART_BaudRate = 9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;
	
		USART_InitStructure.USART_Mode	=	USART_Mode_Rx | USART_Mode_Tx;
	
		/* Init the uart2 */
		USART_Init(USART2, &USART_InitStructure);
	
		/* Enable uart2 receive and send interrupts */
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); 
		/* USART_ITConfig(USART1, USART_IT_TXE, ENABLE); */
	
		/* Enable uart2 */
		USART_Cmd(USART2, ENABLE);
}

void USART2_send_char(char ch)
{
		USART_SendData(USART2, ch);
		while(!USART_GetFlagStatus(USART2, USART_FLAG_TXE));
}

char USART2_receive_char(void)
{
		while(!USART_GetFlagStatus(USART2, USART_FLAG_RXNE));
		return USART_ReceiveData(USART2);
}

/****************************************
* Name   : USART2_IRQHandler
* Deion  : USART2 irq fuc
* Input  : None
* Output : None
* Return : None
*****************************************/
void USART2_IRQHandler(void)
{
		/* USART_SR:5 , 0: no date, 1: date read */
		char ch;
		if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
		{
			USART_ClearITPendingBit(USART2, USART_IT_RXNE);
// 			USART2_send_char( USART_ReceiveData(USART2) );
			ch = USART_ReceiveData(USART2);
			USART2_send_char(ch);
			USART1_send_char(ch);
		}
		
		if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
    {
			USART_ClearITPendingBit(USART2,USART_IT_TXE);
		}
		
		if (USART_GetITStatus(USART2, USART_IT_TC) != RESET)
    {
			USART_ClearITPendingBit(USART2,USART_IT_TC);
		}
}
#endif
