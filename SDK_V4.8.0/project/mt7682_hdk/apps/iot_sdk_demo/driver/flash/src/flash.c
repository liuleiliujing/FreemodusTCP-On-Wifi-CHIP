/****************************************************************
	Copyright(C), 1999-2004 Raisecom, Inc.
	filename: flash.c
	Author	: wangzaizhong
	version	: 1.0
	date	: 2009/02/02
	Others	:
*****************************************************************/
/*
DESCRIPTION
		flash模块函数文件,实现FLASH的读写

SEE ALSO
*/


//#ifdef INCLUDE_FLASH_COMM

#include <string.h>
#include "type_common.h"
#include "spi.h"
#include "flash.h"

#define	DEBUG_MODE_FLASH   /*调试时使用*/

#define	OTP_TRACE_ERROR printf( "FUNC_ENTRY:   %s L#%d \n", __func__, __LINE__);

/*
#ifdef DEBUG_MODE_FLASH
#include "tmr.h"
#include "trace.h"
#endif
*/

falsh_m25p16_info		g_flash_global_info;/*flash使用的通用变量*/

/******************************************************************************
*tmr_delay_ms	--按照ms为单位延迟
*Input:
*	t	--需要延时的毫秒数
*Output:
*	None
*Return:
*	None
*/
OTP_VOID flash_tmr_delay_ms(OTP_UINT32 t)
{
	OTP_UINT32 i = 0;
	
	while(t--)
	{
		/* 对于11.0592M时钟，约延时1ms */
		for (i=0;i<7000;i++)
		{
			;	
		/*不同的单片机晶振搭配可能会对应着不同的循环次数
		对于LPC2103 + 11.0592MHz的搭配，可设定循环次数为7000*/
		/*不是很精确，可自行测试，自行调整*/
		}
	}
} 

/**********************************************************************
*flash_send_address	--flash模块输出地址
*NOTE:
*
*Input:
*	pparam --初始化参数
*Output:
*	无
*Return:
       读取数据
*/
LOCAL STATUS flash_send_address(const OTP_UINT32 flashadd)
{
	STATUS status=ERROR;

	status = g_flash_global_info.writefunc((OTP_UINT8)((flashadd & 0xFF0000)>>16));/*addr23~addr16 高位在前*/
	status = g_flash_global_info.writefunc((OTP_UINT8)((flashadd & 0x00FF00)>>8));
	status = g_flash_global_info.writefunc((OTP_UINT8)(flashadd & 0xFF));

	if (OK !=status)
	{
		return ERROR;
	}

	return OK;
}

/**********************************************************************
*flash_write_enable	--flash模块写使能
*NOTE:
*Input:
*	无
*Output:
*	无
*Return:
*	OK 		--正确
*	ERROR 	--错误
*/
LOCAL STATUS flash_write_enable(void)
{
	/* flash片选选中 */
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_ENABLE);
	/*输出控制字*/
	if (OK !=g_flash_global_info.writefunc((OTP_UINT8)FLASH_WRITE_ENABLE))
	{
		return ERROR;
	}
	/* flash片选禁止*/
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_DISABLE);	

	return OK;
}

/**********************************************************************
*flash_write_disable	--flash模块写禁止
*NOTE:
*Input:
*	无
*Output:
*	无
*Return:
*	OK 		--正确
*	ERROR 	--错误
*/
LOCAL STATUS flash_write_disable(void)
{
	/* flash片选选中 */
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_ENABLE);
	/*输出控制字*/
	if (OK !=g_flash_global_info.writefunc((OTP_UINT8)FLASH_WRITE_DISABLE))
	{
		return ERROR;
	}
	/* flash片选禁止*/
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_DISABLE);	

	return OK;
}

