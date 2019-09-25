#ifndef _SVC_FLASH_H
#define _SVC_FLASH_H

#if 0
#define IOPIN_X(X)          (*((volatile unsigned long *) (0xE0028000+(0x10*(X)))))      /* no in lpc210x*/
#define IOSET_X(X)          (*((volatile unsigned long *) (0xE0028004+(0x10*(X)))))     /* no in lpc210x*/
#define IODIR_X(X)          (*((volatile unsigned long *) (0xE0028008+(0x10*(X)))))      /* no in lpc210x*/
#define IOCLR_X(X)          (*((volatile unsigned long *) (0xE002800C+(0x10*(X)))))      /* no in lpc210x*/
#define FLASH_OE_PIN			BIT(20)
#define FLASH_OE_OUT 			IODIR_X(1) |= FLASH_OE_PIN
#define FLASH_OE_DIS			IOSET_X(1)  = FLASH_OE_PIN
#define FLASH_OE_EN			IOCLR_X(1)  = FLASH_OE_PIN
#endif

/*  AS Instruction Set    */
#define FLASH_WRITE_ENABLE		0x06
#define FLASH_WRITE_DISABLE		0x04
#define FLASH_READ_STATUS	    0x05
#define FLASH_WRITE_STATUS	    0x01
#define FLASH_READ_BYTES   		0x03
#define FLASH_FAST_READ_BYTES  	0x0B
#define FLASH_PAGE_PROGRAM		0x02
#define FLASH_ERASE_SECTOR		0xD8
#define FLASH_ERASE_BULK		0xC7
#define FLASH_READ_SILICON_ID	0xAB
#define FLASH_CHECK_SILICON_ID	0x9F

#define FLASH_READ_STATUS_BIT0	0


#define FLASH_PAGE_SIZE			256		/*FLASH的页宽度*/
#define FLASH_SECTOR_SIZE		0x10000	/*FLASH的扇区宽度*/	
#define FLASH_SECTOR_NUMBER_MAX	128		/*扇区总数*/


#define FLASH_BUSY_RENEW_WRITE_NUMBER	10		/*重新写次数*/

#define FLASH_BUSY_STATUS_VALUE	    	0x01	/*FLASH忙状态数值*/

/*FLASH片选控制*/
typedef enum _e_flash_cs_ena
{
	FLASH_CS_DISABLE	= 0,
	FLASH_CS_ENABLE	= 1
	
}e_flash_cs_ena;


typedef enum _e_flash_write_ret
{
	FLASH_WRITE_OK,
	FLASH_WRITE_BUSY,
	FLASH_WRITE_ERROR
}e_flash_write_ret;

typedef enum _e_memory_work_ret
{
	MEMORY_BUSY,
	MEMORY_OK
}e_memory_work_ret;



/*注册在flash模块中读一个字节函数。
	输入参数:

	输出参数:
		
	返回值:
			接收数据
*/
typedef OTP_UINT8 (*flash_read_byte_func)(void);


/*注册在flash模块中写一个字节函数。
	输入参数:
		pdata		--输出数据的指针

	输出参数:
		
	返回值:
		OK,	ERROR	处理该事件成功或失败
*/
typedef STATUS (*flash_write_byte_func)(OTP_UINT8 pdata);


/*注册在flash模块中读写使能函数。
	输入参数:
		cs_ena	--FLASH片选使能
	输出参数:
		
	返回值:
		OK,	ERROR	处理该事件成功或失败。
*/
typedef STATUS (*flash_enable_func)(e_flash_cs_ena	cs_ena);



/*flash模块总的数据结构定义*/
typedef struct _falsh_m25p16_info
{

	flash_read_byte_func			readfunc;		/*注册的底层读FALSH 一个字节函数*/
	flash_write_byte_func			writefunc;		/*注册的底层写FALSH 一个字节函数*/
	flash_enable_func				enablefunc;		/*注册的底层FLASH芯片使能控制函数*/

	OTP_UINT8 					databuf[FLASH_PAGE_SIZE];/*FLASH_PAGE_SIZE宽的数据缓存*/

}falsh_m25p16_info;

