/*
 * Copyright 2011-2013 Ayla Networks, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of Ayla Networks, Inc.
 */

#include <string.h>
#include <ayla/mcu_platform.h>
#include <mcu_io.h>
#include <stm32.h>

#include <func.h>

#ifdef DEMO_CONF
#include <ayla/conf_token.h>
#include <ayla/conf_access.h>
#endif

#include <ayla/ayla_proto_mcu.h>
#include <ayla/props.h>
#include <ayla/serial_msg.h>



#ifdef DEMO_UART
#include <ayla/uart.h>
#else
#include <spi_platform_arch.h>
#endif /* DEMO_UART */

#ifdef DEMO_FILE_PROP
#include <ayla/prop_dp.h>
#include <demo_stream.h>
#endif /* DEMO_FILE_PROP */

#ifdef DEMO_FILE_PROP
#define VERSION "demo_dp 1.3"
#else
#define VERSION "demo 1.3"
#endif /* DEMO_FILE_PROP */

#ifdef DEMO_SCHED_LIB
#include <ayla/schedeval.h>
#include <ayla/sched.h>
#include <ayla/cmp.h>
#endif /* DEMO_SCHED_LIB */


#ifdef DEMO_IMG_MGMT
#include <flash_layout.h>
#define BUILD_DATE ""
#ifdef AYLA_KEIL
/*
 * Image header location is fixed.
 */
#define IMG_HDR_LOC		(MCU_IMG_ACTIVE + IMAGE_HDR_OFF)
#define IMG_HDR_VER_LOC		(IMG_HDR_LOC + sizeof(struct image_hdr))

const struct image_hdr __img_hdr
			__attribute__((used))
			__attribute((at(IMG_HDR_LOC)));
const char version[72] __attribute((at(IMG_HDR_VER_LOC))) =
	 VERSION " " BUILD_DATE;
#else
const char version[] __attribute__((section(".version"))) =
	VERSION " " BUILD_DATE;
#endif /* AYLA_KEIL */
#else
const char version[] = VERSION;
#endif /* DEMO_IMG_MGMT || AYLA_BUILD_VERSION */

static u8 factory_reset;


static void set_input(struct prop *, void *arg, void *valp, size_t len);
static void set_cmd(struct prop *, void *arg, void *valp, size_t len);
static void set_dec_in(struct prop *prop, void *arg, void *valp, size_t len);

#ifdef DEMO_IMG_MGMT
extern u8 boot2inactive;
extern u8 template_req;
void mcu_img_mgmt_init(void);
int send_inactive_version(struct prop *, void *arg);
void set_boot2inactive(struct prop *, void *arg, void *valp, size_t len);
int send_template_version(struct prop *, void *arg);
void template_version_sent(void);
#endif

static s32 input;
static s32 output;
static s32 decimal_in;
static s32 decimal_out;
static u8 blue_button;
static char cmd_buf[TLV_MAX_STR_LEN + 1];

#ifdef DEMO_SCHED_LIB
static u8 sched_buf[256];
static u8 sched_out_length;
static const u8 *sched_out;
static struct sched_prop schedule_in;
static void set_schedule_in(struct prop *prop, void *arg, void *valp,
    size_t len);
#endif


static void set_led(struct prop *prop, void *arg, void *valp, size_t len)
{
	u8 val = *(u8 *)valp;
	u32 pin = (u32)arg;

	stm32_set_led(pin, val);
}

static int send_led(struct prop *prop, void *arg)
{
	u8 val;
	u32 pin = (u32)prop->arg;

	val = stm32_get_led(pin);
	return prop_send(prop, &val, sizeof(val), arg);
}

static int send_version(struct prop *prop, void *arg)
{
	return prop_send(prop, version, strlen(version), arg);
}

#ifdef DEMO_SCHED_LIB
static int send_schedule(struct prop *prop, void *arg)
{
	return prop_send(prop, sched_out, sched_out_length, arg);
}
#endif

static u8 send_from_host_ready = FALSE;
static u8 host_request_ctime_ready = FALSE;

static int cmdlist = 0;

static s32 work_statu;
static s32 heat_mode;
static s32 menu_var;
static s32 temp_ctrl;
static s32 work_time;
static s32 work_retime;
static s32 fault_alert;
static u8 child_lock;
static s32 lighting_ctrl;
static s32 device_power;
static s32 realtime_temp;