/**********************************************************************
*flash_read_status_register	--flash模块读状态寄存器
*NOTE:
*
*Input:
*	pdata    --读取的数据指针
*Output:
*	无
*Return:
*	OK 		--正确
*	ERROR 	--错误
*/
LOCAL STATUS flash_read_status_register(OTP_UINT8 *pdata)
{
	/* flash片选选中 */
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_ENABLE);
	/*输出控制字*/
	if (OK !=g_flash_global_info.writefunc((OTP_UINT8)FLASH_READ_STATUS))
	{
		return ERROR;
	}
	/* 从flash中读取一个字节 ，读取两次确保读正确*/
	*pdata =g_flash_global_info.readfunc();	
	*pdata =g_flash_global_info.readfunc();
	/* flash片选禁止*/
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_DISABLE);	

	return OK;
}

/**********************************************************************
*flash_busy_status	--flash模块读取数据是否为忙状态
*NOTE:
*
*Input:
*	无   
*Output:
*	MEMORY_BUSY =0,
*	MEMORY_OK =1
*Return:
*	OK 		--正确
*	ERROR 	--错误
*/
LOCAL e_memory_work_ret flash_busy_status(void)
{
	OTP_UINT8 readsta=0;	

	if (OK !=flash_read_status_register(&readsta))
	{
		return MEMORY_BUSY;
	}

	if (readsta &FLASH_BUSY_STATUS_VALUE)
	{
		return MEMORY_BUSY;
	}
	
	return MEMORY_OK;
}

/**********************************************************************
*flash_read_byte	--flash模块读指定地址的一个字节函数
*NOTE:
*
*Input:
*	flashadd --指定地址
*	pdata     --读取的数据指针
*Output:
*	无
*Return:
*	OK 		--正确
*	ERROR 	--错误
*/
STATUS flash_read_byte(const OTP_UINT32 flashadd,OTP_UINT8 *pdata)
{
	/* flash片选选中 */
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_ENABLE);
	/*输出控制字*/
	if (OK !=g_flash_global_info.writefunc((OTP_UINT8)FLASH_READ_BYTES))
	{
		return ERROR;
	}
	/*输出地址*/
	if (OK !=flash_send_address(flashadd))
	{
		return ERROR;
	}	
	/* 从flash中读取一个字节 */
	*pdata =g_flash_global_info.readfunc();	

	/* flash片选禁止*/
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_DISABLE);	

	return OK;
}

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
STATUS flash_read_page(const OTP_UINT32 flashadd,OTP_UINT8 *pdata, OTP_UINT32 len)
{
	OTP_UINT32 i=0;
	OTP_UINT32 pageadd=0;	

	/*入参检测*/
	if(pdata ==NULL)
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;		
	}

	if(len <FLASH_PAGE_SIZE)
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;		
	}
	
	/* flash片选选中 */
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_ENABLE);
	/*输出控制字*/
	if (OK !=g_flash_global_info.writefunc((OTP_UINT8)FLASH_READ_BYTES))
	{
		return ERROR;
	}
	/*页首地址获得*/
	pageadd =((flashadd/FLASH_PAGE_SIZE)*FLASH_PAGE_SIZE);
	/*输出地址*/
	if (OK !=flash_send_address(pageadd))
	{
		return ERROR;
	}	
	/* 从flash中读取一页数据*/
	for (i=0;i<FLASH_PAGE_SIZE;i++)
	{
		pdata[i] =g_flash_global_info.readfunc();	
	}

	/* flash片选禁止*/
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_DISABLE);	

	return OK;
}

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
*	ERROR 	--错误
*/
STATUS flash_write_page(const OTP_UINT32 flashadd,const OTP_UINT8 *pdata, const OTP_UINT32 len)
{
	OTP_UINT8 checkBuf[FLASH_PAGE_SIZE];
	OTP_UINT8 readsta=0;
	OTP_UINT32 i=0;
	OTP_UINT32 pageadd=0;	
	OTP_UINT8 cnt=0;	

	/*入参检测*/
	if(pdata ==NULL)
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;		
	}

	if(len <FLASH_PAGE_SIZE)
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;		
	}

	/*flash忙判断*/
	if (MEMORY_BUSY ==flash_busy_status())
	{
		/*写禁止*/
		if(OK !=flash_write_disable())
		{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
			return ERROR;		
		}
	}
	
