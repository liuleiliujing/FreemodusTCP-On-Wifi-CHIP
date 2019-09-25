/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "sys_init.h"
#include "wifi_nvdm_config.h"
#include "wifi_lwip_helper.h"
#if defined(MTK_MINICLI_ENABLE)
#include "cli_def.h"
#endif

#include "bsp_gpio_ept_config.h"
#include "hal_sleep_manager.h"

#include "connsys_profile.h"
#include "wifi_api.h"

#include "hal.h"

#include "pocket_hub_config.h"
#include "type_common.h"


//#ifdef INCLUDE_FLASH_COMM
#include "flash.h"
//#endif

#include "g113i_uart1.h"
#include "g113i_svc_dev.h"


#ifdef MTK_ATCI_ENABLE
#include "atci.h"
#include "at_command_wifi.h"


/**
 * @brief This function is a task main function for processing the data handled by ATCI module.
 * @param[in] param is the task main function paramter.
 * @return    None
 */
static void atci_def_task(void *param)
{

    LOG_I(common, "enter atci_def_task!!\n\r");
    while (1) {
        atci_processing();
    }
}

#endif

extern void modbus_task();
int32_t wifi_station_port_secure_event_handler(wifi_event_t event, uint8_t *payload, uint32_t length);
int32_t wifi_scan_complete_handler(wifi_event_t event, uint8_t *payload, uint32_t length);
#ifdef MTK_SYSTEM_HANG_CHECK_ENABLE

#ifdef HAL_WDT_MODULE_ENABLED

void wdt_timeout_handle(hal_wdt_reset_status_t wdt_reset_status)
{
    printf("%s: stattus:%u\r\n", __FUNCTION__, (unsigned int)wdt_reset_status);
    /* assert 0 to trigger exception hanling flow */
    configASSERT(0);
}

static void wdt_init(void)
{
    hal_wdt_config_t wdt_init;
    wdt_init.mode = HAL_WDT_MODE_INTERRUPT;
    wdt_init.seconds = 15;
    hal_wdt_init(&wdt_init);
    hal_wdt_register_callback(wdt_timeout_handle);
    hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
}
#endif
#endif


/* for idle task feed wdt (DO NOT enter sleep mode)*/
void vApplicationIdleHook(void)
{

#ifdef MTK_SYSTEM_HANG_CHECK_ENABLE
#ifdef HAL_WDT_MODULE_ENABLED
 static uint32_t time1_start_count = 0;
 static uint32_t time1_end_count = 0;
 static uint32_t time1_count = 0;

 hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &time1_end_count);
 hal_gpt_get_duration_count(time1_start_count, time1_end_count, &time1_count);
 if(time1_count > 32768) {
    hal_wdt_feed(HAL_WDT_FEED_MAGIC);
 hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &time1_start_count);
 time1_count = 0;
	 }


#endif
#endif
}

int32_t wifi_init_done_handler(wifi_event_t event,
                                      uint8_t *payload,
                                      uint32_t length)
{
    LOG_I(common, "WiFi Init Done: port = %d", payload[6]);
    return 1;
}

#ifdef MTK_USER_FAST_TX_ENABLE
#include "type_def.h"

#define DemoPktLen 64
extern UINT_8 DemoPkt[];

extern uint32_t g_FastTx_Channel;
extern PUINT_8 g_PktForSend;
extern UINT_32 g_PktLen;
static void fastTx_init(uint32_t channel, PUINT_8 pPktContent, UINT_32 PktLen)
{
    g_FastTx_Channel = channel;
    g_PktForSend = pPktContent;
    g_PktLen = PktLen;
}
#endif


