

/**
 * \file
 *
 * \brief SAM Uart driver with DMA quick start
 *
 * Copyright (C) 2015-2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <string.h>
#include "platform.h" 
#include "at_ble_api.h" 
#include "console_serial.h" 
#include "timer_hw.h" 
#include "ble_manager.h" 
#include "hr_sensor.h"
#include "ble_utils.h" 
#include "hr_sensor_app.h"
#include "button.h" 
#include "startup_template_app.h"
#include "battery.h"
#include "blp_sensor_app.h"
#include "blp_sensor.h"
#include "profiles.h"

//#include "battery_info.h"
/** @brief APP_BAS_FAST_ADV between 0x0020 and 0x4000 in 0.625 ms units (20ms to 10.24s). */
#define APP_BAS_FAST_ADV				(1600) //1000 ms

/** @brief APP_BAS_ADV_TIMEOUT Advertising time-out between 0x0001 and 0x3FFF in seconds, 0x0000 disables time-out.*/
#define APP_BAS_ADV_TIMEOUT				(655) // 10 min


//#include "aon_sleep_timer_basic.h"
#include<stdio.h>

#define USER_LED LED_0_PIN
#define GPIO_DIRECTION GPIO_PIN_DIR_OUTPUT

#define UARRT0 EDBG_CDC_SERCOM_PIN_PAD0
#define UARRT1 EDBG_CDC_SERCOM_PIN_PAD1
#define UARRT2 EDBG_CDC_SERCOM_PIN_PAD2
#define UARRT3 EDBG_CDC_SERCOM_PIN_PAD3

#define UARMT0 EDBG_CDC_SERCOM_MUX_PAD0
#define UARMT1 EDBG_CDC_SERCOM_MUX_PAD1
#define UARMT2 EDBG_CDC_SERCOM_MUX_PAD2
#define UARMT3 EDBG_CDC_SERCOM_MUX_PAD3

volatile at_ble_status_t status;
at_ble_handle_t htpt_conn_handle;
volatile bool Timer_Flag = false;
volatile bool Temp_Notification_Flag = false;
//! [module_inst]
struct uart_module uart_instance;
//! [module_inst]
float temperature;
float battery_level; //=BATTERY_MIN_LEVEL;

//! [dma_resource]
struct dma_resource uart_dma_resource_tx;
struct dma_resource uart_dma_resource_rx;
//! [dma_resource]

//! [usart_buffer]
#define BUFFER_LEN    2
static uint8_t string[BUFFER_LEN];
//! [usart_buffer]

float int_value;
float temp_value;
float float_value;


//! [transfer_descriptor]
struct dma_descriptor example_descriptor_tx;
struct dma_descriptor example_descriptor_rx;

#define _AON_TIMER_
/****************************************************************************************
*							        Globals
*                                       *
****************************************************************************************/
volatile bool app_state = 0 ; /*!< flag to represent the application state*/
volatile bool start_advertisement = 0; /*!< flag to start advertisement*/
volatile bool advertisement_flag = false;/*!< to check if the device is in advertisement*/
//volatile bool notification_flag = false; /*!< flag to start notification*/
volatile bool disconnect_flag = false;	/*!< flag for disconnection*/
volatile bool hr_initializer_flag = 1; /*!< flag for initialization of hr for each category*/
//volatile bool notification_sent = true;
uint8_t second_counter = 0;	/*!< second_counter to count the time*/
uint16_t energy_expended_val = ENERGY_EXP_NORMAL; /*!< to count the energy expended*/
uint16_t energy_incrementor ;	/*!< energy incrementor for various heart rate values*/
uint16_t heart_rate_value = 0;//HEART_RATE_MIN_NORM; /*!< to count the heart rate value*/
uint16_t rr_interval_value = RR_VALUE_MIN; /*!< to count the rr interval value*/
uint8_t activity = 0; /*!< activiy which will determine the */
uint8_t prev_activity = DEFAULT_ACTIVITY;/*!< previous activity */
int8_t inc_changer	= 0;/*!< increment operator to change heart rate */
int8_t time_operator ;/*!< operator to change the seconds */
uint8_t hr_min_value;/*!<the minimum heart rate value*/
uint8_t hr_max_value;/*!<the maximum heart rate value*/
uint8_t energy_inclusion = 0;/*!<To check for including the energy in hr measurement*/

#define BATTERY_UPDATE_INTERVAL	(1) //1 second
#define BATTERY_MAX_LEVEL		(100)
#define BATTERY_MIN_LEVEL		(0)


#define APP_STACK_SIZE  (1024)
volatile unsigned char app_stack_patch[APP_STACK_SIZE];

/** flag to check if indication has been sent successfully over the air*/
/*volatile*/ bool indication_sent = true;

/** flag to check if notification has been sent successfully over the air*/
/*volatile*/ bool notification_sent = true;

/** Flag to change the events from mmgh to kpa and vice versa*/
/*volatile*/ bool units = APP_DEFAULT_VAL;

/** flag to send notifications */
/*volatile*/ bool notification_flag = false;

/** flag to send indication */
/*volatile*/ bool indication_flag = APP_DEFAULT_VAL;

/** Flag to identify user request for indication and notification*/
/*volatile*/ bool user_request_flag =  APP_DEFAULT_VAL;

/** Counter to maintain interval of indication*/
/*volatile*/ uint8_t timer_count = APP_DEFAULT_VAL;

/** flag to send one notification for one second*/
/*volatile*/ bool notify = 0;

/** flag to check the app state*/
/*volatile bool app_state;*/

/** flags for reversing the direction of characteristic*
 *       change for indication*/
/*volatile*/ int8_t operator_blp[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};

/** Current systolic value  in mmhg*/
uint16_t systolic_val_mmhg = SYSTOLIC_MIN_MMHG;

/** Current diastolic value in mmhg*/
uint16_t diastolic_val_mmhg = DIASTOLIC_MIN_MMHG;

/** Current map value in mmhg*/
uint16_t map_val_mmhg = MAP_MIN_MMHG;

/** Current systolic in kpa*/
uint16_t systolic_val_kpa = SYSTOLIC_MIN_KPA;

/** current diastolic value in kpa*/
uint16_t diastolic_val_kpa = DIASTOLIC_MIN_KPA;

/** current map value in kpa*/
uint16_t map_val_kpa = MAP_MIN_KPA;

/** Current pulse rate value in kpa*/
uint16_t pulse_rate_val = PULSE_RATE_MIN;

/** Current time stamp */
at_ble_prf_date_time_t time_stamp;

/* Intermediate Cuff Pressure Values for notification */
uint16_t interim_diastolic_mmhg = DIASTOLIC_MIN_MMHG;

uint16_t interim_diastolic_kpa = DIASTOLIC_MIN_KPA; 

uint16_t interim_systolic_mmhg = SYSTOLIC_MIN_MMHG;

uint16_t interim_systolic_kpa = SYSTOLIC_MIN_KPA;

uint16_t interim_map_mmhg = MAP_MIN_MMHG;

uint16_t interim_map_kpa = MAP_MIN_KPA;


/* To keep the app in execution mode */
bool app_exec = true;

bool isButton = true;
bool isTimer = false;
bool isIndication = false;
uint8_t g_blp_data[BLP_DATA_LEN];
uint8_t g_idx = 0;





uint8_t db_mem[1024] = {0};
bat_gatt_service_handler_t bas_service_handler;

bool volatile timer_cb_done = false;
bool volatile flag = true;
bool volatile battery_flag = true;
at_ble_handle_t bat_connection_handle;

void resume_cb(void);

/**
 * \Timer callback handler called on timer expiry
 */
