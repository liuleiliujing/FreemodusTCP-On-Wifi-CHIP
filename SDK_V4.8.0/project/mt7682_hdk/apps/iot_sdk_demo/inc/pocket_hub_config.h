
/************************************************************************/
/*	  瑞斯康达科技发展有限公司			版权所有		*/
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
/*	 程序文件名： pocket_hub_config.h								*/
/*	 Filename  ： pocket_hub_config.h								*/
/*	 作者	   ： 											*/
/*	 Author	   ： wangzaizhong										*/
/*	 日期	   ： 2019/08/08											*/
/*	 Date	   ： 2019/08/08											*/
/*	 版本	   ： 1.0													*/
/*	 Version   ： 1.0													*/
/*	 实现目的  ： pocket_hub产品配置文件					*/
/*	 Purpose   ：														*/
/*																		*/
/************************************************************************/


/*产品型号*/
#ifndef	_COMX_POCKET_HUB
#define	_COMX_POCKET_HUB

/********************************************************************************/
/*part1:************************体系结构定义  ***********************************/
/********************************************************************************/
/*																							*/
/*																							*/
/*																							*/
/*********************************************************************************/


/*1为仿真环境，0为真实环境*/
#define OTP_RUN_SIMPC 		0

/*小端模式*/
#define OTP_LITTLE_ENDIAN

/*定义MCU类型*/
#define  INCLUDE_MCU_STM32F10X

/*********************************************************************************/
/*part2:************************平台模块剪裁  *******************************/
/*********************************************************************************/
/*	可以根据具体产品对平台提供的模块进行裁剪使用										  */	
/*																				*/
/*																				*/
/*********************************************************************************/
/*保护DEV_MAIN模块*/
#define INCLUDE_DEV_MAIN

/*包含IBC模块*/
#define INCLUDE_IBC

/*包含IBC_RC006协议处理模块*/
#define INCLUDE_IBC_RC006

/*包含SVCDEV_RC006 模块*/
#define INCLUDE_SVCDEV_RC006

/*包含I2C驱动*/
#define	INCLUDE_I2C 

/*包含SPI驱动*/
#define INCLUDE_SPI

/*包含并行读写驱动*/
#define	INCLUDE_MCU_RW

/*包含AT24Cxx系列驱动*/
#define	INCLUDE_AT24CXX

/*包含FPGA下载模块*/
#define INCLUDE_FPGALOAD

/*包含OTP_TRACE模块*/
#define INCLUDE_TRACE

/*包含CLI模块*/
#define INCLUDE_CLI

/*菜单式console功能*/
/*#define INCLUDE_CONSOLE_MENU*/

/*系统指示灯*/
#define INCLUD_SVC_SYSTEM_LED

/*包含外部FLASH读写模块*/
#define INCLUDE_FLASH_COMM

/*包含X_MODEM模块 */
//#define INCLUDE_X_MODEM_COMM

/*包含FPGA模块*/
#define INCLUDE_FPGA

/*使用X_MODEM升级远端设备*/
/*#define INCLUDE_X_MODEM_REMOTE_IAP*/

/*包含FILE_LOADER模块 */
#define INCLUDE_FILE_LOADER_COMM

/*包含CODEC芯片模块*/
#define INCLUDE_CODEC

/*包含RA1015芯片模块*/
#define INCLUDE_RA1015

/*包含SE0111芯片模块*/
#define INCLUDE_SE0111

/*包含读取出厂信息模块*/
#define INCLUDE_PRODUCT_INFO



#define  INCLUDE_UART1


#if ( OTP_RUN_SIMPC ) /* 仿真环境 ROTP的模块剪裁 */

/*调试CODEC芯片*/
/*#define INCLUDE_CODEC_DEBUG*/
#else

/*包含看门狗模块*/
#define INCLUDE_WATCHDOG

#endif

/*包含产品信息通信协议*/
/*#define INCLUDE_PRODUCT_MSG_COMM_PRTCL*/

/*包含产品信息存储格式*/
//#define INCLUDE_PRODUCT_MSG_FORMAT 

/*定义产品信息下载协议放在哪个协议中处理，1：处理RC006协议串口，2：处理consel协议串口,3:处理RC002协议串口*/
//#define PRODUCT_MSG_COMM_PRTCL_TYPE       2

//#ifdef INCLUDE_PRODUCT_MSG_COMM_PRTCL

#if 0
#if	(PRODUCT_MSG_COMM_PRTCL_TYPE == 1)
#define INCLUDE_PRODUCT_MSG_COMM_PRTCL_RC006
#else
#if	(PRODUCT_MSG_COMM_PRTCL_TYPE == 2)
#define INCLUDE_PRODUCT_MSG_COMM_PRTCL_CONSEL
#else
#if	(PRODUCT_MSG_COMM_PRTCL_TYPE == 3)
#define INCLUDE_PRODUCT_MSG_COMM_PRTCL_RC002
#else
#error "PRODUCT_MSG_COMM_PRTCL_TYPE must be 1 , 2 or 3 !"
#endif
#endif
#endif
#endif
/*********************************************************************************/
/*part3:**************** 平台模块使用参数定制	**************************/
/*********************************************************************************/
/*	进行平台模块使用参数定制	，该处定义不可以删除；	  */
/*																				*/
/*																				*/
/*********************************************************************************/

/****************************************/
/*产品名称*/
#define DEVICE_NAME_ITN2100		"OPCOM3500E-Audio"
#define DEVICE_NAME_RP3000H		"RP3000H-Audio"
#define DEVICE_NAME_SLT8500		"SLT8500-Audio"