#if 0
/* --------------- ----------UART 1--功能模块----------------------------*/
/* ---------------------UART 1 参数初始化--------------*/
//#define VFIFO_SIZE (256)
#define VFIFO_SIZE (1024)
#define SEND_THRESHOLD_SIZE (80)
#define RECEIVE_THRESHOLD_SIZE (250)
#define RECEIVE_ALERT_SIZE (50)
#define UART_PROMPT_INFO "\r\n\r\nPlease input data to this UART1  port and watch it's output:\r\n"
#define UART_PROMPT_INFO_SIZE sizeof(UART_PROMPT_INFO)

#define UART_SUCCESS_INFO "example project test success.\n"
#define UART_SUCCESS_INFO_SIZE sizeof(UART_SUCCESS_INFO)

#define  UART_RCV_TEMPBUF_LEN (4096)

/* Buffer used by UART driver in dma mode must declare with ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN. */
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_uart_send_buffer[VFIFO_SIZE];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_uart_receive_buffer[VFIFO_SIZE];
static volatile bool g_uart_receive_event = false;

static uint32_t g_length= 0;

static uint32_t g_read_length= 0;


static uint16_t		g_pinput=0; 	/*接收缓冲输入的指针值*/
static uint16_t		g_poutput=0;	/*接收缓冲输出的指针值*/
static uint16_t		g_bufof=0;
static uint8_t	g_read_buf[UART_RCV_TEMPBUF_LEN];
static uint8_t	g_send_buf[UART_RCV_TEMPBUF_LEN];
/* Private functions ---------------------------------------------------------*/

/**
*@brief  User defined callback.
*@param[in] user_data: pointer to the data that user will use in this callback.
*@param[in] event: This enum shows UART event when an interrupt occurs.
*@return None.
*/
static void uart_read_from_input(hal_uart_callback_event_t event, void *user_data)
{
    uint8_t buffer[VFIFO_SIZE];
    uint32_t length;
	uint32_t i;
	uint32_t  dsend;
	uint16_t  index;
	
	//printf("\r\n -------uart_read_from_input   event=%x-------\r\n",event);	
	
    if (event == HAL_UART_EVENT_READY_TO_READ) 
	{


/*读取串口接收到的数据长度，*/
 /*若length大于0，这从DMA中读取数据*/

/*将DMA中的数据拷贝到局部的消息内存中*/
/*打印接收信息*/
/*发送消息到消息队列使用xQueueSendFromISR*/

#if defined(MT7682_DEMO_TEST)

       	 g_uart_receive_event = true;

		// memset(buffer,0,VFIFO_SIZE);

		length = hal_uart_get_available_receive_bytes(HAL_UART_1);
		printf("length=%d\r\n",length);
		
		g_length +=length;
		if(g_length >10000)
		{
		printf("g_length=%d\r\n",g_length);
		}
		
		if (0 == hal_uart_receive_dma(HAL_UART_1, buffer, length))
		{
				printf("\r\n -------hal_uart_receive_dma ==0 ------\r\n");
		}


		for(i=0;i<length;i++)
		{
			g_send_buf[i]= buffer[i];
		}		

		g_poutput =length;
		
               // printf("\r\n -------hal_uart_receive_dma  buffer ------\r\n");
				
		/*for(i=0;i<length;i++)
		{
			printf("b[%d]=%d ,",i,buffer[i]);
		}*/
		
		//printf("\r\n -------hal_uart_receive_dma  buffer ------\r\n");	

		//dsend =hal_uart_send_dma(HAL_UART_1, buffer, length);
		dsend =hal_uart_send_dma(HAL_UART_1, buffer, 1);
		 //printf("\r\n -------hal_uart_send_dma   dsend=%d-------\r\n",dsend);	
#endif		
		
    }
   else if(event == HAL_UART_EVENT_READY_TO_WRITE) 
   {
       /*置发送完毕标志*/
	   
#if defined(MT7682_DEMO_TEST)   
		printf("event=%x\r\n",event);	

		if(g_poutput)
		{
			length =g_poutput;
			dsend =hal_uart_send_dma(HAL_UART_1, g_send_buf, length);
			g_poutput=0;
		}
#endif	
	
   }
   else if(event == HAL_UART_EVENT_TRANSACTION_ERROR) 
   {
		printf("event=%x\r\n",event);

   }
   else
   {
	printf("\r\n event=%x\r\n",event);	
	
   }
	
}