static void aon_sleep_timer_callback(void)
{
	timer_cb_done = true;
	send_plf_int_msg_ind(USER_TIMER_CALLBACK, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
}


void gpio_led();

void gpio_led()
{
	struct gpio_config config_gpio_pin;
	gpio_get_config_defaults(&config_gpio_pin);
	config_gpio_pin.direction=GPIO_DIRECTION;
	gpio_pin_set_config(USER_LED,&config_gpio_pin);
}
//! [transfer_descriptor]

//! [setup]
//! [transfer_done_tx]
//-dmaaa prorgrams

static void transfer_done_tx(struct dma_resource* const resource )
{
	dma_start_transfer_job(&uart_dma_resource_rx);
}
//! [transfer_done_tx]

//! [transfer_done_rx]
static void transfer_done_rx(struct dma_resource* const resource )
{
	////dma_start_transfer_job(&uart_dma_resource_tx);
	//if(string[0]=='N')
	//gpio_pin_set_output_level(USER_LED,0);
	//else if(string[0]=='F')
	//gpio_pin_set_output_level(USER_LED,1);
	//else 
	//{
	//float a=string[1];
	//temperature= a/10;
	//if(string[0]=='-')
	//temperature=temperature*(-1);
//
	//}
	//dma_start_transfer_job(&uart_dma_resource_rx);


	//dma_start_transfer_job(&uart_dma_resource_tx);
	int_value=(float)string[0];
	float_value=(float)string[1];
	if((string[0] | string[1])==129)
	{
		//! if leopard gecko led0 sets, set this
		gpio_pin_set_output_level(LED_0_PIN, false);
	}
	else
	{
		// f leopard gecko led0 is off, put this off
		gpio_pin_set_output_level(LED_0_PIN, true);
	}

	if (string[0]=='C')
	{
	  heart_rate_value=string[1];
	}
	
	else if(string[0]=='B')
	 {
	    systolic_val_mmhg = string[0];//SYSTOLIC_MIN_MMHG;
	    diastolic_val_mmhg = string[1];//DIASTOLIC_MIN_MMHG;
	    map_val_mmhg = string[1];//MAP_MIN_MMHG;
	 }

	
	else if((string[0]<128) && (string[1]<128))
	{
		int_value=string[0];
		float_value=(float)(string[1])*(0.1);
		temperature=(int_value)+(float_value);
		battery_level=temperature;
		printf("%c", string[0]);
	}
	dma_start_transfer_job(&uart_dma_resource_rx);
}
//! [transfer_done_rx]

//! [config_dma_resource_tx]
static void configure_dma_resource_tx(struct dma_resource *resource)
{
//! [setup_tx_1]
	struct dma_resource_config config;
//! [setup_tx_1]

//! [setup_tx_2]
	dma_get_config_defaults(&config);
//! [setup_tx_2]

//! [setup_tx_3]
	config.des.periph = UART0TX_DMA_PERIPHERAL;
	config.des.enable_inc_addr = false;
	config.src.periph = UART0TX_DMA_PERIPHERAL;
	
//! [setup_tx_3]

//! [setup_tx_4]
	dma_allocate(resource, &config);
//! [setup_tx_4]
}
//! [config_dma_resource_tx]

//! [setup_dma_transfer_tx_descriptor]
static void setup_transfer_descriptor_tx(struct dma_descriptor *descriptor)
{

//! [setup_tx_5]
	dma_descriptor_get_config_defaults(descriptor);
//! [setup_tx_5]

//! [setup_tx_6]
	descriptor->buffer_size = BUFFER_LEN;
	descriptor->read_start_addr = (uint32_t)string;
	descriptor->write_start_addr = 
			(uint32_t)(&uart_instance.hw->TRANSMIT_DATA.reg);
//! [setup_tx_6]
}
//! [setup_dma_transfer_tx_descriptor]

//! [config_dma_resource_rx]
static void configure_dma_resource_rx(struct dma_resource *resource)
{
	//! [setup_rx_1]
	struct dma_resource_config config;
	//! [setup_rx_1]

	//! [setup_rx_2]
	dma_get_config_defaults(&config);
	//! [setup_rx_2]

	//! [setup_rx_3]
	config.src.periph = UART0RX_DMA_PERIPHERAL;
	config.src.enable_inc_addr = false;
	config.src.periph_delay = 1;
	
	
	//! [setup_rx_3]

	//! [setup_rx_4]
	dma_allocate(resource, &config);
	//! [setup_rx_4]
}
//! [config_dma_resource_rx]
/** @brief heart_rate_value_init will initializes the heart rate values
 *	 for simulation.
 *	 Based on the time different heart rate values are chosen to indicate
 *	 different activity.
 */
static void heart_rate_value_init(void)
{
	activity = second_counter / 40;

	if (activity != prev_activity) {		
		switch(activity) {
		case ACTIVITY_NORMAL:
			hr_min_value = HEART_RATE_MIN_NORM;
			hr_max_value = HEART_RATE_MAX_NORM;
			//heart_rate_value = hr_min_value;
			energy_incrementor = ENERGY_EXP_NORMAL;
			break;
			
		case ACTIVITY_WALKING:
			hr_min_value = HEART_RATE_MIN_WALKING;
			hr_max_value = HEART_RATE_MAX_WALKING;
			//heart_rate_value = hr_min_value;
			energy_incrementor = ENERGY_EXP_WALKING;
			break;
			
		case ACTIVITY_BRISK_WALKING:
			hr_min_value = HEART_RATE_MIN_BRISK_WALK;
			hr_max_value = HEART_RATE_MAX_BRISK_WALK;
			//heart_rate_value = hr_min_value;
			energy_incrementor = ENERGY_EXP_BRISK_WALKING;
			break;
			
		case ACTIVITY_RUNNING:
			hr_min_value = HEART_RATE_MIN_RUNNING;
			hr_max_value = HEART_RATE_MAX_RUNNING;
			//heart_rate_value = hr_min_value;
			energy_incrementor = ENERGY_EXP_RUNNING;
			break;
			
		case ACTIVITY_FAST_RUNNING:
			hr_min_value = HEART_RATE_MIN_FAST_RUNNING;
			hr_max_value = HEART_RATE_MAX_FAST_RUNNING;
			//heart_rate_value = hr_min_value;
			energy_incrementor = ENERGY_EXP_FAST_RUNNING;
			break;
		}
		prev_activity = activity;
	}
	
	if (heart_rate_value == hr_max_value) {
		//inc_changer = -1;
	} else if (heart_rate_value == hr_min_value) {
		//inc_changer = 1;
	}
}
//! [setup_dma_transfer_rx_descriptor]
static void setup_transfer_descriptor_rx(struct dma_descriptor *descriptor)
{
	//! [setup_rx_5]
	dma_descriptor_get_config_defaults(descriptor);
	//! [setup_rx_5]

	//! [setup_tx_6]
	descriptor->buffer_size = BUFFER_LEN;
	descriptor->read_start_addr =
			(uint32_t)(&uart_instance.hw->RECEIVE_DATA.reg);
	descriptor->write_start_addr = (uint32_t)string;
	//! [setup_tx_6]
}
//! [setup_dma_transfer_rx_descriptor]

//! [setup_usart]
static void configure_usart(void)
{
//! [setup_config]
	struct uart_config config_uart;
//! [setup_config]

//! [setup_config_defaults]
	uart_get_config_defaults(&config_uart);
//! [setup_config_defaults]

//! [setup_change_config]
	config_uart.baud_rate = 9600;
	config_uart.pin_number_pad[0] = UARRT0;
	config_uart.pin_number_pad[1] = UARRT1;
	config_uart.pin_number_pad[2] = UARRT2;
	config_uart.pin_number_pad[3] = UARRT3;
	config_uart.pinmux_sel_pad[0] = UARMT0;
	config_uart.pinmux_sel_pad[1] = UARMT1;
	config_uart.pinmux_sel_pad[2] = UARMT2;
	config_uart.pinmux_sel_pad[3] = UARMT3;
//! [setup_change_config]

//! [setup_set_config]
	while (uart_init(&uart_instance,
			EDBG_CDC_MODULE, &config_uart) != STATUS_OK) {
	}
//! [setup_set_config]

//! [enable_interrupt]
	uart_enable_transmit_dma(&uart_instance);
	uart_enable_receive_dma(&uart_instance);
//! [enable_interrupt]
}
//! [setup_usart]

//! [setup_callback]
static void configure_dma_callback(void)
{
//! [setup_callback_register]
	dma_register_callback(&uart_dma_resource_tx, transfer_done_tx, DMA_CALLBACK_TRANSFER_DONE);
	dma_register_callback(&uart_dma_resource_rx, transfer_done_rx, DMA_CALLBACK_TRANSFER_DONE);
//! [setup_callback_register]

//! [setup_enable_callback]
	dma_enable_callback(&uart_dma_resource_tx, DMA_CALLBACK_TRANSFER_DONE);
	dma_enable_callback(&uart_dma_resource_rx, DMA_CALLBACK_TRANSFER_DONE);
//! [setup_enable_callback]

//! [enable_inic]
	NVIC_EnableIRQ(PROV_DMA_CTRL0_IRQn);
//! [enable_inic]
}
//! [setup_callback]

//! [setup]

//-ble funcitons
static void ble_advertise (void)
{
	printf("\nAssignment 2.1 : Start Advertising");
	status = ble_advertisement_data_set();
	if(status != AT_BLE_SUCCESS)
	{
		printf("\n\r## Advertisement data set failed : error %x",status);
		while(1);
	}
	/* Start of advertisement */
	status = at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED,\
	AT_BLE_ADV_GEN_DISCOVERABLE,\
	NULL,\
	AT_BLE_ADV_FP_ANY,\
	1000,\
	655,\
	0);
	if(status != AT_BLE_SUCCESS)
	{
		printf("\n\r## Advertisement data set failed : error %x",status);
		while(1);
	}
}