/*初始化信息结构定义*/
typedef struct _falsh_m25p16_create_param
{
	flash_read_byte_func			readfunc;		/*注册的底层读FALSH 一个字节函数*/
	flash_write_byte_func			writefunc;		/*注册的底层写FALSH 一个字节函数*/
	flash_enable_func				enablefunc;		/*注册的底层FLASH芯片使能控制函数*/

}falsh_m25p16_create_param;



/******************************************************************************
					函数定义
******************************************************************************/

/**********************************************************************
*flash_init_mx52l	--flash模块初始化函数
*NOTE:根据输入参数对cmf模块进行初始化
*
*Input:
*	pparam --初始化参数
*Output:
*	无
*Return:
*	OK 		--正确
*	ERROR 	--错误
*/
STATUS flash_init_mx52l(const falsh_m25p16_create_param *pparam);

/**********************************************************************
*flash_read_byte	--flash模块读指定地址的一个字节函数
*NOTE:
*
*Input:
*	pparam --初始化参数
*	pdata    --读取的数据指针
*Output:
*	无
*Return:
*	OK 		--正确
*	ERROR 	--错误
*/
STATUS flash_read_byte(const OTP_UINT32 flashadd,OTP_UINT8 *pdata);

/**********************************************************************
*flash_read_page	--flash模块整页读函数(256字节)
*NOTE:整页读
*
*Input:
*	flashadd --页的起始地址；
*	pdata     --读出数据的缓存区地址；
*	flashadd --读出数据的缓存区长度；
*Output:
*	无
*Return:
*	OK 		--正确
*	ERROR 	--错误
*/
STATUS flash_read_page(const OTP_UINT32 flashadd,OTP_UINT8 *pdata, OTP_UINT32 len);

/**********************************************************************
*flash_write_page	--flash模块整页写函数(256字节)
*NOTE:整页写
*
*Input:
*	flashadd --页的起始地址；
*	pdata     --需要写数据的缓存区地址；
*	flashadd --需要写数据的缓存区有效数据长度；
*Output:
*	无
*Return:
*	OK 		--正确
*	BUSY	--忙，
*	ERROR 	--错误
*/
STATUS flash_write_page(const OTP_UINT32 flashadd,const OTP_UINT8 *pdata, OTP_UINT32 len);

/**********************************************************************
*flash_single_sector_erase	--flash模块单个扇区清除函数
*NOTE:清指定扇区
*
*Input:
*	flashadd --扇区首地址
*Output:
*	无
*Return:
*	OK 		--正确
*	BUSY	--忙，
*	ERROR 	--错误
*/
STATUS flash_single_sector_erase(const OTP_UINT32 flashadd);

/**********************************************************************
*flash_sector_erase	--flash模块扇区清除函数
*NOTE:清指定扇区
*
*Input:
*	flashadd --扇区首地址
*	flashadd --扇区末地址
*Output:
*	无
*Return:
*	OK 		--正确
*	BUSY	--忙，
*	ERROR 	--错误
*/
STATUS flash_sector_erase(const OTP_UINT32 staflashadd,const OTP_UINT32 endflashadd);



/**********************************************************************
*flash_bulk_erase	--flash模块全部清函数
*NOTE:根据输入参数对cmf模块进行初始化
*
*Input:
*	pparam --初始化参数
*Output:
*	无
*Return:
*	OK 		--正确
*	ERROR 	--错误
*/
STATUS flash_bulk_erase(void);

/**********************************************************************
*flash_sector_copy	--flash模块扇区拷贝函数
*NOTE:
*
*Input:
*	dstid		--目标扇区号，
*	srcid		--源扇区号，
*Output:
*	无
*Return:
*	OK 		--正确
*	ERROR 	--错误
*/
STATUS flash_sector_copy(OTP_UINT8 dstid, OTP_UINT8 srcid);

/**********************************************************************
*flash_sector_copy	--flash模块扇区拷贝函数
*NOTE:
*
*Input:
*	dstid		--目标扇区号，
*	srcid		--源扇区号，
*Output:
*	无
*Return:
*	OK 		--正确
*	ERROR 	--错误
*/
STATUS flash_page_write_zero(const OTP_UINT32 pageadd);

#endif