static void uart1_init(void)
{
    hal_uart_config_t basic_config;
    hal_uart_dma_config_t dma_config;
   hal_uart_status_t   rev;
	uint32_t  dsend;


	 printf("\r\n -------uart1_init  sta-------\r\n");

    /* Step1: Call hal_pinmux_set_function() to set GPIO pinmux, if EPT tool was not used to configure the related pinmux.*/
    hal_gpio_init(HAL_GPIO_2);
    hal_gpio_init(HAL_GPIO_3);
    hal_pinmux_set_function(HAL_GPIO_2, HAL_GPIO_2_URXD1);
    hal_pinmux_set_function(HAL_GPIO_3, HAL_GPIO_3_UTXD1);

    /* Configure UART port with basic function */
    basic_config.baudrate = HAL_UART_BAUDRATE_115200;
    basic_config.parity = HAL_UART_PARITY_NONE;
    basic_config.stop_bit = HAL_UART_STOP_BIT_1;
    basic_config.word_length = HAL_UART_WORD_LENGTH_8;
    rev =hal_uart_init(HAL_UART_1, &basic_config);
	if(rev !=HAL_UART_STATUS_OK)
	{
		 printf("\r\n -------hal_uart_init   rev=%d-------\r\n",rev);
	}
	
    /*Step2: Configure UART port to dma mode. */
    dma_config.receive_vfifo_alert_size = RECEIVE_ALERT_SIZE;
    dma_config.receive_vfifo_buffer = g_uart_receive_buffer;
    dma_config.receive_vfifo_buffer_size = VFIFO_SIZE;
    dma_config.receive_vfifo_threshold_size = RECEIVE_THRESHOLD_SIZE;
    dma_config.send_vfifo_buffer = g_uart_send_buffer;
    dma_config.send_vfifo_buffer_size = VFIFO_SIZE;
    dma_config.send_vfifo_threshold_size = SEND_THRESHOLD_SIZE;

	rev=hal_uart_set_dma(HAL_UART_1, &dma_config);

	if(rev !=HAL_UART_STATUS_OK)
	{
		 printf("\r\n -------hal_uart_set_dma   rev=%d-------\r\n",rev);
	}
	 
	rev= hal_uart_register_callback(HAL_UART_1, uart_read_from_input, NULL);/*wangzaizhong*/
	if(rev !=HAL_UART_STATUS_OK)
	{
		 printf("\r\n -------hal_uart_register_callback   rev=%d-------\r\n",rev);
	}
	
    /* Print the prompt content to the test port */
    dsend =hal_uart_send_dma(HAL_UART_1, (const uint8_t *)UART_PROMPT_INFO, UART_PROMPT_INFO_SIZE);
 	printf("\r\n -------hal_uart_send_dma   dsend=%d-------\r\n",dsend);
	
	 printf("\r\n -------uart1_init  end -------\r\n");

}

#endif



/*************************TEST  STA*****************************************/
unsigned char  led_sys=0;
hal_gpio_data_t  g_gpio_input_test=0;