static void set_work_statu(struct prop *prop, void *arg, void *valp, size_t len);
static void set_heat_mode(struct prop *prop, void *arg, void *valp, size_t len);
static void set_menu_var(struct prop *prop, void *arg, void *valp, size_t len);
static void set_temp_ctrl(struct prop *prop, void *arg, void *valp, size_t len);
static void set_work_time(struct prop *prop, void *arg, void *valp, size_t len);
static void set_work_retime(struct prop *prop, void *arg, void *valp, size_t len);
static void set_fault_alert(struct prop *prop, void *arg, void *valp, size_t len);
static void set_child_lock(struct prop *prop, void *arg, void *valp, size_t len);
static void set_lighting_ctrl(struct prop *prop, void *arg, void *valp, size_t len);
static void set_device_power(struct prop *prop, void *arg, void *valp, size_t len);
static void set_realtime_temp(struct prop *prop, void *arg, void *valp, size_t len);

struct prop prop_table[] = {
#define DEMO_VERSION       0
	{ "version",          ATLV_UTF8, NULL,               send_version,      NULL,           0,                 AFMT_READ_ONLY},
#define WORK_STATU    1
	{ "vw_work_statu",    ATLV_INT,  set_work_statu,     prop_send_generic, &work_statu,    sizeof(work_statu)},
#define HEAT_MODE     2
	{ "vw_heat_mode",     ATLV_INT,  set_heat_mode,      prop_send_generic, &heat_mode,     sizeof(heat_mode)},
#define MENU_VAL      3
	{ "vw_menu_var",      ATLV_INT,  set_menu_var,       prop_send_generic, &menu_var,      sizeof(menu_var)},
#define TEMP_CTRL     4
	{ "vw_temp_ctrl",     ATLV_INT,  set_temp_ctrl,      prop_send_generic, &temp_ctrl,     sizeof(temp_ctrl)},
#define WORK_TIME     5
	{ "vw_work_time",     ATLV_INT,  set_work_time,      prop_send_generic, &work_time,     sizeof(work_time)},
#define WORK_RETIME   6
	{ "vw_work_retime",   ATLV_INT,  set_work_retime,    prop_send_generic, &work_retime,   sizeof(work_retime)},
#define FAULT_ALERT   7
	{ "vw_fault_alert",   ATLV_INT,  set_fault_alert,    prop_send_generic, &fault_alert,   sizeof(fault_alert)},
#define CHILD_LOCK    8
	{ "vm_child_lock",    ATLV_BOOL, set_child_lock,     prop_send_generic, &child_lock,    sizeof(child_lock)},
#define LINHTING_CTRL 9 
	{ "vw_lighting_ctrl", ATLV_BOOL, set_lighting_ctrl,  prop_send_generic, &lighting_ctrl, sizeof(lighting_ctrl)},
#define DEVICE_POWER  10
	{ "vw_device_power",  ATLV_BOOL, set_device_power,   prop_send_generic, &device_power,  sizeof(device_power)},
	#define REALTIME_TEMP 11
	{ "vw_realtime_temp", ATLV_INT,  set_realtime_temp,  prop_send_generic, &realtime_temp, sizeof(realtime_temp)},
	
