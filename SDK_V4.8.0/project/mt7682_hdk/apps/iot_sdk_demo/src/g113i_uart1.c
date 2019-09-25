/* Copyright Statement:
 *

 */

#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Hal includes. */
#include "hal.h"
#include "hal_dcxo.h"

#include "nvdm.h"
#include "syslog.h"
#include "io_def.h"
#include "bsp_gpio_ept_config.h"

#include "memory_map.h"
#include "memory_attribute.h"
#include "sys_init.h"

#include "g113i_uart1.h"




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

 uint16_t		g_debug_exloop=1;

/* ---------------------UART 1 参数初始化结束--------------*/


/* Buffer used by UART driver in dma mode must declare with ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN. */
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_uart2_send_buffer[UART2_VFIFO_SIZE];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_uart2_receive_buffer[UART2_VFIFO_SIZE];
static volatile bool g_uart2_receive_event = false;

static uint32_t g_uart2_length= 0;
static uint32_t g_uart2_read_length= 0;

static uint16_t		g_uart2_pinput=0; 	/*接收缓冲输入的指针值*/
static uint16_t		g_uart2_poutput=0;	/*接收缓冲输出的指针值*/
static uint16_t		g_uart2_bufof=0;
static uint8_t	g_uart2_read_buf[UART2_RCV_TEMPBUF_LEN];
static uint8_t	g_uart2_send_buf[UART2_RCV_TEMPBUF_LEN];
/* ---------------------UART 2 参数初始化结束--------------*/

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

/*#if defined(MT7682_DEMO_TEST)*/
#if 1
       	 g_uart_receive_event = true;

		// memset(buffer,0,VFIFO_SIZE);

		length = hal_uart_get_available_receive_bytes(HAL_UART_1);
		printf("uart1 length=%d\r\n",length);
		
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
				
		for(i=0;i<length;i++)
		{
			printf("b[%d]=%d ,",i,buffer[i]);
		}
		
		//printf("\r\n -------hal_uart_receive_dma  buffer ------\r\n");	

		//dsend =hal_uart_send_dma(HAL_UART_1, buffer, length);
		
		//if(g_debug_exloop ==1)
		//{
			dsend =hal_uart_send_dma(HAL_UART_1, buffer, length);
		//}
		 //printf("\r\n -------hal_uart_send_dma   dsend=%d-------\r\n",dsend);	
#endif		
		
    }
   else if(event == HAL_UART_EVENT_READY_TO_WRITE) 
   {
       /*置发送完毕标志*/
	   
/*#if defined(MT7682_DEMO_TEST)   */
#if 1
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

/* Private functions ---------------------------------------------------------*/

/**
*@brief  User defined callback.
*@param[in] user_data: pointer to the data that user will use in this callback.
*@param[in] event: This enum shows UART event when an interrupt occurs.
*@return None.
*/
static void uart2_read_from_input(hal_uart_callback_event_t event, void *user_data)
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

/*#if defined(MT7682_DEMO_TEST)*/
#if 1
       	 g_uart2_receive_event = true;

		// memset(buffer,0,VFIFO_SIZE);

		length = hal_uart_get_available_receive_bytes(HAL_UART_2);
		printf("uart2_length :::uart2_length=%d\r\n",length);
		
		g_uart2_length +=length;
		if(g_uart2_length >10000)
		{
		printf("g_uart2_length=%d\r\n",g_uart2_length);
		}
		
		if (0 == hal_uart_receive_dma(HAL_UART_2, buffer, length))
		{
				printf("\r\n ----HAL_UART_2---hal_uart_receive_dma ==0 ------\r\n");
		}


		for(i=0;i<length;i++)
		{
			g_uart2_send_buf[i]= buffer[i];
		}		

		g_uart2_poutput =length;
		
               // printf("\r\n -------hal_uart_receive_dma  buffer ------\r\n");
				
		/*for(i=0;i<length;i++)
		{
			printf("b[%d]=%d ,",i,buffer[i]);
		}*/
		
		//printf("\r\n -------hal_uart_receive_dma  buffer ------\r\n");	

		//dsend =hal_uart_send_dma(HAL_UART_1, buffer, length);
		dsend =hal_uart_send_dma(HAL_UART_2, buffer, length);
		 //printf("\r\n -------hal_uart_send_dma   dsend=%d-------\r\n",dsend);	
#endif		
		
    }
   else if(event == HAL_UART_EVENT_READY_TO_WRITE) 
   {
       /*置发送完毕标志*/
	   
/*#if defined(MT7682_DEMO_TEST)   */
#if 1
		printf(" uart2 event=%x\r\n",event);	

		if(g_uart2_poutput)
		{
			length =g_uart2_poutput;
			dsend =hal_uart_send_dma(HAL_UART_2, g_uart2_send_buf, length);
			g_uart2_poutput=0;
		}
#endif	
	
   }
   else if(event == HAL_UART_EVENT_TRANSACTION_ERROR) 
   {
		printf(" uart2  event=%x\r\n",event);

   }
   else
   {
	printf("\r\n uart2 event=%x\r\n",event);	
	
   }
	
}