#if defined(MT7682_DEMO_TEST)
void led_sys_processing(void)
{
	hal_gpio_data_t data_pull_up;
	hal_gpio_status_t ret;
	
	if(led_sys)
	{
		led_sys=0;
		data_pull_up =HAL_GPIO_DATA_HIGH;
		/*printf("hal_gpio_set_input led_sys=%d\r\n",led_sys);*/
		ret = hal_gpio_set_output(HAL_GPIO_1, data_pull_up);
		if (HAL_GPIO_STATUS_OK != ret)
		{
			printf("hal_gpio_set_input failed\r\n");
			hal_gpio_deinit(HAL_GPIO_1);
			return;
		}	
	}
	else
	{
		led_sys=1;
		data_pull_up =HAL_GPIO_DATA_LOW;
		/*printf("hal_gpio_set_input led_sys=%d\r\n",led_sys);*/
		ret = hal_gpio_set_output(HAL_GPIO_1, data_pull_up);
		if (HAL_GPIO_STATUS_OK != ret)
		{
			printf("hal_gpio_set_input failed\r\n");
			hal_gpio_deinit(HAL_GPIO_1);
			return;
		}
	}

	/*输入测试*/
	ret = hal_gpio_get_input(HAL_GPIO_12, &data_pull_up);
	if (HAL_GPIO_STATUS_OK != ret)
	{
		printf("hal_gpio_set_input failed\r\n");
		hal_gpio_deinit(HAL_GPIO_1);
		return;
	}	
	if (g_gpio_input_test !=data_pull_up)
	{
		g_gpio_input_test =data_pull_up;
		printf("\r\n hal_gpio_input_test =%d !\r\n",g_gpio_input_test);
	}

		
}

#else
void led_sys_processing(void)
{
	hal_gpio_data_t data_pull_up;
	hal_gpio_status_t ret;
	
	if(led_sys)
	{
		led_sys=0;
		data_pull_up =HAL_GPIO_DATA_HIGH;
		ret = hal_gpio_set_output(HAL_GPIO_4, data_pull_up);
		//ret = hal_gpio_set_output(HAL_GPIO_11, data_pull_up);	
		//ret = hal_gpio_set_output(HAL_GPIO_12, data_pull_up);
		ret = hal_gpio_set_output(HAL_GPIO_13, data_pull_up);		
		if (HAL_GPIO_STATUS_OK != ret)
		{
			/*printf("hal_gpio_set_input failed\r\n");*/
			hal_gpio_deinit(HAL_GPIO_4);
			return;
		}	

		//printf("\r\n led_sys =%d !\r\n",led_sys);
	}
	else
	{
		led_sys=1;
		data_pull_up =HAL_GPIO_DATA_LOW;
		ret = hal_gpio_set_output(HAL_GPIO_4, data_pull_up);
		//ret = hal_gpio_set_output(HAL_GPIO_11, data_pull_up);		
		//ret = hal_gpio_set_output(HAL_GPIO_12, data_pull_up);
		ret = hal_gpio_set_output(HAL_GPIO_13, data_pull_up);		
		if (HAL_GPIO_STATUS_OK != ret)
		{
			/*printf("hal_gpio_set_input failed\r\n");*/
			hal_gpio_deinit(HAL_GPIO_4);
			return;
		}
		//printf("\r\n led_sys =%d !\r\n",led_sys);
	}

	
}

#endif

static void led_test_task(void *param)
{

   
	 printf("\r\n ---led_test_task---\r\n");
	 
    while (1) 
	{
		led_sys_processing();
		vTaskDelay(1000 / portTICK_RATE_MS);  

	}
}





/*******************TEST  END**************************************/