	{ "Blue_button", ATLV_BOOL, NULL, prop_send_generic,
	    &blue_button, sizeof(blue_button), AFMT_READ_ONLY},
#define PROP_BUTTON 12
	{ "output", ATLV_INT, NULL, prop_send_generic, &output,
	    sizeof(output), AFMT_READ_ONLY},
#define PROP_OUTPUT 13
	{ "log", ATLV_UTF8, NULL, prop_send_generic, &cmd_buf[0],
	    0, AFMT_READ_ONLY},
#define PROP_LOG 14
	{ "decimal_out", ATLV_CENTS, NULL, prop_send_generic, &decimal_out,
	    sizeof(decimal_out), AFMT_READ_ONLY},
#define PROP_DEC_OUT 15
	{ "decimal_in", ATLV_CENTS, set_dec_in, prop_send_generic,
	    &decimal_in, sizeof(decimal_in)},
	{ "Blue_LED", ATLV_BOOL, set_led, send_led,
	    (void *)(1 << LED0_PIN), 1},
	{ "Green_LED", ATLV_BOOL, set_led, send_led,
	    (void *)(1 << LED1_PIN), 1},
#ifdef DEMO_SCHED_LIB
	{ "schedule_in", ATLV_SCHED, set_schedule_in, NULL, &schedule_in},
	{ "schedule_out", ATLV_SCHED, NULL, send_schedule, NULL, 0,
	  AFMT_READ_ONLY},
#define PROP_SCHED_OUT 20
#endif
	{ "cmd", ATLV_UTF8, set_cmd, prop_send_generic, &cmd_buf[0]},
	{ "input", ATLV_INT, set_input, prop_send_generic,
	    &input, sizeof(input)},

#ifdef DEMO_IMG_MGMT
	{ "inactive_version", ATLV_UTF8, NULL, send_inactive_version, NULL,
	  0, AFMT_READ_ONLY },
	{ "boot_to_inactive", ATLV_BOOL, set_boot2inactive, prop_send_generic,
	  &boot2inactive, sizeof(boot2inactive) },
	{ "oem_host_version", ATLV_UTF8, NULL, send_template_version },
#endif
#ifdef DEMO_FILE_PROP
	/*
	 * Long data points must use property type ATLV_LOC in this table,
	 * even though they have type ATLV_BIN in the protocol.
	 */
	{ "stream_up_len", ATLV_INT, set_length_up, prop_send_generic,
	    &stream_up_len, sizeof(stream_up_len)},
	{ "stream_up", ATLV_LOC, NULL, prop_dp_send, &stream_up_state, 0},
	{ "stream_down", ATLV_LOC, prop_dp_set, prop_dp_send,
	    &stream_down_state, 0},
	{ "stream_down_len", ATLV_UINT, NULL, prop_send_generic,
	   &stream_down_state.next_off, sizeof(stream_down_state.next_off)},
	{ "stream_down_match_len", ATLV_UINT, NULL, prop_send_generic,
	   &stream_down_patt_match_len, sizeof(stream_down_patt_match_len)},
#endif /* DEMO_FILE_PROP */
	{ NULL }
};
u8 prop_count = (sizeof(prop_table) / sizeof(prop_table[0])) - 1;

u8 send_property_from_host( void )
{
	if(cmdlist > 0)
	{
		prop_send(&prop_table[cmdlist], 	prop_table[cmdlist].arg,	prop_table[cmdlist].val_len,	NULL);
		cmdlist = 0;
		Mcu_Response_Host();
		return TRUE;
	}	
	return FALSE;
}

u8 host_request_ctime_host( void )
{
	conf_time_poll();

	return TRUE;
}

void Set_Network_Configura_Mode(u8 *infodata, int len)
{
		if(len < 0)
				return;
		
		USART1_send_char((char)infodata[0]);
		if(infodata[0] == 0x01 || infodata[0] == 0x02) // Wifi mode of AP(enter configure mode
		{
				conf_wifi_start_ap();
				return;
		}
		if(infodata[0] == 0x00) // esc confgiure mode
		{
			  stm32_reset_module(); //reset module
				return;
		}
}

void Host_Response_result(u8 *infodata, int len)
{
		if(len == 0)
			return;
			
		if(infodata[0] == 0)//  ctrl fail
			return;

		prop_table[infodata[2]].send_mask = valid_dest_mask;
		
}

void Host_Prop_Ctime(u8 *infodata, int len)
{
	host_request_ctime_ready = TRUE;
}

void Host_Prop_Sync_Service(u8 *infodata, int len)
{
			u8 *data = infodata;
			u32 tmp;
	
			if(len == 0)
				return;
	
			if(prop_table[data[0]].type == ATLV_INT)
			{
					cmdlist = data[0];
					t16tot32_3B(tmp, data[1], data[2], data[3]);
					*(s32 *)prop_table[cmdlist].arg = tmp; //send to the service data
					prop_table[cmdlist].val_len = sizeof(s32);
					send_from_host_ready = TRUE;  // host ready data up to the service
					return;
			}
			
			if(prop_table[data[0]].type == ATLV_BOOL)
			{
					cmdlist = data[0];
					*(s32 *)(prop_table[data[0]].arg) = data[1];
					prop_table[cmdlist].val_len = sizeof(u8);
					send_from_host_ready = TRUE;
					return;
			}
			
			if(prop_table[data[0]].type == ATLV_UTF8)
			{
					return;
			}
			
}

static void set_work_statu(struct prop *prop, void *arg, void *valp, size_t len)
{
		int data = *(int *)valp;

		if (len != sizeof(int) || data > 3) //data: 0, 1, 2, 3
		{
				return;
		}
		
		Send_4B_Ctrl_Packet(data, WORK_STATU, len);
//		prop_table[WORK_STATU].send_mask = valid_dest_mask;
}