/*************写使能******************************/	

	if(OK !=flash_write_enable())
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;		
	}	
	
/***************page write****************************/	
	/* flash片选选中 */
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_ENABLE);
	/*输出控制字*/
	if (OK !=g_flash_global_info.writefunc((OTP_UINT8)FLASH_PAGE_PROGRAM))
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;
	}
	/*页首地址获得*/
	/*pageadd =((flashadd/FLASH_PAGE_SIZE)*FLASH_PAGE_SIZE);*/
	/*输出地址*/
	//if (OK !=flash_send_address(pageadd))
	if (OK !=flash_send_address(flashadd))
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif	
		return ERROR;
	}	
	/* 向flash中写入一页数据*/
	for (i=0;i<FLASH_PAGE_SIZE;i++)
	{
/*		if (OK !=g_flash_global_info.writefunc(spi_m_l(pdata[i])))		*/	
		//if (OK !=g_flash_global_info.writefunc(pdata[i]))
			if (OK !=g_flash_global_info.writefunc(i))
		{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
			return ERROR;
		}
	}

	/* flash片选禁止*/
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_DISABLE);	

/************整页写操作完成判断*******************************/	

	/* flash片选选中 */
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_ENABLE);

	/*输出控制字，读状态*/
	if (OK !=g_flash_global_info.writefunc((OTP_UINT8)FLASH_READ_STATUS))
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;
	}
	
	/*完成判断*/
	readsta =g_flash_global_info.readfunc();	
	while(readsta & FLASH_BUSY_STATUS_VALUE)
	{/*write a page about 12ms */
		flash_tmr_delay_ms(1);
		readsta =g_flash_global_info.readfunc();	
		cnt ++;
		if(cnt >15)
		{
#ifdef	DEBUG_MODE_FLASH
			OTP_TRACE_ERROR;
#endif		
			break;
		}
	}

	/* flash片选禁止*/
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_DISABLE);	

/****************写禁止***************************/	

	if(OK !=flash_write_disable())
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;		
	}	

/******************check*************************/	

	if(OK != flash_read_page(flashadd,checkBuf,FLASH_PAGE_SIZE))
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;		
	}
	for (i=0;i<FLASH_PAGE_SIZE;i++)
	{
		if (checkBuf[i] != pdata[i])
		{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif		
			return ERROR;
		}
	}

	return OK;
}

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
STATUS flash_single_sector_erase(const OTP_UINT32 flashadd)
{
	OTP_UINT32 sectoradd=0;
	OTP_UINT8 readsta=0;	
	OTP_UINT8 cnt=0;	
#ifdef DEBUG_MODE_FLASH
	//OTP_UINT32 lasttime,nowtime;
	//lasttime= tmr_get_time(); 
#endif

	/*入参检查*/

	/*flash忙判断*/
	if (MEMORY_BUSY ==flash_busy_status())
	{
		/*写禁止*/
		if(OK !=flash_write_disable())
		{
#ifdef	DEBUG_MODE_FLASH
			OTP_TRACE_ERROR;
#endif
			return ERROR;		
		}	
	}

/*******************************************/	

	/*写使能*/
	if(OK !=flash_write_enable())
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;		
	}	

/*******************************************/	

	/* flash片选选中 */
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_ENABLE);
	/*输出清扇区控制字*/
	if (OK !=g_flash_global_info.writefunc((OTP_UINT8)FLASH_ERASE_SECTOR))
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif	
		return ERROR;
	}

	/*获得扇区首地址*/
	sectoradd  =(flashadd/FLASH_SECTOR_SIZE)*FLASH_SECTOR_SIZE;
	
	/*输出地址*/
	if (OK !=flash_send_address(sectoradd))
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;
	}	
	/* flash片选禁止*/
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_DISABLE);	

