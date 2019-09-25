/************************************************************************/
/*	  瑞斯康达科技发展有限公司			版权所有						*/
/************************************************************************/
/*	 @@BEGAIN_INTERNAL_LEGAL@@											*/
/*																		*/
/*					 Copyright(C) Description							*/
/* Raisecom	 Science & Technology Development Co.,Ltd. Beijing,	China	*/
/*	   Unpublished work-rights reserved	under the China	Copyright Act.	*/
/*	   Use,duplication,	or disclosure by the government	is subject to	*/
/*	   restrictions	set	forth in the CDTT commercial license agreement.	*/
/*																		*/
/*	 @@END_INTERNAL_LEGAL@@												*/
/************************************************************************/
/*	 程序文件名：Svc_dev.c												*/
/*	 Filename  ：Svc_dev.c												*/
/*	 作者	   ：李静芳													*/
/*	 Author	   ：lijingfang												*/
/*	 日期	   ：2018-12-26												*/
/*	 Date	   ：2018-12-26												*/
/*	 版本	   ：1.0													*/
/*	 Version   ：1.0													*/
/*	 实现目的  ：业务盘中全部功能函数                    				*/
/*	 Purpose   ：														*/
/*																		*/
/************************************************************************/


/************************************************************************/
/*   文件修改记录:												 		*/
/*   [序号, MM/DD/YYYY, 版本, 修改者, 修改内容]							*/
/*																		*/
/*	  															 		*/
/*																 		*/
/*   Modified History:													*/
/*	 [Index, MM/DD/YYYY, Version, Author, Content]						*/
/************************************************************************/


//#ifdef	INCLUDE_SVC_DEV

#if 0
#include <string.h>
#include "macro_common.h"
#include "Trace.h"
#include "toolkit.h"
#include "uart.h"
#include "uart0.h"
#include "console.h"
#include "ibc.h"
#include "ibc_rc002.h"
#include "bcm53115_drv.h"
#include "bcm53115_gphy_mii.h"
#include "xrt83vsh38.h"
#include "ra1027.h"
#include "tmr1.h"
#include "sfp.h"
#include "faultpass.h"
#include "svc_system.h"
#include "svc_dev.h"
#include "svc_rdev_ibc.h"
#include "flash.h"
#include "x_modem.h"
#include "fileloader.h"
#include "iap.h"
#include "spi.h"

#include "type_common.h"
#include "hal_gpio.h"
#include "flash.h"
#include "spi.h"
#endif

#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
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

#include "type_common.h"

#include "hal_gpio.h"
#include "flash.h"
#include "spi.h"

#include "g113i_uart1.h"
#include "g113i_svc_dev.h"

/*lint -save -e534 -e641*/



/*************************************************************************/
/**UART1 从DMA中接收消息*/
QueueHandle_t g_uart1_read_msgQ_id = NULL;
#define	UART1_RECEIVE_MSG_MAX 	20      /*UART1 从DMA中接收消息的最大个数*/
/*************************************************************************/
/**UART1 接收的业务数据消息*/
QueueHandle_t g_svc_read_data_msgQ_id = NULL;
#define	SVC_RECEIVE_DATA_MSG_MAX 	20      /*UART1 从DMA中接收消息的最大个数*/
/*************************************************************************/
/**UART1 接收的业务管理消息*/
QueueHandle_t g_svc_read_manage_msgQ_id = NULL;
#define	SVC_RECEIVE_MANAGE_MSG_MAX 	20    /*UART1 从DMA中接收消息的最大个数*/
/*************************************************************************/



#if 1
/* FLASH读写使能禁止控制 */    
STATUS flash_enable(e_flash_cs_ena	cs_ena);

/*spi 发送一个字节*/
STATUS flash_spi_send_byte_msb(OTP_UINT8 data);

/*spi接收一个字节*/
OTP_UINT8 flash_spi_rcv_byte_msb(void);

const falsh_m25p16_create_param G_FLASH_M25P16_INIT_PARAM = {

	flash_spi_rcv_byte_msb,		/*注册的底层读FALSH 一个字节函数*/
	flash_spi_send_byte_msb,	/*注册的底层写FALSH 一个字节函数*/
	flash_enable,				/*注册的底层FLASH芯片使能控制函数*/
};

#if 0
/*flieloader*/
const fileload_global_info G_FILELOAD_INIT_PARAM = {

	256,					/*FLASH页宽度*/
	0x10000,				/*FLASH扇区宽度*/

	0x007C0000,				/*mcu 的FLASH首扇区地址0x007C0000*/
	0x007D0000,				/*mcu 的FLASH结束扇区地址0x007D0000*/	

	0x003E0000,				/*fpga 的FLASH首扇区地址0x003E0000*/
	0x007B0000,				/*fpga 的FLASH结束扇区地址0x007B0000*/	

	/*由FLASH驱动提供，可以为外部或ARM内部FLASH*/
	flash_sector_erase,		/*flash 清扇区函数*/
	flash_write_page,		/*flash 页写函数*/	
	flash_read_page,		/*flash 页读函数*/	

	/*由FLASH驱动提供*/
	0x007DFF00,				/*mcu升级标志存储区扇区地址0x007DFF00*/
	0x007BFF00,				/*fpga升级标志存储区扇区地址0x007BFF00*/	
	
	flash_page_write_zero,	/*flash升级文件正确完成后，修改升级标志函数*/
};
#endif