static void set_heat_mode(struct prop *prop, void *arg, void *valp, size_t len)
{
		int data = *(int *)valp;

		if (len != sizeof(int) || data > 8) // data: 0, 1,.....8
		{
				return;
		}

		Send_4B_Ctrl_Packet(data, HEAT_MODE, len);
//		prop_table[HEAT_MODE].send_mask = valid_dest_mask;
}

static void set_menu_var(struct prop *prop, void *arg, void *valp, size_t len)
{
		int data = *(int *)valp;

		if (len != sizeof(int) || data > 28) // data 0, 1,.....28
		{
				return;
		}

		Send_4B_Ctrl_Packet(data, MENU_VAL, len);
//		prop_table[MENU_VAL].send_mask = valid_dest_mask;
}

static void set_temp_ctrl(struct prop *prop, void *arg, void *valp, size_t len)
{
		int data = *(int *)valp;

		if (len != sizeof(int) || data > 240 || data < 50) //data 50, 51,......240
		{
				return;
		}

		Send_4B_Ctrl_Packet(data, TEMP_CTRL, len);
//		prop_table[TEMP_CTRL].send_mask = valid_dest_mask;
}

static void set_work_time(struct prop *prop, void *arg, void *valp, size_t len)
{
		int data = *(int *)valp;

		if (len != sizeof(int) || data > 540 || data < 0) // data 0, 1,...... 540
		{
				return;
		}

		Send_4B_Ctrl_Packet(data, WORK_TIME, len);
//		prop_table[WORK_TIME].send_mask = valid_dest_mask;
}

static void set_work_retime(struct prop *prop, void *arg, void *valp, size_t len)
{
		int data = *(int *)valp;

		if (len != sizeof(int) || data > 540 || data < 0) // data 0, 1,..... 540
		{
				return;
		}

		Send_4B_Ctrl_Packet(data, WORK_RETIME, len);
//		prop_table[WORK_RETIME].send_mask = valid_dest_mask;
}

static void set_fault_alert(struct prop *prop, void *arg, void *valp, size_t len)
{
//		prop_table[FAULT_ALERT].send_mask = valid_dest_mask;
}

static void set_child_lock(struct prop *prop, void *arg, void *valp, size_t len)
{
		u8 data = *(u8 *)valp;

		if (len != sizeof(u8) || data > 1) // data: 0(on), 1(off)
		{
				return;
		}

		Send_4B_Ctrl_Packet(data, CHILD_LOCK , len);
//		prop_table[CHILD_LOCK].send_mask = valid_dest_mask;
}

static void set_lighting_ctrl(struct prop *prop, void *arg, void *valp, size_t len)
{
		u8 data = *(u8 *)valp;

		if (len != sizeof(u8) || data > 1) // data: 0(on), 1(off)
		{
				return;
		}

		Send_4B_Ctrl_Packet(data, LINHTING_CTRL, len);
//		prop_table[LINHTING_CTRL].send_mask = valid_dest_mask;
}

static void set_device_power(struct prop *prop, void *arg, void *valp, size_t len)
{
		u8 data = *(u8 *)valp;

		if (len != sizeof(u8) || data > 1) // data: 0(on), 1(off)
		{
				return;
		}

		Send_4B_Ctrl_Packet(data, DEVICE_POWER, len);
//		prop_table[DEVICE_POWER].send_mask = valid_dest_mask;
}

static void set_realtime_temp(struct prop *prop, void *arg, void *valp, size_t len)
{
		int data = *(int *)valp;

		if (len != sizeof(int) || data > 250) //data 0, 1,......250
		{
				return;
		}

		Send_4B_Ctrl_Packet(data, REALTIME_TEMP, len);
//		prop_table[REALTIME_TEMP].send_mask = valid_dest_mask;
}


static void set_input(struct prop *prop, void *arg, void *valp, size_t len)
{
	s32 i = *(s32 *)valp;

	if (len != sizeof(s32)) {
		return;
	}
	input = i;
	if (i > 0x7fff || i < -0x8000) {
		output = -1;		/* square would overflow */
	} else {
		output = i * i;
	}
	prop_table[PROP_OUTPUT].send_mask = valid_dest_mask;
}