///* Advertisement data set and Advertisement start */
static at_ble_status_t battery_service_advertise(void)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	
	if((status = ble_advertisement_data_set()) != AT_BLE_SUCCESS)
	{
		DBG_LOG("advertisement data set failed reason :%d",status);
		return status;
	}
	
	/* Start of advertisement */
	if((status = at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, APP_BAS_FAST_ADV, APP_BAS_ADV_TIMEOUT, 0)) == AT_BLE_SUCCESS)
	{
		DBG_LOG("BLE Started Adv");
		return AT_BLE_SUCCESS;
	}
	else
	{
		DBG_LOG("BLE Adv start Failed reason :%d",status);
	}
	return status;
}



/* Callback registered for AT_BLE_CONNECTED event*/
static at_ble_status_t ble_paired_cb (void *param)
{

	at_ble_handle_t vol_conn_handle;
	at_ble_connected_t *connected = (at_ble_connected_t *) param;
	at_ble_connection_params_t gap_conn_parameter;
	//vol_conn_handle = connected->handle;

	gap_conn_parameter.con_intv_min  = GAP_CONN_INTERVAL_MIN;
	gap_conn_parameter.con_intv_max  = GAP_CONN_INTERVAL_MAX;
	gap_conn_parameter.con_latency  = GAP_CONN_SLAVE_LATENCY;
	gap_conn_parameter.ce_len_min  = GAP_CE_LEN_MIN;
	gap_conn_parameter.ce_len_max  = GAP_CE_LEN_MAX;
	gap_conn_parameter.superv_to= GAP_SUPERVISION_TIMOUT;

	at_ble_connection_param_update(vol_conn_handle,&gap_conn_parameter);
   
    bat_connection_handle = connected->handle;
    #if !BLE_PAIR_ENABLE
    ble_paired_app_event(param);
    #else
    ALL_UNUSED(param);
    #endif
    return AT_BLE_SUCCESS;
	

	at_ble_pair_done_t *pair_params = param;
	printf("\nAssignment 3.2: Application paired ");
	/* Enable the HTP Profile */ 
	printf("\nAssignment 4.1: enable health temperature service "); 
	status = at_ble_htpt_enable(pair_params->handle, HTPT_CFG_INTERM_MEAS_NTF); 
	if(status != AT_BLE_SUCCESS)
	{ 
		printf("*** Failure in HTP Profile Enable"); 
		while(true); 
	}
	ALL_UNUSED(param);
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_DISCONNECTED event */
static at_ble_status_t ble_disconnected_cb (void *param)
{
    timer_cb_done = false;
    flag = true;
    
    //aon_sleep_timer_service_stop();
    battery_service_advertise();
	printf("\nAssignment 3.2: Application disconnected ");
	ble_advertise();
	ALL_UNUSED(param);
	return AT_BLE_SUCCESS;
}





/* Callback registered for AT_BLE_PAIR_DONE event from stack */
static at_ble_status_t ble_paired_app_event(void *param)
{
	timer_cb_done = false;
	ALL_UNUSED(param);
	return AT_BLE_SUCCESS;
}


/* Callback registered for AT_BLE_NOTIFICATION_CONFIRMED event from stack */
static at_ble_status_t ble_notification_confirmed_app_event(void *param)
{
	at_ble_cmd_complete_event_t *notification_status = (at_ble_cmd_complete_event_t *)param;
	if(!notification_status->status)
	{
		flag = true;
		DBG_LOG_DEV("sending notification to the peer success");
	}
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_CHARACTERISTIC_CHANGED event from stack */
static at_ble_status_t ble_char_changed_app_event(void *param)
{
	uint16_t device_listening;
	at_ble_characteristic_changed_t *char_handle = (at_ble_characteristic_changed_t *)param;

	if(bas_service_handler.serv_chars.client_config_handle == char_handle->char_handle)
	{
		device_listening = char_handle->char_new_value[1]<<8| char_handle->char_new_value[0];
		if(!device_listening)
		{
			//aon_sleep_timer_service_stop();
		}
		else
		{
			//aon_sleep_timer_service_init(1);
			//aon_sleep_timer_service_start(aon_sleep_timer_callback);
		}
	}
	return bat_char_changed_event(char_handle->conn_handle,&bas_service_handler, char_handle, &flag);
}







static const ble_event_callback_t app_gap_cb[] = {
	NULL,// AT_BLE_UNDEFINED_EVENTf
	NULL,// AT_BLE_SCAN_INFO
	NULL,// AT_BLE_SCAN_REPORT
	NULL,// AT_BLE_ADV_REPORT
	NULL,// AT_BLE_RAND_ADDR_CHANGED
	NULL,// AT_BLE_CONNECTED
	ble_disconnected_cb,// AT_BLE_DISCONNECTED
	NULL,// AT_BLE_CONN_PARAM_UPDATE_DONE
	NULL,// AT_BLE_CONN_PARAM_UPDATE_REQUEST
	ble_paired_cb,// AT_BLE_PAIR_DONE
	ble_conn_param_update_req,// AT_BLE_PAIR_REQUEST
	NULL,// AT_BLE_SLAVE_SEC_REQUEST
	NULL,// AT_BLE_PAIR_KEY_REQUEST
	NULL,// AT_BLE_ENCRYPTION_REQUEST
	NULL,// AT_BLE_ENCRYPTION_STATUS_CHANGED
	NULL,// AT_BLE_RESOLV_RAND_ADDR_STATUS
	NULL,// AT_BLE_SIGN_COUNTERS_IND
	NULL,// AT_BLE_PEER_ATT_INFO_IND
	NULL // AT_BLE_CON_CHANNEL_MAP_IND
};


//static void htp_temperature_read(void)
//{
	//float temperature;
	///* Read Temperature Value from IO1 Xplained Pro */
	//temperature = at30tse_read_temperature();
	///* Display temperature on com port */
	//#ifdef HTPT_FAHRENHEIT
	//printf("\nTemperature: %d Fahrenheit", (uint16_t)temperature);
	//#else
	//printf("\nTemperature: %d Deg Celsius", (uint16_t)temperature);
	//#endif
//}





static const ble_event_callback_t app_gatt_server_cb[] = {
	 NULL,//AT_BLE_NOTIFICATION_CONFIRMED 
	 NULL,//AT_BLE_INDICATION_CONFIRMED 
	 NULL,//AT_BLE_CHARACTERISTIC_CHANGED, 
	 NULL,//AT_BLE_CHARACTERISTIC_CONFIGURATION_CHANGED 
	 NULL,//AT_BLE_SERVICE_CHANGED_INDICATION_SENT 
	 NULL,//AT_BLE_WRITE_AUTHORIZE_REQUEST 
	 NULL,//AT_BLE_MTU_CHANGED_INDICATION 
	 NULL,//AT_BLE_MTU_CHANGED_CMD_COMPLETE 
	 NULL,//AT_BLE_CHARACTERISTIC_WRITE_CMD_CMP, 
	 NULL //AT_BLE_READ_AUTHORIZE_REQUEST 
};

static const ble_event_callback_t app_gatt_client_cb[] = {
	 NULL,//AT_BLE_NOTIFICATION_CONFIRMED 
	 NULL,//AT_BLE_INCLUDED_SERVICE_FOUND 
	 NULL,//AT_BLE_CHARACTERISTIC_FOUND 
	 NULL,//AT_BLE_DESCRIPTOR_FOUND 
	 NULL,//AT_BLE_DISCOVERY_COMPLETE
	 NULL,//AT_BLE_CHARACTERISTIC_READ_BY_UUID_RESPONSE 
	 NULL,//AT_BLE_CHARACTERISTIC_READ_MULTIBLE_RESPONSE 
	 NULL,//AT_BLE_CHARACTERISTIC_WRITE_RESPONSE 
	 NULL,//AT_BLE_NOTIFICATION_RECIEVED 
	 NULL //AT_BLE_INDICATION_RECIEVED 
};

static const ble_event_callback_t battery_app_gatt_server_cb[] = {
	ble_notification_confirmed_app_event,
	NULL,
	ble_char_changed_app_event,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

	

	void resume_cb(void)
	{
		init_port_list();
		//uart_init(UART_HW_MODULE_UART1,&uart_cfg);
		serial_console_init();
	}



static void htp_init (void) 
{ 
	
	printf("\nAssignment 4.1: Init Health temperature service "); 
	/* Create htp service in GATT database*/ 
	status = at_ble_htpt_create_db( HTPT_TEMP_TYPE_CHAR_SUP, HTP_TYPE_ARMPIT, 1, 30, 1, HTPT_AUTH, &htpt_conn_handle); 
	if (status != AT_BLE_SUCCESS)
	{ 
		printf("HTP Data Base creation failed"); 
		while(true); 
	} 
}
	
/* Timer callback */ 
static void timer_callback_handler(void) 
{ 
	/* Stop timer */ 
	hw_timer_stop(); 
	/* Set timer Alarm flag */ 
	Timer_Flag = true; 
	heart_rate_value_init();
	notification_flag = true;
	isTimer = true;
	
	send_plf_int_msg_ind(USER_TIMER_CALLBACK, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
	/* Restart Timer */ 
	hw_timer_start(10); 
}
	
/* Sending the temperature value after reading it from IO1 Xplained Pro */ 
static void htp_temperature_send(void) 
{ 
	at_ble_prf_date_time_t timestamp; 
	
	/* Read Temperature Value from IO1 Xplained Pro */ 
	//temperature = at30tse_read_temperature(); 
	#ifdef HTPT_FAHRENHEIT 
		temperature = (((temperature * 9.0)/5.0) + 32.0); 
	#endif 
	/* Read Temperature Value from IO1 Xplained Pro */ 
	timestamp.day = 1; 
	timestamp.hour = 9; 
	timestamp.min = 2; 
	timestamp.month = 8; 
	timestamp.sec = 36; 
	timestamp.year = 15; 
	/* Read Temperature Value from IO1 Xplained Pro */ 
	if(at_ble_htpt_temp_send(convert_ieee754_ieee11073_float((float)temperature), 
		&timestamp, 
		#ifdef HTPT_FAHRENHEIT 
			(at_ble_htpt_temp_flags)(HTPT_FLAG_FAHRENHEIT | HTPT_FLAG_TYPE), 
		#else 
			(at_ble_htpt_temp_flags)(HTPT_FLAG_CELSIUS | HTPT_FLAG_TYPE), 
		#endif 
		HTP_TYPE_ARMPIT, 
		1 
		) == AT_BLE_SUCCESS) 
	{ 
		#ifdef HTPT_FAHRENHEIT 
			//printf("\nTemperature: %d Fahrenheit", (uint16_t)temperature); 
		#else 
			//printf("\nTemperature: %d Deg Celsius", (uint16_t)temperature); 
		#endif 
	} 
	
}
	
static at_ble_status_t app_htpt_cfg_indntf_ind_handler(void *params) 
{ 
	at_ble_htpt_cfg_indntf_ind_t htpt_cfg_indntf_ind_params; 
	memcpy((uint8_t *)&htpt_cfg_indntf_ind_params, params, sizeof(at_ble_htpt_cfg_indntf_ind_t)); 
	if (htpt_cfg_indntf_ind_params.ntf_ind_cfg == 0x03) 
	{ 
		printf("Started HTP Temperature Notification"); 
		Temp_Notification_Flag = true; 
	} 
	else 
	{ 
		printf("HTP Temperature Notification Stopped"); 
		Temp_Notification_Flag = false; 
	} 
	return AT_BLE_SUCCESS; 
}

static const ble_event_callback_t app_htpt_handle[] = { 
	NULL, // AT_BLE_HTPT_CREATE_DB_CFM 
	NULL, // AT_BLE_HTPT_ERROR_IND 
	NULL, // AT_BLE_HTPT_DISABLE_IND 
	NULL, // AT_BLE_HTPT_TEMP_SEND_CFM 
	NULL, // AT_BLE_HTPT_MEAS_INTV_CHG_IND 
	app_htpt_cfg_indntf_ind_handler, // AT_BLE_HTPT_CFG_INDNTF_IND 
	NULL, // AT_BLE_HTPT_ENABLE_RSP 
	NULL, // AT_BLE_HTPT_MEAS_INTV_UPD_RSP 
	NULL // AT_BLE_HTPT_MEAS_INTV_CHG_REQ 
};

/* Register GAP callbacks at BLE manager level*/
static void register_ble_callbacks (void)
{
	/* Register GAP Callbacks */
	printf("\nAssignment 3.2: Register bluetooth events callbacks");
	status = ble_mgr_events_callback_handler(REGISTER_CALL_BACK,BLE_GAP_EVENT_TYPE,app_gap_cb);
	if (status != true)
	{
		printf("\n##Error when Registering SAMB11 gap callbacks");
	}
	status = ble_mgr_events_callback_handler(REGISTER_CALL_BACK, BLE_GATT_HTPT_EVENT_TYPE,app_htpt_handle);
	if (status != true)
	{
		printf("\n##Error when Registering SAMB11 htpt callbacks");
	}
}	
/** @brief hr_measurment_send sends the notifications after adding the hr values
 *	heart rate values starts @60bpm increments by 1 goes upto 255 bpm and
 *	restarts @60
 *  Energy Expended will be sent on every 10th notification,it starts @ 0 and
 *	increments by 20
 *  rr interval values, two rr interval values will be sent in every
 *	notification
 */
static void hr_measurment_send(void)
{
	uint8_t hr_data[HR_CHAR_VALUE_LEN];
	uint8_t idx = 0;
	
	if ((energy_expended_val == ENERGY_RESET) || (second_counter % 10 == energy_inclusion)) {
		hr_data[idx] = (RR_INTERVAL_VALUE_PRESENT | ENERGY_EXPENDED_FIELD_PRESENT);
		
		/* To send energy expended after 10 notifications after reset */
		if (energy_expended_val == ENERGY_RESET) {
			energy_inclusion = second_counter % 10 ;
		}
	} else {
		hr_data[idx] = RR_INTERVAL_VALUE_PRESENT ;
	}
	idx += 1;			
	DBG_LOG("Heart Rate: %f bpm", heart_rate_value);
	//heart_rate_value += (inc_changer);
//heart_rate_value=89;
	/* Heart Rate Value 8bit*/
	hr_data[idx++] = (uint8_t)heart_rate_value ;
	if (hr_data[0] & ENERGY_EXPENDED_FIELD_PRESENT) {
		memcpy(&hr_data[idx], &energy_expended_val, 2);
		idx += 2;	
	}
	
	/* Appending RR interval values*/	
	if (rr_interval_value >= RR_VALUE_MAX) {
		rr_interval_value = (uint8_t) RR_VALUE_MIN; 
	}	
	DBG_LOG_CONT("\tRR Values:(%d,%d)msec",
				rr_interval_value, rr_interval_value + 200);
	memcpy(&hr_data[idx], &rr_interval_value, 2);
	idx += 2;
	rr_interval_value += 200;
	memcpy(&hr_data[idx], &rr_interval_value, 2);
	idx += 2;
	rr_interval_value += 200;
	
	/*printing the user activity,simulation*/
	switch(activity) {
	case ACTIVITY_NORMAL:
		DBG_LOG_CONT(" User Status:Idle");
		break;
		
	case ACTIVITY_WALKING:
		DBG_LOG_CONT(" User Status:Walking");
		break;
		
	case ACTIVITY_BRISK_WALKING:
		DBG_LOG_CONT(" User status:Brisk walking");
		break;
		
	case ACTIVITY_RUNNING:
		DBG_LOG_CONT(" User status:Running");
		break;
		
	case ACTIVITY_FAST_RUNNING:
		DBG_LOG_CONT(" User Status:Fast Running");
		break;	
	}
	
	/* Printing the energy*/
	if ((hr_data[0] & ENERGY_EXPENDED_FIELD_PRESENT)) {
		DBG_LOG("Energy Expended :%d KJ\n", energy_expended_val);
		energy_expended_val += energy_incrementor;
	}

	/* Sending the data for notifications*/
	
	hr_sensor_send_notification(hr_data,idx);
}	
static void app_reset_handler(void)
{
	energy_expended_val = 0;
	DBG_LOG("Energy Expended is made '0'on user Reset");
}
/** @brief notification handler function called by the profile
 *	@param[in] notification_enable which will tell the state of the
 *  application
 */
static void app_notification_handler(uint8_t notification_enable)
{
	if (notification_enable == true) {
		DBG_LOG("Notification Enabled");
		hw_timer_start(NOTIFICATION_INTERVAL);
	} else {
		hw_timer_stop();
		notification_flag = false;
		DBG_LOG("Notification Disabled");
	}
}
/** @brief connected state handler
 *  @param[in] status of the application
 */
static at_ble_status_t app_connected_event_handler(void *params)
{
	app_state = true;
	LED_On(LED0);
	DBG_LOG("Enable the notification in app to listen "
	"heart rate or press the button to disconnect");
	advertisement_flag = false;
	notification_sent = true;
        ALL_UNUSED(params);
	return AT_BLE_SUCCESS;
}


static at_ble_status_t app_disconnected_event_handler(void *params)
{
	app_state = false;
	hw_timer_stop();
	notification_flag = false;
	energy_expended_val = ENERGY_EXP_NORMAL;
	second_counter = 0;
	activity = ACTIVITY_NORMAL;
	prev_activity = DEFAULT_ACTIVITY;
	energy_inclusion = 0;
	heart_rate_value_init();
	LED_Off(LED0);
	DBG_LOG("Press button to advertise");

	//Resetting all the simulated values
	
	interim_diastolic_mmhg = DIASTOLIC_MIN_MMHG;
	interim_diastolic_kpa = DIASTOLIC_MIN_KPA;
	interim_systolic_mmhg = SYSTOLIC_MIN_MMHG;
	interim_systolic_kpa = SYSTOLIC_MIN_KPA;
	interim_map_mmhg = MAP_MIN_MMHG;
	interim_map_kpa = MAP_MIN_KPA;
	if(string[0]=='B')
	{
	systolic_val_mmhg = string[0];//30;//SYSTOLIC_MIN_MMHG;
	diastolic_val_mmhg = string[1];//25;//DIASTOLIC_MIN_MMHG;
	map_val_mmhg = 15;//MAP_MIN_MMHG;
	systolic_val_kpa = SYSTOLIC_MIN_KPA;
	}
	diastolic_val_kpa = DIASTOLIC_MIN_KPA;
	map_val_kpa = MAP_MIN_KPA;
	pulse_rate_val = PULSE_RATE_MIN;
	units = !units;
	indication_sent = true;
	notification_sent = true;
	notify = false;
	timer_count = APP_DEFAULT_VAL;
	user_request_flag =  APP_DEFAULT_VAL;
	indication_flag = APP_DEFAULT_VAL;
	notification_flag = APP_DEFAULT_VAL;
	
	/* Starting advertisement */
	blp_sensor_adv();
	



	ALL_UNUSED(params);
	return AT_BLE_SUCCESS;
}
/** @brief hr_notification_confirmation_handler called by ble manager 
 *	to give the status of notification sent
 *  @param[in] at_ble_cmd_complete_event_t address of the cmd completion
 */	
at_ble_status_t app_notification_cfm_handler(void *params)
	{
	at_ble_cmd_complete_event_t event_params;
	memcpy(&event_params,params,sizeof(at_ble_cmd_complete_event_t));
	if (event_params.status == AT_BLE_SUCCESS) {
		DBG_LOG_DEV("App Notification Successfully sent over the air");
		notification_sent = true;
	} else {
		DBG_LOG_DEV("Sending Notification over the air failed");
		notification_sent = false;
	}
	return AT_BLE_SUCCESS;
}
static const ble_event_callback_t app_gap_handle[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	app_connected_event_handler,
	app_disconnected_event_handler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};
static const ble_event_callback_t app_gatt_server_handle[] = {
	app_notification_cfm_handler,
	app_indication_confirmation_handler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

//static const ble_event_callback_t battery_app_gatt_server_cb[] = {
	//ble_notification_confirmed_app_event,
	//NULL,
	//ble_char_changed_app_event,
	//NULL,
	//NULL,
	//NULL,
	//NULL,
	//NULL,
	//NULL,
	//NULL
//};


/** @brief app_indication_confirmation_handler called by ble manager 
 *	to give the status of notification sent
 *  @param[in] at_ble_cmd_complete_event_t address of the cmd completion
 */	
static at_ble_status_t app_indication_confirmation_handler(void *params)
{
	if (((at_ble_cmd_complete_event_t * )params)->status == AT_BLE_SUCCESS) {
		DBG_LOG_DEV("App Indication successfully sent over the air");
		indication_sent = true;
		user_request_flag = false;
		DBG_LOG("\r\nPress the button to receive the blood pressure parameters");
	} else {
		DBG_LOG_DEV("Sending indication over the air failed reason %x ",
								((at_ble_cmd_complete_event_t * )params)->status);
		indication_sent = false;
	}
	return AT_BLE_SUCCESS;
}


/** @brief Updating the time stamp
 *
 */
static void update_time_stamp(void)
{
	if (time_stamp.sec < SECOND_MAX)
	{
		time_stamp.sec++;
	}
	else
	{
		time_stamp.sec = 0;	
		if (time_stamp.min < MINUTE_MAX)
		{
			time_stamp.min++;
		}
		else
		{
			time_stamp.min = 0;
			if (time_stamp.hour < HOUR_MAX)
			{
				time_stamp.hour++;
			}
			else
			{
				time_stamp.hour = 0;
				if (time_stamp.day < DAY_MAX)
				{
					time_stamp.day++;
				}
				else
				{
					time_stamp.day = 1;
					if (time_stamp.month < MONTH_MAX)
					{
						time_stamp.month++;
					}
					else
					{
						time_stamp.month = 1;
						if (time_stamp.year < YEAR_MAX)
						{
							time_stamp.year++;
						} 
						else
						{
							time_stamp.year = 2015;
						}
					}
				}
			}
		}			
	}	
}

/** @brief initializes the time stamp with default time stamp
 *
 */
static void time_stamp_init(void)
{
	memset((uint8_t *)&time_stamp, 0, sizeof(at_ble_prf_date_time_t));
	time_stamp.year = 2015;
	time_stamp.day = 1;
	time_stamp.month = 1;
}

/** @brief blp_value_update which will change the blood pressure measurement operations
 *  @param[in] data to which the blood pressure parameter value to be appended
 *  @param[in] idx index where the value has to be updated
 *  @param[in] value_type which will determine which blood pressure parameter 
 */
static void blp_value_update(uint8_t *data, uint8_t idx_blp, uint16_t value, uint8_t value_type)
{
	uint8_t min_val = 0, max_val = 0;
	
	switch(value_type) {
	case SYSTOLIC_MMHG:	
		min_val = SYSTOLIC_MIN_MMHG;
		max_val = SYSTOLIC_MAX_MMHG;
	break;
	
	case DIASTOLIC_MMHG:
		min_val = DIASTOLIC_MIN_MMHG;
		max_val = DIASTOLIC_MAX_MMHG;
	break;
	
	case MAP_MMHG:
		min_val = MAP_MIN_MMHG;
		max_val = MAP_MAX_MMHG;
	break;
	
	case PULSE_RATE:
		min_val = PULSE_RATE_MIN;
		max_val = PULSE_RATE_MAX;
	break;
	
	case SYSTOLIC_KPA:
		min_val = SYSTOLIC_MIN_KPA;
		max_val = SYSTOLIC_MAX_KPA;
	break;
	
	case DIASTOLIC_KPA:
		min_val = DIASTOLIC_MIN_KPA;
		max_val = DIASTOLIC_MAX_KPA;
	break;
	
	case MAP_KPA:
		min_val = MAP_MIN_KPA;
		max_val = MAP_MAX_KPA;
	break;
	
	case INTERIM_SYS_MMHG:
		min_val = SYSTOLIC_MIN_MMHG;
		max_val = SYSTOLIC_MAX_MMHG;
	break;
	
	case INTERIM_SYS_KPA:
		min_val = SYSTOLIC_MIN_KPA;
		max_val = SYSTOLIC_MAX_KPA;
	break;
	}
	
	if (value == max_val) {
		operator_blp[value_type] = -1;
	} else if (value == min_val) {
		operator_blp[value_type] = 1;
	}
	memcpy((data + idx_blp),&value,2);
}
/** @brief sends the characteristic data for the profile to send indication
 *
 */
static void blp_char_indication(void)
{
	uint8_t blp_data[BLP_DATA_LEN];
	uint8_t idx_blp = 0;

	memset(blp_data, 0, sizeof(uint8_t) * BLP_DATA_LEN);
	idx_blp = 0;

	DBG_LOG("\n\n");
	
	DBG_LOG("The Blood Pressure Values are:");
	/* initializing flags to zero*/
	blp_data[0] = 0;
	/** Blood pressure measurement flags */
	if (units) {
		/** Units in mmhg*/
			blp_data[idx_blp] |= (0x1)  & ~(BLOOD_PRESSURE_UNITS_FLAG_MASK);
	} else {
		/** Units in kpa*/
		    blp_data[idx_blp] |= (0x1)  | BLOOD_PRESSURE_UNITS_FLAG_MASK ;
	} 
	
	/** Appending the flags for time stamp , pulse rate, user id , mm status */
	blp_data[idx_blp]	|= BLOOD_PRESSURE_TIME_FLAG_MASK;
	blp_data[idx_blp]	|= BLOOD_PRESSURE_PULSE_FLAG_MASK;
	blp_data[idx_blp]	|= BLOOD_PRESSURE_USERID_FLAG_MASK;
	blp_data[idx_blp++] |= BLOOD_PRESSURE_MMT_STATUS_FLAG_MASK;
	
	if (units) {
	    if(string[0]=='B')
		systolic_val_mmhg = string[0];//systolic_val_mmhg + (operator_blp[SYSTOLIC_MMHG]);
		blp_value_update(blp_data, idx_blp, systolic_val_mmhg, SYSTOLIC_MMHG);
		idx_blp += 2;
		DBG_LOG("%-12s", "Systolic");
		DBG_LOG_CONT("   %d mmhg", systolic_val_mmhg);

		if(string[0]=='B')
		diastolic_val_mmhg = string[1];//diastolic_val_mmhg + (operator_blp[DIASTOLIC_MMHG]);
		blp_value_update(blp_data, idx_blp, diastolic_val_mmhg, DIASTOLIC_MMHG);
		idx_blp += 2;
		DBG_LOG("%-12s", "Diastolic");
		DBG_LOG_CONT("   %d mmhg", diastolic_val_mmhg);

		map_val_mmhg = 15;//map_val_mmhg + (operator_blp[MAP_MMHG]);
		blp_value_update(blp_data, idx_blp, map_val_mmhg, MAP_MMHG);
		idx_blp += 2;
		DBG_LOG("%-12s", "Map");
		DBG_LOG_CONT("   %d mmhg", map_val_mmhg);
	} else {
		systolic_val_kpa = systolic_val_kpa + (operator_blp[SYSTOLIC_KPA]);
		blp_value_update(blp_data, idx_blp, systolic_val_kpa, SYSTOLIC_KPA);
		idx_blp += 2;
		DBG_LOG("%-12s", "Systolic");
		DBG_LOG_CONT("   %02d kpa", systolic_val_kpa);
		diastolic_val_kpa = diastolic_val_kpa + (operator_blp[DIASTOLIC_KPA]);
		blp_value_update(blp_data, idx_blp, diastolic_val_kpa, DIASTOLIC_KPA);
		idx_blp += 2;
		DBG_LOG("%-12s", "Diastolic");
		DBG_LOG_CONT("   %02d kpa", diastolic_val_kpa);
		map_val_kpa = map_val_kpa + (operator_blp[MAP_KPA]);
		blp_value_update(blp_data, idx_blp, map_val_kpa, MAP_KPA);
		idx_blp += 2;
		DBG_LOG("%-12s", "Map");
		DBG_LOG_CONT("   %02d kpa", map_val_kpa);
	}
		memcpy(&blp_data[idx_blp],(uint8_t*)&(time_stamp.year),2);
		idx_blp += 2;
		memcpy(&blp_data[idx_blp],(uint8_t*)&(time_stamp.month),1);
		idx_blp += 1;
		memcpy(&blp_data[idx_blp],(uint8_t*)&(time_stamp.day),1);
		idx_blp += 1;
		memcpy(&blp_data[idx_blp],(uint8_t*)&(time_stamp.hour),1);
		idx_blp += 1;
		memcpy(&blp_data[idx_blp],(uint8_t*)&(time_stamp.min),1);
		idx_blp += 1;
		memcpy(&blp_data[idx_blp],(uint8_t*)&(time_stamp.sec),1);
		idx_blp += 1;
		
	pulse_rate_val = pulse_rate_val + (operator_blp[PULSE_RATE]);
	blp_value_update(blp_data, idx_blp, pulse_rate_val, PULSE_RATE);
	idx_blp += 2;
	DBG_LOG("%-12s", "Pulserate");
	DBG_LOG_CONT("   %d bpm", pulse_rate_val);

	/** Appending User id */
	if (units) {
		blp_data[idx_blp++] = USERID_1;
	} else {
		blp_data[idx_blp++] = USERID_2;
	}
	
	/** Appending Measurement status field */
	blp_data[idx_blp++] = 0xf;
	blp_data[idx_blp++] = 0x0;
	
	blp_sensor_send_indication(blp_data,idx_blp);	
	
}

/** @brief sends the characteristic data for profile to send notification
 *
 */
static void blp_char_notification(void)
{
	uint8_t blp_data[BLP_DATA_LEN];	
	uint8_t idx_blp = 0;

	memset(blp_data, 0, sizeof(uint8_t) * BLP_DATA_LEN);
	idx_blp = 0;

	if (units) {
		/** Units in mmhg*/
		blp_data[idx_blp++] |= (0x1)  & ~(BLOOD_PRESSURE_UNITS_FLAG_MASK);
		blp_data[0] = blp_data[0] & 1;
		DBG_LOG("Cuff pressure  %d mmhg", interim_systolic_mmhg);
		blp_value_update(blp_data, idx_blp, interim_systolic_mmhg, INTERIM_SYS_MMHG);
		idx_blp += 2;
		interim_systolic_mmhg = interim_systolic_mmhg + (operator_blp[7]);
	} else {
		/** Units in kpa*/
		blp_data[idx_blp++] |=  (0x1)  | BLOOD_PRESSURE_UNITS_FLAG_MASK;
		blp_data[0] = blp_data[0] & 1;
		DBG_LOG("Cuff pressure  %02d kpa", interim_systolic_kpa);
		blp_value_update(blp_data, idx_blp, interim_systolic_kpa, INTERIM_SYS_KPA);
		idx_blp += 2;
		interim_systolic_kpa = interim_systolic_kpa + (operator_blp[8]);
	}

	/** Appending diastolic in kpa*/
	blp_data[idx_blp++] = 0;
	blp_data[idx_blp++] = 0;
	
	/** Appending map in kpa */
	blp_data[idx_blp++] = 0;
	blp_data[idx_blp++] = 0;
	
	blp_data[0]	|= BLOOD_PRESSURE_USERID_FLAG_MASK;
	
	/** Appending User id */
	if (units) {
		blp_data[idx_blp++] = USERID_1;
		} else {
		blp_data[idx_blp++] = USERID_2;
	}
	
	blp_sensor_send_notification(blp_data,idx_blp);
}

/** @brief indication handler function called by the profile
 *	@param[in] enable will give weather indication has to enabled
 *  or disabled.
 */
static void app_indication_handler(bool enable)
{
	uint8_t blp_data[BLP_DATA_LEN];
	uint8_t idx_blp = 0;	 

	memset(blp_data, 0, sizeof(uint8_t) * BLP_DATA_LEN);
	idx_blp = 0;

	indication_flag = enable;

	if (indication_flag) {
		DBG_LOG("Indications enabled by the remote device for blood pressure\n ");
		if (units) {
			blp_data[idx_blp] = 0;
			DBG_LOG("Systolic       %02d mmhg",systolic_val_mmhg);
			memcpy(&blp_data[idx_blp],&systolic_val_mmhg,2);
			idx_blp += 2;
			DBG_LOG("Diastolic      %02d mmhg",diastolic_val_mmhg);
			memcpy(&blp_data[idx_blp],&diastolic_val_mmhg,2);
			idx_blp += 2;
			DBG_LOG("Map            %02d mmhg",map_val_mmhg);
			memcpy(&blp_data[idx_blp],&map_val_mmhg,2);
			idx_blp += 2;
		} else {
			blp_data[idx_blp] = 0x1;
			memcpy(&blp_data[idx_blp],&systolic_val_kpa,2);
			idx_blp += 2;
			DBG_LOG("Systolic       %02d kpa",systolic_val_kpa);
			memcpy(&blp_data[idx_blp],&diastolic_val_kpa,2);
			idx_blp += 2;
			DBG_LOG("Diastolic      %02d kpa",diastolic_val_kpa);
			memcpy(&blp_data[idx_blp],&map_val_kpa,2);
			idx_blp += 2;
			DBG_LOG("Map            %02d kpa",map_val_kpa);
		}
		
		blp_data[0]	|= BLOOD_PRESSURE_PULSE_FLAG_MASK;
			DBG_LOG("Pulse rate     %d bpm",pulse_rate_val);
		memcpy(&blp_data[idx_blp],&pulse_rate_val,2);
		idx_blp += 2;
		/* DBG_LOG("Flags are %d and length is %d",blp_data[0],idx_blp); */

		isIndication = true;

		memcpy(g_blp_data, blp_data, sizeof(uint8_t) * BLP_DATA_LEN);
		g_idx = idx_blp;
		send_plf_int_msg_ind(USER_TIMER_CALLBACK, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);

		/* Sending the default notification for the first time */
		/* blp_sensor_send_indication(blp_data,idx_blp); */
	} else {
		DBG_LOG("Disabled indication by the remote server for blood pressure");
	}
}


/**
 * @brief Button Press Callback
 */
static void button_cb(void)
{
	isButton = true;
	send_plf_int_msg_ind(USER_TIMER_CALLBACK, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
}








int main(void)
{
	/**
	 * For make this QS work, disable the systick to stop task switch.
	 * Should not do it if you want the BLE functions.
	 */
	 indication_sent = true;
	 notification_sent = true;
	 units = APP_DEFAULT_VAL;
	 notification_flag = APP_DEFAULT_VAL;
	 indication_flag = 1;//APP_DEFAULT_VAL;
	 user_request_flag =  APP_DEFAULT_VAL;
	 timer_count = APP_DEFAULT_VAL;
	 notify = 0;
	 app_state = 0;
	 memset(operator_blp, 1, sizeof(int8_t) * 9);
	 systolic_val_mmhg = 30;//SYSTOLIC_MIN_MMHG;
	 diastolic_val_mmhg = 25;//DIASTOLIC_MIN_MMHG;
	 map_val_mmhg = 15;//MAP_MIN_MMHG;
	 systolic_val_kpa = SYSTOLIC_MIN_KPA;
	 diastolic_val_kpa = DIASTOLIC_MIN_KPA;
	 map_val_kpa = MAP_MIN_KPA;
	 pulse_rate_val = PULSE_RATE_MIN;
	 interim_diastolic_mmhg = DIASTOLIC_MIN_MMHG;
	 interim_diastolic_kpa = DIASTOLIC_MIN_KPA;
	 interim_systolic_mmhg = SYSTOLIC_MIN_MMHG;
	 interim_systolic_kpa = SYSTOLIC_MIN_KPA;
	 interim_map_mmhg = MAP_MIN_MMHG;
	 interim_map_kpa = MAP_MIN_KPA;
	 app_exec = true;
	 isButton = true;
	 isTimer = false;
	 isIndication = false;
	 memset(g_blp_data, 0, sizeof(uint8_t) * BLP_DATA_LEN);
	 g_idx = 0;


	 
at_ble_status_t status;




	platform_driver_init(); 
	acquire_sleep_lock(); 
	/* Initialize serial console */ 
	serial_console_init();
	
	
	
	 
	
	gpio_led();
	/* Hardware timer */ 
	hw_timer_init(); 
	/* Register the callback */ 
	hw_timer_register_callback(timer_callback_handler);
	/* Start timer */ 
	hw_timer_start(1);

	printf("\n\rSAMB11 BLE Application");
	/* initialize the BLE chip and Set the Device Address */
	ble_device_init(NULL);

	//
	//SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
//
////! [setup_init]
////! [init_system]
	//system_clock_config(CLOCK_RESOURCE_XO_26_MHZ, CLOCK_FREQ_26_MHZ);
//! [init_system]

//! [setup_usart]
	configure_usart();
//! [setup_usart]

//! [setup_dma_resource]
	configure_dma_resource_tx(&uart_dma_resource_tx);
	configure_dma_resource_rx(&uart_dma_resource_rx);
//! [setup_dma_resource]

//! [setup_transfer_descriptor]
	setup_transfer_descriptor_tx(&example_descriptor_tx);
	setup_transfer_descriptor_rx(&example_descriptor_rx);
//! [setup_transfer_descriptor]

//! [add_descriptor_to_resource]
	dma_add_descriptor(&uart_dma_resource_tx, &example_descriptor_tx);
	dma_add_descriptor(&uart_dma_resource_rx, &example_descriptor_rx);
//! [add_descriptor_to_resource]
	gpio_pin_set_output_level(USER_LED,1);// SETTING LED_0_PIN OFF
//! [configure_callback]
	configure_dma_callback();
//! [configure_callback]
//! [setup_init]
hr_sensor_init(NULL);
//! [main]
//! [main_1]
	dma_start_transfer_job(&uart_dma_resource_rx);
//! [main_1]

//! [endless_loop]
	
	///* Initialize the temperature sensor */ 
	//at30tse_init(); 
	///* configure the temperature sensor ADC */ 
	//at30tse_write_config_register(AT30TSE_CONFIG_RES(AT30TSE_CONFIG_RES_12_bit)); 
	///* read the temperature from the sensor */ 
	//htp_temperature_read();
	/* Initialize the htp service */ 


	htp_init();
	/* Initialize the button */
	gpio_init();
	button_init();
	button_register_callback(button_cb);


	/* Initialize the battery service */
	bat_init_service(&bas_service_handler, &battery_level);

	
	
	DBG_LOG("Initializing Blood Pressure Sensor Application");

	

	/* Initialize the blood pressure sensor profile */
	blp_sensor_init(NULL);

	/** Initializing the application time stamp*/
	time_stamp_init();

	

	/* Define the primary service in the GATT server database */
	if((status = bat_primary_service_define(&bas_service_handler))!= AT_BLE_SUCCESS)
	{
		DBG_LOG("defining battery service failed %d", status);
	}


	/* Register Bluetooth events Callbacks */
	register_ble_callbacks();


	/* Start Advertising process */ 
	ble_advertise();

	/* Triggering advertisement */
	blp_sensor_adv();


	battery_service_advertise();
	
	/* Register callbacks for gap related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GAP_EVENT_TYPE,
	app_gap_handle);
	
	/* Register callbacks for gatt server related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GATT_SERVER_EVENT_TYPE,
	battery_app_gatt_server_cb);
	start_advertisement = true;
	advertisement_flag = true;


	register_resume_callback(resume_cb);
	
	//ble_set_ulp_mode(BLE_ULP_MODE_SET);



	/* Registering the app_notification_handler with the profile */
	register_hr_notification_handler(app_notification_handler);

	/* Registering the app_reset_handler with the profile */
	register_hr_reset_handler(app_reset_handler);


	/* Registering the app_notification_handler with the profile */
	register_blp_notification_handler(app_notification_handler);
	
	/* Registering the app_indication_handler with the profile */
	register_blp_indication_handler(app_indication_handler);
		
	at_ble_tx_power_set(AT_BLE_TX_PWR_LVL_NEG_20_DB);	
	send_plf_int_msg_ind(USER_TIMER_CALLBACK, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);

	while (true) {


		//ble_event_task(655);
		
			ble_event_task(BLE_EVENT_TIMEOUT);


			if (timer_cb_done)
			{
				timer_cb_done = false;

				htp_temperature_send();
				/* send the notification and Update the battery level  */
				if(flag){
					if(bat_update_char_value(bat_connection_handle,&bas_service_handler, battery_level, &flag) == AT_BLE_SUCCESS)
					{
						DBG_LOG("Battery Level:%d%%", battery_level);
					}
					if(battery_level == BATTERY_MAX_LEVEL)
					{
						battery_flag = false;
					}
					else if(battery_level == BATTERY_MIN_LEVEL)
					{
						battery_flag = true;
					}
					if(battery_flag)
					{
						battery_level++;
					}
					else
					{
						battery_level--;
					}
				}
			}


			htp_temperature_send();

			/* send the notification and Update the battery level  */
			if(flag){
				if(bat_update_char_value(bat_connection_handle,&bas_service_handler, battery_level, &flag) == AT_BLE_SUCCESS)
				{
					DBG_LOG("Battery Level:%f%%", battery_level);
					
				}
			}

			if (start_advertisement == true || disconnect_flag == true) {
				/* button debounce delay*/
				/*delay_ms(350);*/
			}
			
			/* Flag to start advertisement */
			if (start_advertisement) {
				hr_sensor_adv();
				start_advertisement = false;
			}

			/* Flag to start notification */
			if (notification_flag) {
				LED_Toggle(LED0);
				if (notification_sent) {
					hr_measurment_send();
					} else {
					DBG_LOG("Previous notification not sent");
				}
				
				notification_flag = false;
			}

			/* Flag to disconnect with the peer device */
			if (disconnect_flag) {
				hr_sensor_disconnect();
				app_state = false;
				disconnect_flag = false;
			}





			if (isTimer == true) {
				if (user_request_flag) {
					timer_count++;
					notify = true;
				}

				update_time_stamp();

				hw_timer_start(TIMER_INTERVAL);

				isTimer = false;
			}

			

			if (isButton == true) {
				/* App connected state*/
				if (app_state) {
					if (user_request_flag == false) {
						if (indication_flag) {
							/** For changing the units for each button press*/
							units = !units;
						}

						if (indication_flag || notification_flag) {
							/** To trigger the blood pressure indication */
							user_request_flag = true;
							timer_count = 0;
						}
						if (notification_flag) {
							DBG_LOG("\r\nStarted sending Interim Cuff Pressure Values");
						}
					}
				}

				//isButton = false;
			}

			if (isIndication == true) {
				/* Sending the default notification for the first time */
				blp_sensor_send_indication(g_blp_data, g_idx);

				isIndication = false;
			}

			/* Checking for button press */
			if (user_request_flag ) {
				
				/*Sending notifications of interim cuff pressure*/
				
				if (timer_count < INDICATION_TIMER_VAL ) {
					
					/* checking for notification enabled */
					if (notification_flag) {
						
						/* Sending one notification per second */
						if (notify) {
							
							/* Checking for previous notification sent over the air */
							if (notification_sent) {
								blp_char_notification();
							}
							notify = 0;
						}
					}
				}
				
				if (timer_count == INDICATION_TIMER_VAL) {
					if (indication_flag) {
						/*Checking for previous indication sent over the  air */
						if (indication_sent) {
							
							/* Send a indication */
							blp_char_indication();
							} else {
							DBG_LOG("Previous indication is failed and device is disconnecting");
							blp_disconnection();
						}
					}
					timer_count = 0;
					user_request_flag = 0;
				}
			}
		





		
		if (Timer_Flag & notification_flag)
		{
			//temperature=35;
			htp_temperature_send();

			/* send the notification and Update the battery level  */
				if(flag){
					if(bat_update_char_value(bat_connection_handle,&bas_service_handler, battery_level, &flag) == AT_BLE_SUCCESS)
					{
						DBG_LOG("Battery Level:%d%%", battery_level);
					}
					if(battery_level == BATTERY_MAX_LEVEL)
					{
						battery_flag = false;
					}
					else if(battery_level == BATTERY_MIN_LEVEL)
					{
						battery_flag = true;
					}
					if(battery_flag)
					{
						battery_level++;
					}
					else
					{
						battery_level--;
					}
				}
		}
	}

}