/******************************************************************************
*flash_enable    FLASH读写使能禁止控制
*
*Input:
*	cs_ena --FLASH使能禁止控制
*Output:
*	无
*Return:
*	OK 		--正确
*	ERROR 	--错误
*/
STATUS flash_enable(e_flash_cs_ena	cs_ena)
{

	if(FLASH_CS_DISABLE ==cs_ena)
	{
		spi_cs_flash(0);
	}
	else
	{
		spi_cs_flash(1);
	}


	return OK;
}

/******************************************************************************
*flash_spi_send_byte_msb    FLASH利用SPI发送一个字节
*
*Input:
*	data --发送一个字节
*Output:
*	无
*Return:
*	OK 		--正确
*	ERROR 	--错误
*/
STATUS flash_spi_send_byte_msb(OTP_UINT8 data)
{
	spi_send_byte_msb_flash(data);
	return OK;
}

/******************************************************************************
*flash_spi_rcv_byte_msb    FLASH利用SPI接收一个字节
*
*Input:
*	无
*Output:
*	接收一个字节
*Return:
*	OK 		--正确
*	ERROR 	--错误
*/
OTP_UINT8 flash_spi_rcv_byte_msb(void)
{
	OTP_UINT8 retdata = 0;

	retdata = spi_rcv_byte_msb_flash();

	return retdata;
}


/********************************************************************
* svc_dev_init -- 业务盘相关操作初始化
*
* PARAMETERS - As follow:
*
* RETURNS:	
*
* EXAMPLE:
* ...
*/
void svc_dev_ibc_init(void)
{	



	/*消息队列的创建*/
	/*创建串口数据接收消息队列*/
	g_uart1_read_msgQ_id = xQueueCreate(UART1_RECEIVE_MSG_MAX, sizeof(uart1_receive_message_struct_t));

	if (NULL ==g_uart1_read_msgQ_id )
	{
		printf("\r\n xQueueCreate g_uart1_read_msgQ_id fail !\r\n");
	}


	/*创建数据业务接收消息队列*/
	g_svc_read_data_msgQ_id = xQueueCreate(SVC_RECEIVE_DATA_MSG_MAX, sizeof(svc_receive_data_msg_struct_t));

	if (NULL ==g_svc_read_data_msgQ_id )
	{
		printf("\r\n xQueueCreate g_svc_read_data_msgQ_id fail !\r\n");
	}


	/*创建管理业务接收消息队列*/
	g_svc_read_manage_msgQ_id = xQueueCreate(SVC_RECEIVE_MANAGE_MSG_MAX, sizeof(svc_receive_manage_msg_struct_t));

	if (NULL ==g_svc_read_manage_msgQ_id )
	{
		printf("\r\n xQueueCreate svc_receive_manage_msg_struct_t fail !\r\n");
	}
	 


}






/********************************************************************
* svc_dev_init -- 业务盘相关操作初始化
*
* PARAMETERS - As follow:
*
* RETURNS:	
*
* EXAMPLE:
* ...
*/
void svc_dev_init(void)
{	

#if 1
	OTP_UINT16 i = 0;
	LOCAL OTP_UINT8 temp[256] = {0};
#endif

	/* 初始化flash  wangzaizhong add*/
	if(OK != flash_init_mx52l(&G_FLASH_M25P16_INIT_PARAM))
	{
		//ASSERT(0);
	}	

	/*flash访问调试*/
#if 0

	printf("\r\n ---svc_dev_init---flash test sta --- \r\n");

	memset(temp, 0, 256);
	(OTP_VOID)flash_sector_erase(0x007C0000, 0x007C0000);

	for(i = 0; i < 256; i++)
	{
		(OTP_VOID)flash_read_byte(0x007C0000+i, &temp[i]);
	}

	for(i = 0; i < 256; i++)
	{
		temp[i] = i;
	}
	(OTP_VOID)flash_write_page(0x007C0000, temp, 256);

	memset(temp, 0, 256);
	for(i = 0; i < 256; i++)
	{
		(OTP_VOID)flash_read_byte(0x007C0000+i, &temp[i]);
	}

	for(i = 0; i < 200; i++)
	{
		printf("temp[%d] = %d;  ",i,temp[i]);
	}

	(OTP_VOID)flash_sector_erase(0x007C0000, 0x007C0000);

	printf("\r\n ---svc_dev_init---flash test end--- \r\n");
	
#endif




}









#endif


/*lint -restore*/

//#endif