static void set_dec_in(struct prop *prop, void *arg, void *valp, size_t len)
{
	s32 i = *(s32 *)valp;

	if (len != sizeof(s32)) {
		return;
	}
	decimal_in = i;
	decimal_out = i;
	prop_table[PROP_DEC_OUT].send_mask = valid_dest_mask;
}

static void set_cmd(struct prop *prop, void *arg, void *valp, size_t len)
{
	if (len >= sizeof(cmd_buf)) {
		len = sizeof(cmd_buf) - 1;
	}
	memcpy(cmd_buf, valp, len);
	cmd_buf[len] = '\0';
	prop_table[PROP_LOG].send_mask = valid_dest_mask;
}

#ifdef DEMO_SCHED_LIB
static void set_schedule_in(struct prop *prop, void *arg, void *valp,
    size_t len)
{
	if (len > sizeof(sched_buf)) {
		len = sizeof(sched_buf);
	}
	memcpy(sched_buf, valp, len);
	sched_out = sched_buf;
	sched_out_length = len;
	prop_table[PROP_SCHED_OUT].send_mask = valid_dest_mask;
	memcpy(schedule_in.tlvs, valp, len);
	schedule_in.len = sched_out_length;
	sched_run_all(NULL);
}
#endif

/*
 * Blue button push observed by interrupt handler.
 * Callers are in stm32.c
 */
void demo_set_button_state(u8 button_value)
{
	blue_button = button_value;
	prop_table[PROP_BUTTON].send_mask = valid_dest_mask;
}

int main(int argc, char **argv)
{
	struct prop *prop;
		
	USART1_Init();
	
	feature_mask |= MCU_LAN_SUPPORT;
#ifdef DEMO_IMG_MGMT
	mcu_img_mgmt_init();
	feature_mask |= MCU_OTA_SUPPORT;
#endif
#ifdef DEMO_SCHED_LIB
	feature_mask |= MCU_TIME_SUBSCRIPTION;
#endif
	mcu_io_init();
#ifdef DEMO_UART
	feature_mask |= MCU_DATAPOINT_CONFIRM;
	uart_init();
#else
	spi_platform_init();
#endif
	stm32_reset_module();
	stm32_init();
	//factory_reset = stm32_factory_reset_detect();
#ifdef DEMO_FILE_PROP
	demo_stream_init();
#endif /* DEMO_FILE_PROP */
	delay_init();
//	Keep_Live_Start();
  printd("main start ....... enter fordd");
					
// 	if(devices_unread() == 0) //unread
// 	{
// 			check_device_ready();
// 	}
// 	
//	Keep_Live_Start();
// 	devices_info = ReadFlash(0);
// 	u32tou8(dinfo, devices_info);
//	printd(dinfo);
// 	while(1)
// 	{
// 		if(wr == 1)
// 		{
// 			WirteFlash(0, 0x12345678);
// 			wr = 0;
// 		}
// 		delay_ms(1000);
// 		printd("a");
// 	}
	for (;;) {
		if (stm32_ready()) {
			if (factory_reset &&
			    !serial_tx_cmd(ACMD_LOAD_FACTORY, NULL, 0)) {
				factory_reset = 0;
				//stm32_set_factory_rst_led(0);
				while (stm32_ready()) {
					serial_poll();
				}
			}
#ifdef DEMO_CONF
			conf_poll();
#endif
			prop_poll();
			serial_poll();
#ifdef DEMO_SCHED_LIB
			if (sched_next_event_tick &&
			    (tick == sched_next_event_tick ||
			    cmp_gt(tick, sched_next_event_tick))) {
				sched_run_all(&sched_next_event_tick);
			}
#endif
		}
#ifdef DEMO_IMG_MGMT
		if (template_req &&
		    prop_send_done(prop_lookup("oem_host_version")) == 0) {
			/*
			 * Template version number has been sent.
			 */
		//	template_version_sent();
		}
#endif
		prop = prop_lookup_error();
		if (prop != NULL) {
			/*
			 * Property send has failed with error code.
			 * Error code is available in prop->send_err
			 *
			 * Insert logic here to handle the failure.
			 */
			prop->send_err = 0;
		}
		if( send_from_host_ready )
		{
			send_property_from_host();			
			send_from_host_ready = FALSE;
		}
		if( host_request_ctime_ready )
		{
			host_request_ctime_host();
			host_request_ctime_ready = FALSE;
		}
	}
}