/*******************************************/	

	/* flash片选选中 */
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_ENABLE);

	/*输出控制字，读状态*/
	if (OK !=g_flash_global_info.writefunc((OTP_UINT8)FLASH_READ_STATUS))
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;
	}
	
	/*完成判断*/
	readsta =g_flash_global_info.readfunc();	
	while(readsta & FLASH_BUSY_STATUS_VALUE)
	{/*erase a sector about 800ms */
		flash_tmr_delay_ms(5);
		readsta =g_flash_global_info.readfunc();	
		cnt ++;
		if(cnt >200)
		{
#ifdef	DEBUG_MODE_FLASH
			OTP_TRACE_ERROR;
#endif
			break;
		}
	}

	/* flash片选禁止*/
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_DISABLE);	

/*******************************************/	

	/*写禁止*/
	if(OK !=flash_write_disable())
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;		
	}	

#ifdef DEBUG_MODE_FLASH
	//nowtime= tmr_get_time(); 
	//OTP_TRACE("erase flash ,  delta = %u\r\n", nowtime-lasttime);
#endif

	return OK;
}

/**********************************************************************
*flash_sector_erase	--flash模块扇区清除函数
*NOTE:清指定扇区
*
*Input:
*	flashadd --首扇区地址
*	flashadd --末扇区地址
*Output:
*	无
*Return:
*	OK 		--正确
*	BUSY	--忙，
*	ERROR 	--错误
*/
STATUS flash_sector_erase(const OTP_UINT32 staflashadd,const OTP_UINT32 endflashadd)
{
	OTP_UINT32 flashaddr=0;

	/*入参检查*/
	if (staflashadd >endflashadd)
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;
	}

	/*获得扇区首地址*/
	flashaddr  =(staflashadd/FLASH_SECTOR_SIZE)*FLASH_SECTOR_SIZE;	

	while(flashaddr <=endflashadd)
	{
		if (OK !=flash_single_sector_erase(flashaddr))
		{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
			return ERROR;
		}
		
		flashaddr +=FLASH_SECTOR_SIZE;
	}
	
	return OK;	
}

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
STATUS flash_bulk_erase(void)
{
	OTP_UINT8 readsta=0;	
	OTP_UINT16 cnt=0;	

	/*flash忙判断*/
	if (MEMORY_BUSY ==flash_busy_status())
	{
		/*写禁止*/
		if(OK !=flash_write_disable())
		{
#ifdef	DEBUG_MODE_FLASH
			OTP_TRACE_ERROR;
#endif
			return ERROR;		
		}	
	}

/*******************************************/	
	/*写使能*/
	if(OK !=flash_write_enable())
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;		
	}
	
/*******************************************/	

	/* flash片选选中 */
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_ENABLE);

	/*输出清全部扇区控制字*/
	if (OK !=g_flash_global_info.writefunc(FLASH_ERASE_BULK))
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;
	}
	
	/* flash片选禁止*/
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_DISABLE);	

/*******************************************/	

	/* flash片选选中 */
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_ENABLE);

	/*输出控制字，读状态*/
	if (OK !=g_flash_global_info.writefunc((OTP_UINT8)FLASH_READ_STATUS))
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;
	}
	
	/*完成判断*/
	readsta =g_flash_global_info.readfunc();	
	while(readsta & FLASH_BUSY_STATUS_VALUE)
	{
		readsta =g_flash_global_info.readfunc();	
		cnt ++;
		if(cnt >1000)
		{
#ifdef	DEBUG_MODE_FLASH
			OTP_TRACE_ERROR;
#endif		
			break;
		}
	}

	/* flash片选禁止*/
	(OTP_VOID)g_flash_global_info.enablefunc(FLASH_CS_DISABLE);	

/*******************************************/	

#if 0
	/*写禁止*/
	if(OK !=flash_write_disable())
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;		
	}	