#define ITN2100_NAME			"OPCOM3500E"
#define RP3000H_NAME			"RP3000H"
#define SLT8500_NAME			"SLT"

/*端口个数定义*/
#define	FXSO_PORT_NUM				16
#define MT_PORT_NUM                 4
#define AUDIO_PORT_NUM              20
/* 识别MT模块在位使用的芯片和通道 */
#define MT_USE_CODEC_CHIP_NUM       4
#define MT_USE_CODEC_CHANNLE_NUM    1
/* MT模块配置外部增益使用的CODEC芯片 */
#define MT_GAIN_ADJ_CODEC_CHIP_NUM  5

/*产品软件版本*/
#define SOFTWARE_VERSION			0x11  /*wangzaizhong  201604027 增加派生DEVICE_NAME_SLT8500*/

/*设备类型定义*/
#define SVC_LOCAL_DEVTYPE			0x1A4A
#define RETAIL_AUDIO_DEVTYPE        0x1A4D
#define SLT8500_AUDIO_DEVTYPE        0x1AC7

/****************************************/
/*平台初始化之前，等待外围芯片启动时间，单位ms*/
#define SVC_START_DELAY				1000


/****************************************/
/*真正有业务的单盘数目*/
#define DEV_MAX_SERVICE_SLOT		13


/*如果包含了uart0，可以指定串口接收和发送缓冲区的长度*/
#ifdef INCLUDE_UART1	/*IBC通信*/
/*uart0 接收缓冲的长度*/
#define UART1_RCV_BUFLEN		1024
/*uart0 发送缓冲的长度*/
#define UART1_SEND_BUFLEN		1024
/*uart0 的配置参数*/
#define UART1_CFG_PARA			{19200, 8, 1, 0}/*速率为19200bps，数据位8比特，1位停止位，无校验*/
#endif

#ifdef INCLUDE_UART2
/*uart0 接收缓冲的长度*/
#define UART2_RCV_BUFLEN		300
/*uart0 发送缓冲的长度*/
#define UART2_SEND_BUFLEN		300
/*uart0 的配置参数*/
#define UART2_CFG_PARA			{19200, 8, 1, 0}/*速率为9600bps，数据位8比特，1位停止位，无校验*/
#endif

/*如果包含了uart3，可以指定串口接收和发送缓冲区的长度*/
#ifdef INCLUDE_UART3	/*调试口*/
/*uart3 接收缓冲的长度*/
#define UART3_RCV_BUFLEN		2000
#ifdef INCLUDE_CODEC_DEBUG
/*uart3 发送缓冲的长度*/
#define UART3_SEND_BUFLEN		8192
#else
/*uart3 发送缓冲的长度*/
#define UART3_SEND_BUFLEN		2000
#endif
/*uart3 的配置参数*/
#define UART3_CFG_PARA			{9600, 8, 1, 0}/*速率为9600bps，数据位8比特，1位停止位，无校验*/
#endif

/****************************************/
/*如果包含了定时器模块，定义允许的定时器数目*/
#ifdef INCLUDE_TMR
#define TMR_MAX_TMR_NUM			20
#endif


/****************************************/
/*如果包含了INCLUDE_IBC_RC006，
可以指定RC006协议IBC模块使用的缓冲用于各个业务模块数据打包*/
#ifdef INCLUDE_IBC_RC006
#define IBC_RC006_MSG_BUF_LEN	300
#endif


/****************************************/
/*如果包含了TRACE模块，定义用于trace的buf大小*/
#ifdef INCLUDE_TRACE
#define TRACE_BUF_LEN_MAX		128
#endif


/*********************************************************************************/
/*part4:******************* 驱动的IO管脚定制	******************************/
/*********************************************************************************/
/*	进行各种驱动的IO定义；											  			*/
/*	各管脚的具体定义将根据实际产品定义进行修改 ;			   					*/      
/*	对于产品没有应用到的模块管脚，可以删除；									*/
/*	在此处添加产品特有功能的IO管脚定义；										*/
/*********************************************************************************/


/*系统指示灯*/
#ifdef	INCLUD_SVC_SYSTEM_LED


#endif


/******************************************/
/*CPU 对网管盘的中断引脚的相关定义*/
#ifdef  INCLUDE_IBC_RC006
/*中断引脚*/

#endif


/*********************************************************************************/
/*part4:********************* 产品 模块剪裁	******************************/
/*********************************************************************************/
/*	基于产品的模块裁剪；	由用户自行控制裁剪。											  */
/*																				*/
/*																				*/
/*********************************************************************************/


#if ( OTP_RUN_SIMPC ) /* 仿真环境 产品的模块剪裁  */
/*包含SVC_dev模块*/
#define INCLUDE_SVC_DEV

/*包含SVC_SYSTEM模块*/
#define INCLUDE_SVC_SYSTEM

/*在RC006模块中使用文件下载*/
#define  INCLUDE_SVCDEV_RC006_FILE_DOWNLOAD
#define  INCLUDE_RESET
#else

/*包含SVC_dev模块*/
#define INCLUDE_SVC_DEV
/*包含SVC_SYSTEM模块*/
#define INCLUDE_SVC_SYSTEM

/*在RC006模块中使用文件下载*/
#define INCLUDE_SVCDEV_RC006_FILE_DOWNLOAD
#define INCLUDE_RESET
#endif

#endif


