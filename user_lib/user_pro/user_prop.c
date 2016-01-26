/***************************************
*	author: Dong
*	specification: protocol , modules host,   MCU devices
****************************************/

#include <func.h>
#include <user_data_tabel.h>
#include <stm32f10x.h>
#include <string.h>
#include <ayla/mcu_platform.h>
#include <mcu_io.h>


u8 infohead[HEADLEN];
u8 infodata[DATALEN];
static u8 indexh;
static u8 indexd;
static u8 infolen;
static int rd = 1;
extern u8 factory_reset;

/****************************************
* Name   : CRC8 verify
*****************************************/
uint8_t Crc8(uint8_t *ptr, uint8_t len)
{
		uint8_t i;
		uint8_t crc = 0;
	
		while(len-- != 0)
		{
				for(i = 1; i != 0; i*= 2)
				{
						if((crc & 1) != 0)
						{
								crc /= 2;
								crc ^= 0x8c;
						}		else {
								crc /= 2;
						}	
						if((*ptr & i ) != 0)
						{
								crc ^= 0x8c;
						}
				}		
				ptr++;
		}
		return (crc);
}

int Wifi_Leady_Link(void)
{
	/*PB2: LOW LINK,  HIGHT NOT LINK */
	return (LINK_N_GPIO->IDR & bit(LINK_N_PIN)) == 0;
}

/****************************************
* FUNC   :  data send to the host
*****************************************/
void Send_4B_Ctrl_Packet(int data, int id, int len)
{
		char pack[8];
		char *crc = (pack + 3);
		
		pack[0] = 0xfa;

		pack[3] = 0x03;
		pack[4] = id;
		if(data < 256) //data < 0xff
		{			
			pack[5] = data;
			pack[1] = 4; // info len 6 - 2
			pack[2] = (char)Crc8((uint8_t *)crc, 3); //crc8 value
			USART1_send_buf(pack, 6);	
			return;
		}
		if(data > 255 && data < 1000)
		{
				pack[6] = data & 0x000000ff;
				pack[5] = (data >> 8) & 0x000000ff;
				pack[1] = 5; // info len 7 - 2 
				pack[2] = (char)Crc8((uint8_t *)crc, 4);  //crc8 value
				USART1_send_buf(pack, 7);	
				return;
		}
		

}
/****************************************
* Name   : Host_Request_Info
* Deion  : type: 0x00,   response: no,  
*****************************************/
void Host_Request_Info(void)
{
		char buf[4];
	
		buf[0] = 0xfa; // type: 0x00
		buf[1] = 0x02; // len:  0x02
		buf[2] = 0x00; // crc8 = 0x00
		buf[3] = 0x00; // data = 0x00
	
		USART1_send_buf(buf, 4);
}

/****************************************
* FUNC   :  request host info
*****************************************/
void Mcu_Response_Host(void)
{
		char buf[4];
	
		buf[0] = 0xfa;
		buf[1] = 0x02;
		buf[2] = 0xbc;
		buf[3] = 0x02;
	
		USART1_send_buf(buf, 4);
}

/****************************************
* Name   : devices_unread
* Deion  : check device whether has been start
* Return : None
*****************************************/
int devices_unread(void)
{
		uint32_t data;
	
		data = ReadFlash(FLAG);
	
		if(data == (FLAG_DATA))
				return 1;
		
		return 0;
}
/****************************************
* Name   : check_read
* Deion  : check device info
* Return : None
*****************************************/
void check_device_ready(void)
{
		uint32_t data[8];
	
		while(rd)
		{
				Host_Request_Info();
				delay_ms(1000);
		}
		
		data[0] = 0xb1b1a2a2;
		u8tou32(data[1], infohead[0], infohead[1], infohead[2], infohead[3]);
		u8tou32(data[2], infodata[0], infodata[1], infodata[2], infodata[3]);
		u8tou32(data[3], infodata[4], infodata[5], infodata[6], infodata[7]);
		u8tou32(data[4], infodata[8], infodata[9], infodata[10], infodata[11]);
		u8tou32(data[5], 0x00, 0x00, infodata[12], infodata[13]);
		u8tou32(data[6], infodata[infohead[1] - 4], infodata[infohead[1] - 3], infodata[infohead[1] - 2], infodata[infohead[1] - 1]);
		
		WirteFlash(0, data, 7);
		
}

/****************************************
* FUNC   :  clear buf and index
*****************************************/
void Clear_buf_off()
{
		memset(infohead, 0, HEADLEN);
		memset(infodata, 0, DATALEN);	
		indexd = 0;
		indexh = 0;
}

/****************************************
* FUNC   :  receive host data of Byte
*****************************************/
void Receive_Host_Byte(u8 ch)
{
		USART1_send_char((char)ch);
	  /* Detect whether the message header */
		if((ch == 0xfa) && (indexh == 0) && infohead[0] != 0xfa) 
		{
			infohead[0] = ch;
			infohead[1] = 0xfb;
			indexh = 1;
			indexd = 0;
			return;
		}
		
		/* Detect whether the message len */
		if(infohead[1] == 0xfb && infohead[0] == 0xfa)
		{
				infohead[1] = ch;
				indexh = 2;
				infolen = infohead[1] - 2;
				return;
		}
		
		/* Detect whether the message len for 4 */
		if(indexh < HEADLEN)
		{
				infohead[indexh] = ch;
				indexh = indexh + 1;
				return;
		}
		
		/* The message content */
		if(indexd < infolen)
		{
				infodata[indexd] = ch;
				indexd = indexd + 1;
		}
		
		/* The message content receive complete */
		if(indexd == infolen)
		{
				if(infohead[3] == 0x00) //type 0x00
				{
						rd = 0;  // host ready
						Clear_buf_off();
						return;
				}
				if(infohead[3] == 0x06) //type 0x06
				{
						Set_Network_Configura_Mode(infodata, indexd);
						Mcu_Response_Host();
						Clear_buf_off();
						return;
				}
				if(infohead[3] == 0x04) //type 0x04
				{
						Host_Prop_Sync_Service(infodata, indexd);
						Clear_buf_off();
						return;
				}
				if(infohead[3] == 0x03) //type 0x03
				{
						Host_Response_result(infodata, indexd);
						Clear_buf_off();
						return;
				}
				if(infohead[3] == 0x0c) // type 0x0c
				{
						Clear_buf_off();
						return;
				}
		}
			
}

/****************************************
* Name   : Keep_Live_Start
* Deion  : hear pro start
* Return : None
*****************************************/
void Keep_Live_Start(void)
{
		TIM3_Init(5000);  // time 5s
		TIM_Cmd(TIM3, ENABLE);
}

/****************************************
* Name   : Keep_Live_Off
* Deion  : hear pro off
* Return : None
*****************************************/
void Keep_Live_Off(void)
{
		TIM_Cmd(TIM3, DISABLE);
}