void uart1_init(void)
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
		 printf("\r\n -------hal_uart1_init   rev=%d-------\r\n",rev);
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
		 printf("\r\n ----HAL_UART_1---hal_uart_set_dma   rev=%d-------\r\n",rev);
	}
	 
	rev= hal_uart_register_callback(HAL_UART_1, uart_read_from_input, NULL);/*wangzaizhong*/
	if(rev !=HAL_UART_STATUS_OK)
	{
		 printf("\r\n -------hal_uart_register_callback   rev=%d-------\r\n",rev);
	}
	
    /* Print the prompt content to the test port */
    dsend =hal_uart_send_dma(HAL_UART_0, (const uint8_t *)UART_PROMPT_INFO, UART_PROMPT_INFO_SIZE);
 	printf("\r\n -------hal_uart_send_dma   dsend=%d-------\r\n",dsend);
	
	 printf("\r\n -------uart1_init  end -------\r\n");

}



/*********************************************************************************************/
void uart2_init(void)
{
    hal_uart_config_t basic_config;
    hal_uart_dma_config_t dma_config;
   hal_uart_status_t   rev;
	uint32_t  dsend;


	 printf("\r\n -------uart2_init  sta-------\r\n");

    /* Step1: Call hal_pinmux_set_function() to set GPIO pinmux, if EPT tool was not used to configure the related pinmux.*/
    hal_gpio_init(HAL_GPIO_11);
    hal_gpio_init(HAL_GPIO_12);
    hal_pinmux_set_function(HAL_GPIO_11, HAL_GPIO_11_URXD2);
    hal_pinmux_set_function(HAL_GPIO_12, HAL_GPIO_12_UTXD2);

    /* Configure UART port with basic function */
    basic_config.baudrate = HAL_UART_BAUDRATE_115200;
    basic_config.parity = HAL_UART_PARITY_NONE;
    basic_config.stop_bit = HAL_UART_STOP_BIT_1;
    basic_config.word_length = HAL_UART_WORD_LENGTH_8;
    rev =hal_uart_init(HAL_UART_2, &basic_config);
	if(rev !=HAL_UART_STATUS_OK)
	{
		 printf("\r\n ---HAL_UART_2----hal_uart2_init   rev=%d-------\r\n",rev);
	}
	
    /*Step2: Configure UART port to dma mode. */
    dma_config.receive_vfifo_alert_size = UART2_RECEIVE_ALERT_SIZE;
    dma_config.receive_vfifo_buffer = g_uart2_receive_buffer;
    dma_config.receive_vfifo_buffer_size = UART2_VFIFO_SIZE;
    dma_config.receive_vfifo_threshold_size = UART2_RECEIVE_THRESHOLD_SIZE;
    dma_config.send_vfifo_buffer = g_uart2_send_buffer;
    dma_config.send_vfifo_buffer_size = UART2_VFIFO_SIZE;
    dma_config.send_vfifo_threshold_size = UART2_SEND_THRESHOLD_SIZE;

	rev=hal_uart_set_dma(HAL_UART_2, &dma_config);

	if(rev !=HAL_UART_STATUS_OK)
	{
		 printf("\r\n ----HAL_UART_2---hal_uart_set_dma   rev=%d-------\r\n",rev);
	}
	 
	rev= hal_uart_register_callback(HAL_UART_2, uart2_read_from_input, NULL);/*wangzaizhong*/
	if(rev !=HAL_UART_STATUS_OK)
	{
		 printf("\r\n ---HAL_UART_2----hal_uart_register_callback   rev=%d-------\r\n",rev);
	}
	
    /* Print the prompt content to the test port */
    dsend =hal_uart_send_dma(HAL_UART_2, (const uint8_t *)UART2_PROMPT_INFO, UART2_PROMPT_INFO_SIZE);
 	printf("\r\n ------HAL_UART_2-hal_uart_send_dma   dsend=%d-------\r\n",dsend);
	
	 printf("\r\n -------uart2_init  end -------\r\n");

}