void printf_register_map(void)
{
    uint32_t i=0;
    volatile uint8_t *pdata;
    uint32_t read_add=0;
    uint32_t read_data=0;
    uint32_t write_add=0;	

#if 0
	printf("register :  bbpll_ctrl Base address: (+A2040000h) ! \n\r");
	read_add =0xA2040200;
	pdata =((volatile uint8_t *)read_add);
	for (i = 0u; i <4; i += 1u)
	{
		read_data = *(pdata+i);
		printf("register : 0x%x  = 0x%x ! \n\r",(read_add+i),read_data);
	}
	
	read_add =0xA2040300;
	pdata =((volatile uint8_t *)read_add);
	for (i = 0u; i <= 163; i += 1u)
	{
		read_data = *(pdata+i);
		printf("register : 0x%x  = 0x%x ! \n\r",(read_add+i),read_data);
	}

	read_add =0xA2040400;
	pdata =((volatile uint8_t *)read_add);
	for (i = 0u; i <20; i += 1u)
	{
		read_data = *(pdata+i);
		printf("register : 0x%x  = 0x%x ! \n\r",(read_add+i),read_data);
	}
	
	read_add =0xA2040700;
	pdata =((volatile uint8_t *)read_add);
	for (i = 0u; i <4; i += 1u)
	{
		read_data = *(pdata+i);
		printf("register : 0x%x  = 0x%x ! \n\r",(read_add+i),read_data);
	}

	
	printf("register : gpio_reg Base address: (+A20b0000h) ! \n\r");
	read_add =0xA20b0000;
	pdata =((volatile uint8_t *)read_add);
	for (i = 0u; i <114; i += 1u)
	{
		read_data = *(pdata+i);
		printf("register : 0x%x  = 0x%x ! \n\r",(read_add+i),read_data);
	}


	printf("register : EINT Base address: (+A2100000h)  ! \n\r");
	read_add =0xA2100300;
	pdata =((volatile uint8_t *)read_add);
	for (i = 0u; i <212; i += 1u)
	{
		read_data = *(pdata+i);
		printf("register : 0x%x  = 0x%x ! \n\r",(read_add+i),read_data);
	}

	read_add =0xA2100400;
	pdata =((volatile uint8_t *)read_add);
	for (i = 0u; i <128; i += 1u)
	{
		read_data = *(pdata+i);
		printf("register : 0x%x  = 0x%x ! \n\r",(read_add+i),read_data);
	}	
#endif

#if 1
	write_add =0xA2100328;/*wangzaizhong*/
	for (i = 0u; i <2; i += 1u)
	{
		pdata = ((volatile uint8_t *)(write_add+i));
		*pdata =0xff;
	}
		pdata = ((volatile uint8_t *)(write_add+i));
		*pdata =0xdf;		
		i += 1u;
		pdata = ((volatile uint8_t *)(write_add+i));
		*pdata =0xfd;	


/*	write_add =0xA210032b;

	pdata = ((volatile uint8_t *)(write_add+i));
	*pdata =0xff;
*/
	

	read_add =0xA2100320;
	pdata =((volatile uint8_t *)read_add);
	for (i = 0u; i <16; i += 1u)
	{
		read_data = *(pdata+i);
		printf("register : 0x%x  = 0x%x ! \n\r",(read_add+i),read_data);
	}

#endif

}


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

uint32_t time1_test= 0;

#ifdef MTK_USER_FAST_TX_ENABLE
    /* Customize Packet Content and Length */
    fastTx_init(11, DemoPkt, DemoPktLen);

 	printf("\r\n --fastTx_init-\r\n");
#endif

    /* Do system initialization, eg: hardware, nvdm, logging and random seed. */
    system_init();

    /* User initial the parameters for wifi initial process,  system will determin which wifi operation mode
     * will be started , and adopt which settings for the specific mode while wifi initial process is running*/

	printf("ssystem_init() ok!!\r\n");