#endif

	return OK;	
}

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
STATUS flash_sector_copy(OTP_UINT8 dstid, OTP_UINT8 srcid)
{
	OTP_UINT32 i=0;
	OTP_UINT32 busynum=0;	
	OTP_UINT32 pagenum=0;
	OTP_UINT32 destinationpageaddr=0;
	OTP_UINT32 sourcepageaddr=0;	
	OTP_UINT8	pagedata[FLASH_PAGE_SIZE];
	STATUS stetus=ERROR;
	
	/*入参检查*/
	if ((dstid >=FLASH_SECTOR_NUMBER_MAX) || (srcid >=FLASH_SECTOR_NUMBER_MAX))
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;	
	}

	memset(pagedata,0,sizeof(pagedata));

	/*获得页个数*/
	pagenum =(FLASH_SECTOR_SIZE/FLASH_PAGE_SIZE);
	/*获得目的地址*/
	destinationpageaddr =dstid*FLASH_SECTOR_SIZE;
	/*获得源地址*/	
	sourcepageaddr =srcid*FLASH_SECTOR_SIZE;	

	/*按页进行数据拷贝*/
	for (i=0;i<pagenum;i++)
	{
		/*读源页数据*/
		if (OK !=flash_read_page(sourcepageaddr+(FLASH_PAGE_SIZE*i),pagedata,FLASH_PAGE_SIZE))
		{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
			return ERROR;	
		}

/********在进行扇区数据拷贝时，对数据的bit序进行了调整，**************/
/*********主要为RC958的在线升级中，FPGA的启动文件和实际生****************/
/*********成的.bin文件的bit序是颠倒的，所以在这里进行了处理**************/
/*********，其它项目时请注意，在不需要调整bit序时不使用该部分*******/
#if	0		
		/*数据格式的变化*/
		for (j=0;j<FLASH_PAGE_SIZE;j++)
		{
			pagedata[j] =spi_m_l(pagedata[j]);
		}
#endif
		/*写目的页*/
		for (busynum=0;busynum<FLASH_BUSY_RENEW_WRITE_NUMBER;busynum++)	
		{
			stetus =flash_write_page(destinationpageaddr+(FLASH_PAGE_SIZE*i),pagedata,FLASH_PAGE_SIZE);
			if (OK ==stetus)
			{
				break;
			}
			else
			{
				/*适当延时*/
				flash_tmr_delay_ms(2);
			}
		}

		if (busynum >=FLASH_BUSY_RENEW_WRITE_NUMBER)
		{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
			return ERROR;
		}
	}

	return OK;	
}

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
STATUS flash_page_write_zero(const OTP_UINT32 pageadd)
{
	OTP_UINT8 tempdata[FLASH_PAGE_SIZE];

	memset(tempdata,0,sizeof(tempdata));

	if (OK !=flash_write_page(pageadd,tempdata, (OTP_UINT32)FLASH_PAGE_SIZE))
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;	

	}
	return OK;	
}

/**********************************************************************
*flash_init_mx52l	--flash模块初始化函数
*NOTE:根据输入参数对flash驱动进行初始化
*初始化flash驱动使用的缓存和变量，
*注册底层读FALSH 一个字节函数；
*注册底层写FALSH 一个字节函数；
*注册底层FLASH芯片使能控制函数
*
*Input:
*	pparam --初始化参数
*Output:
*	无
*Return:
*	OK 		--正确
*	ERROR 	--错误
*/
STATUS flash_init_mx52l(const falsh_m25p16_create_param *pparam)
{	
	if(pparam == NULL)
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;
	}

/******只限RC958使用，其它项目不应写入驱动中****************************/ 
	/*flash 芯片使能控制，在flash初始化时使用*/
#if 0
	FLASH_OE_OUT;
	FLASH_OE_EN;
#endif
/***********************************************************************/
	memset(g_flash_global_info.databuf,0,sizeof(g_flash_global_info.databuf));

	g_flash_global_info.readfunc = pparam->readfunc;		/*底层读FALSH 一个字节函数*/
	g_flash_global_info.writefunc = pparam->writefunc;		/*底层写FALSH 一个字节函数*/
	g_flash_global_info.enablefunc = pparam->enablefunc;	/*底层FLASH芯片使能控制函数*/

	/*写禁止*/
	if(OK !=flash_write_disable())
	{
#ifdef	DEBUG_MODE_FLASH
		OTP_TRACE_ERROR;
#endif
		return ERROR;		
	}	

	printf("\r\n ---flash_init_mx52l  ok!---\r\n");

	return OK;
}

//#endif

