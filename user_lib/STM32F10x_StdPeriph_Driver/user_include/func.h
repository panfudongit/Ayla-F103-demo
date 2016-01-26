
#include "stm32f10x.h"

void delay_init(void);
void delay_us(u32 nus);
void delay_ms(u32 nms);

#define TRUE			1
#define FALSE			0

/*****************************
* UART1 config
* RX:PA10, TX:PA9
******************************/
#define UART1_IRQ

void USART1_Init(void);
void USART1_GPIO_Configuration(void);
void USART1_Configuration(void);
void USART1_send_char(char ch);
void USART1_send_buf(char *buf, int len);
char USART1_receive_char(void);
void NVIC_Configuration(void);


/******************************
* TIM3
*******************************/
#define TIM3_ENABLE
void TIM3_Init(int ms);
void RCC_Timer(void);
void TIMER_cfg(int ms);
void NVCC_Timer_cfg(void);

/******************************
* TIM2
*******************************/
#define TIM2_ENABLE
void TIM2_Init(int ms);
void RCC_Timer2(void);
void TIMER2_cfg(int ms);
void NVCC_Timer2_cfg(void);

/******************************
* Flash Write, Read
*******************************/
void WirteFlash(int addoff, uint32_t *data, int wordn);
uint32_t ReadFlash(int addoff);

uint8_t Crc8(uint8_t *ptr, uint8_t len);
int devices_unread(void);
void check_device_ready(void);
void Set_Network_Configura_Mode(u8 *infodata, int len);
void Receive_Host_Byte(u8 ch);
void Keep_Live_Start(void);
void Keep_Live_Off(void);
void Mcu_Response_Host(void);
void Host_Prop_Sync_Service(u8 *infodata, int len);
void Host_Response_result(u8 *infodata, int len);
void Send_4B_Ctrl_Packet(int data, int id, int len);

int Wifi_Leady_Link(void);
#define LINK_N_GPIO	GPIOB
#define LINK_N_PIN	1
/****************************
* DEBUG msg
*****************************/
#define DEBUGD
#ifdef DEBUG
void printd(char *p);
#endif
extern void printd(char *p);

#define u8tou32(t, a, b, c, d) (t) = (((a) << 24) | ((b) << 16) | ((c) << 8) | (d))
#define u32tou8(t, a) { t[0] = ((a) >> 0); t[1] = ((a) >> 8); t[2] = ((a) >> 16); t[3] = ((a) >> 24); }
	
#define t16tot32_3B(i, a, b, c) (i) = (a + (256 * (b&0x0f)) + (4096 *(b >> 4)) + (65536 *(c&0x0f)))
int utc_to_ctime(u32 sencond, u8 *argv);