#if 1	
    wifi_cfg_t wifi_config = {0};
    if (0 != wifi_config_init(&wifi_config)) {
        LOG_E(common, "wifi config init fail");
        return -1;
    }

	printf("wifi_config_init() ok!!\r\n");

    wifi_config_t config = {0};
    wifi_config_ext_t config_ext = {0};

    config.opmode = wifi_config.opmode;

    memcpy(config.sta_config.ssid, wifi_config.sta_ssid, 32);
    config.sta_config.ssid_length = wifi_config.sta_ssid_len;
    config.sta_config.bssid_present = 0;
    memcpy(config.sta_config.password, wifi_config.sta_wpa_psk, 64);
    config.sta_config.password_length = wifi_config.sta_wpa_psk_len;
    config_ext.sta_wep_key_index_present = 1;
    config_ext.sta_wep_key_index = wifi_config.sta_default_key_id;
    config_ext.sta_auto_connect_present = 1;
    config_ext.sta_auto_connect = 1;

    memcpy(config.ap_config.ssid, wifi_config.ap_ssid, 32);
    config.ap_config.ssid_length = wifi_config.ap_ssid_len;
    memcpy(config.ap_config.password, wifi_config.ap_wpa_psk, 64);
    config.ap_config.password_length = wifi_config.ap_wpa_psk_len;
    config.ap_config.auth_mode = (wifi_auth_mode_t)wifi_config.ap_auth_mode;
    config.ap_config.encrypt_type = (wifi_encrypt_type_t)wifi_config.ap_encryp_type;
    config.ap_config.channel = wifi_config.ap_channel;
    config.ap_config.bandwidth = wifi_config.ap_bw;
    config.ap_config.bandwidth_ext = WIFI_BANDWIDTH_EXT_40MHZ_UP;
    config_ext.ap_wep_key_index_present = 1;
    config_ext.ap_wep_key_index = wifi_config.ap_default_key_id;
    config_ext.ap_hidden_ssid_enable_present = 1;
    config_ext.ap_hidden_ssid_enable = wifi_config.ap_hide_ssid;
    config_ext.sta_power_save_mode = (wifi_power_saving_mode_t)wifi_config.sta_power_save_mode;

    /* Initialize wifi stack and register wifi init complete event handler,
     * notes:  the wifi initial process will be implemented and finished while system task scheduler is running,
     *            when it is done , the WIFI_EVENT_IOT_INIT_COMPLETE event will be triggered */
     printf( "wifi_init() config set !!\r\n");
#endif

#if 1	
    wifi_init(&config, &config_ext);

	printf( "wifi_init() ok!!\r\n");

    wifi_connection_register_event_handler(WIFI_EVENT_IOT_INIT_COMPLETE, wifi_init_done_handler);

//Castro-
    /* Tcpip stack and net interface initialization,  dhcp client, dhcp server process initialization*/
    lwip_network_init(config.opmode);
    lwip_net_start(config.opmode);
#endif

#if 1
#if defined(MTK_MINICLI_ENABLE)
    /* Initialize cli task to enable user input cli command from uart port.*/
    cli_def_create();
    cli_task_create();
#endif
#endif

#if 1
#ifdef MTK_ATCI_ENABLE
    /* init ATCI module and set UART port */
    atci_init(HAL_UART_2);
    /* create task for ATCI */
    xTaskCreate(atci_def_task, ATCI_TASK_NAME, ATCI_TASK_STACKSIZE / ((uint32_t)sizeof(StackType_t)), NULL, ATCI_TASK_PRIO, NULL);
#if defined(MTK_WIFI_AT_COMMAND_ENABLE)
    wifi_atci_example_init();
#endif

#endif
#endif

    /* Call this function to indicate the system initialize done. */
    SysInitStatus_Set();

#if 1
#ifdef MTK_SYSTEM_HANG_CHECK_ENABLE
#ifdef HAL_WDT_MODULE_ENABLED
    wdt_init();
#endif
#endif
#endif

   /* Enable I,F bits */
   /* __enable_irq();
    __enable_fault_irq();*/

	uart1_init();/*uart1初始化*/
	uart2_init();

	svc_dev_init();/*设备初始化*/

	// printf("\r\n ---xTaskCreate--led_test_task-\r\n");

	/*sys led test   正常代码删除*/
	//xTaskCreate(led_test_task, LED_TASK_NAME, LED_TASK_STACKSIZE, NULL, LED_TASK_PRIO, NULL);
	xTaskCreate(modbus_task, MODBUS_TASK_NAME, MODBUS_TASK_STACKSIZE, NULL, MODBUS_TASK_PRIO, NULL);

	//printf("\r\n --Create  led_test_task ok !\r\n");
	printf("\r\n --Create  modbus_task ok !\r\n");
	printf_register_map();/*wangzaizhong*/

	vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for (;;);
}

