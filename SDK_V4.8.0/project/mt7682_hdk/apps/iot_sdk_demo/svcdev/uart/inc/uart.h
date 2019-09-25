#ifndef _UART_H
#define _UART_H


/*端口号*/
enum
{
	UART1_ID = 0,
	UART2_ID = 1,
	UART3_ID = 2,	
	UARTIDMAX = 3,
};

#if  (UART1_REMAP_MODE == 0)
#define 	UART1_PORT 		PORTA	/*TXD1是端口PA*/
#define 	UART1_TXD_PIN 		9		/*TXD1是引脚PA9*/
#define 	UART1_RXD_PIN	 	10		/*RXD1是引脚PA10*/
#else
#define 	UART1_PORT 		PORTB	/*TXD1是端口PB*/
#define 	UART1_TXD_PIN 		6		/*TXD1是引脚PB6*/
#define 	UART1_RXD_PIN	 	7		/*RXD1是引脚PB7*/
#endif

#if  (UART2_REMAP_MODE == 0)
#define 	UART2_PORT 		PORTA	/*TXD2是端口PA*/
#define 	UART2_TXD_PIN 		2		/*TXD2是引脚PA2*/
#define 	UART2_RXD_PIN	 	3		/*RXD2是引脚PA3*/
#else
#define 	UART2_PORT 		PORTD	/*TXD2是端口PD*/
#define 	UART2_TXD_PIN 		5		/*TXD2是引脚PD5*/
#define 	UART2_RXD_PIN	 	6		/*RXD2是引脚PD6*/
#endif

#if  (UART3_REMAP_MODE == 0)
#define 	UART3_PORT 		PORTB	/*TXD3是端口PB*/
#define 	UART3_TXD_PIN 		10		/*TXD3是引脚PB10*/
#define 	UART3_RXD_PIN	 	11		/*RXD3是引脚PB11*/
#elif  (UART3_REMAP_MODE == 1)
#define 	UART3_PORT 		PORTC	/*TXD3是端口PC*/
#define 	UART3_TXD_PIN 		10		/*TXD3是引脚PC10*/
#define 	UART3_RXD_PIN	 	11		/*RXD3是引脚PC11*/
#elif  (UART3_REMAP_MODE == 3)
#define 	UART3_PORT 		PORTD	/*TXD3是端口PD*/
#define 	UART3_TXD_PIN 		8		/*TXD3是引脚PD8*/
#define 	UART3_RXD_PIN	 	9		/*RXD3是引脚PD9*/
#endif

/*波特率定义*/
enum
{
	UART_BAUD_300 		= 300,
	UART_BAUD_600 		= 600,
	UART_BAUD_1200 		= 1200,
	UART_BAUD_2400 		= 2400,
	UART_BAUD_4800 		= 4800,
	UART_BAUD_9600 		= 9600,
	UART_BAUD_14400 	= 14400,
	UART_BAUD_19200 	= 19200,
	UART_BAUD_38400		= 38400,
	UART_BAUD_57600		= 57600,
	UART_BAUD_115200	= 115200,
};

/*串口数据位定义*/
enum
{
	UART_DATAB_5BIT		= 5,
	UART_DATAB_6BIT		= 6,
	UART_DATAB_7BIT		= 7,
	UART_DATAB_8BIT		= 8,
	UART_DATAB_9BIT		= 9,	
};

/*串口停止位*/
enum
{
	UART_STOPB_1BIT = 1,
	UART_STOPB_2BIT = 2,
};

/*校验*/
enum
{
	UART_PARITY_NONE,
	UART_PARITY_ODD,
	UART_PARITY_EVEN,
	UART_PARITY_FONE,
	UART_PARITY_FZERO,
};

#define UART_VALID_UID(id) ((id)<UARTIDMAX)
#define UART_VALID_BARD(baud) (((baud) >= UART_BAUD_300)&&((baud)<= UART_BAUD_115200))
#define UART_VALID_DATAB(datab) (((datab) >= UART_DATAB_5BIT) && ((datab) <= UART_DATAB_9BIT))
#define UART_VALID_STOPB(stopb) (((stopb) >= UART_STOPB_1BIT) && ((stopb) <= UART_STOPB_2BIT))
#define UART_VALID_PARITY(parity) ((parity) <= UART_PARITY_FZERO)

/*交叉端口配置指针的合法性*/
#define UART_VALID_PCFG(pcfg) ((pcfg)&&UART_VALID_BARD(pcfg->baud) \
	&&UART_VALID_DATAB(pcfg->datab)&&UART_VALID_STOPB(pcfg->stopb)	\
	&&UART_VALID_PARITY(pcfg->parity))

/*数据寄存器*/
#define UXDR(ureg) 			((ureg)->DR)
#define UXDR_SET(ureg, val) 	((ureg)->DR = (val))

/*状态寄存器*/
#define UXSR(ureg) 			((ureg)->SR)

/*波特率寄存器*/
#define UXBRR(ureg) 			((ureg)->BRR)

/*控制寄存器1，设置数据位,校验位,发送接收使能,中断使能*/
#define UXCR1(ureg) 			((ureg)->CR1)

/*控制寄存器2，设置停止位,*/
#define UXCR2(ureg) 			((ureg)->CR2)

/*控制寄存器3，设置半双工模式使能,*/
#define UXCR3(ureg) 			((ureg)->CR3)

/*保护时间和预分频寄存器，设置Prescaler值*/
#define UXGTPR(ureg) 		((ureg)->GTPR)


#define UART_RCV_TEMPBUF_LEN	256



/*定义发送和接收数据时使用的协议类型*/
typedef enum _uart_protocol_type
{
	UART_PRTTYPE_002,
	UART_PRTTYPE_004,
	UART_PRTTYPE_006,
	UART_PRTTYPE_CONSOLE,
	UART_PRTTYPE_OPTTYPE,	
	UART_PRTTYPE_INVALID,
}uart_protocol_type;

/*注册的接收处理函数函数*/
/****************************************************************************
*arg:  函数自定义参数,这里定义为
*
*
*/
typedef STATUS (*uart_rcv_func)(OTP_VOID *arg, uart_protocol_type prt, OTP_UINT8 *pbuf, OTP_UINT16 len, OTP_UINT8 version);


#ifdef INCLUDE_OPTTYPE

/*需要通过ibc存储光头类型*/
typedef struct S_OPTTYPE_PAYLOAD_STRUCT
{
#ifdef OTP_LITTLE_ENDIAN

	OTP_UINT8	typeClass:7;	/*类型种类*/
	OTP_UINT8	reserved_1:1;	/*校验位,不处理*/

	OTP_UINT8	typeHigh:4;		/*类型种类*/
	OTP_UINT8	reserved_2:4;	/*校验位,不处理*/
	
	OTP_UINT8	typeLow:4;		/*类型种类*/
	OTP_UINT8	reserved_3:4;	/*校验位,不处理*/

	
#else 							/*wangzaizhong  20081201 修改*/
	OTP_UINT8	reserved_1:1;	/*校验位,不处理*/
	OTP_UINT8	typeClass:7;	/*类型种类*/

	OTP_UINT8	reserved_2:4;	/*校验位,不处理*/
	OTP_UINT8	typeHigh:4;		/*类型种类*/	
	
	OTP_UINT8	reserved_3:4;	/*校验位,不处理*/
	OTP_UINT8	typeLow:4;		/*类型种类*/
	
#endif

}OTP_PACK_ALIGN(1)S_OPTTYPE_PAYLOAD;



#define		UART_OPTTYPE_SLOT_POS			0
#define		UART_OPTTYPE_IDENTYFY_POS		1
#define		UART_OPTTYPE_LEN_POS			2
#define		UART_OPTTYPE_DATA_POS			5
#define		UART_OPTTYPE_EXTIDENTIFY_POS	8
#define		UART_OPTTYPE_EXTDENTIFY_LEN		15

#define		UART_OPTTYPE_LEN_EXCLUDE_BYTE	3
#define		UART_OPTTYPE_SLOT_MASK			0xf
#define		UART_OPTTYPE_LEN_MASK			0x3f

#define		UART_OPTTYPE_IDENTIFY			0xfe
#define		UART_OPTTYPE_LEN_DEFAULT		0x05

#define		UART_OPTTYPE_DATA_LEN			22	/*支持4个口*/
typedef struct _uart_opttype_buf
{
	OTP_UINT8				data[UART_OPTTYPE_DATA_LEN];
	OTP_UINT8				len;
	OTP_UINT8				ucnt;
	OTP_UINT32		 		rtime;	
	uart_rcv_func			rcv;	/*接收报文处理函数*/	
}OTP_PACK_ALIGN(1)uart_opttype_buf;

#endif

/*定义串口处理的一些标志*/
typedef struct _uart_buf_flag
{
	OTP_UINT8 rsvd:7;
	OTP_UINT8 bufof:1;/*暂存表溢出*/
}OTP_PACK_ALIGN(1)uart_buf_flag;


/*暂存缓冲区*/
typedef struct _uart_rcv_tempbuf
{
	OTP_UINT16	pinput; 	/*接收缓冲输入的指针值*/
	OTP_UINT16	poutput;	/*接收缓冲输出的指针值*/
	OTP_UINT8	buf[UART_RCV_TEMPBUF_LEN];
}OTP_PACK_ALIGN(1)uart_rcv_tempbuf;

/*接收处理缓冲区*/
typedef struct _uart_rcv_procbuf
{
	OTP_UINT16	outcnt;		/*处理的字节数*/
	OTP_UINT16	pktlen;		/*协议报文的长度*/
	OTP_UINT16	datalen;	/*缓冲区的长度*/
	OTP_UINT8	*pdata;		/*缓冲区指针*/
	uart_rcv_func	rcv;	/*接收报文处理函数*/
}OTP_PACK_ALIGN(1)uart_rcv_procbuf;


typedef struct _uart_rcv_info
{
	OTP_UINT32		rtime;	/*接收时间定义，记录上次字节接收时间*/
	uart_buf_flag   	flag;
	uart_rcv_tempbuf 	rtbuf;	/*暂存缓冲区信息*/
	uart_rcv_procbuf 	rproc;	/*报文处理缓冲区*/
	OTP_UINT16		errpkt;		/*接收错误报文计数*/
#ifdef INCLUDE_OPTTYPE
	uart_opttype_buf	opttype;
#endif	
}uart_rcv_info;

/*接收缓冲区*/


/*发送缓冲区*/
typedef struct _uart_send_info
{
	BOOL		sending;	/*表示是否开始发送缓冲区数据*/
	OTP_UINT16 	pinput;		/*接收缓冲输入的指针值;范围0~datalen-1*/
	OTP_UINT16 	poutput;	/*接收缓冲输入的指针值;范围0~datalen-1*/
	OTP_UINT16	datalen;	/*缓冲区的长度*/
	OTP_UINT8	*pdata;		/*缓冲区指针*/
	uart_buf_flag   flag;
}uart_send_info;


/*接口配置数据结构*/
typedef struct _uart_config_struct
{
	OTP_UINT32 baud; /*波特率*/
	OTP_UINT8 datab;	/*数据位，有效值:5,6,7,8*/
	OTP_UINT8 stopb;	/*停止位，1个或者2个,当此值为2，并且datab为5时表示1.5个停止位*/
	OTP_UINT8 parity;	/*0--无校验，1--奇校验，2--偶校验，3--强制为1，4--强制为0*/
}uart_config_struct;



/*初始化uart端口使用的数据结构*/
typedef struct _uart_init_param
{
	uart_config_struct  cfg;	/*接口配置*/
	uart_protocol_type	prt;/*接收和发送报文所用的协议类型*/
	uart_rcv_func		rcv; /*注册的接收处理函数*/
}uart_init_param;



typedef struct _uart_info
{
	uart_protocol_type prt;
	uart_rcv_info 	rinfo;
	uart_send_info	sinfo;
	OTP_VOID	*arg;		/*自定义指针*/
}uart_info;

/*rc002协议处理定义*/
#define UART_RC002DELIMITER_POS		0	/*定界符在报文中的位置*/
#define UART_RC002VERMARK_POS			1	/*协议版本标识的位置*/
#define UART_RC002PKT_LENORTYPE_POS	2	/*报文长度或设备类型字节*/
#define UART_RC002PKT_LEN_LOW_POS	3	/*报文长度低字节*/
#define UART_RC002PKT_RMC_LEN_LOW_POS	4	/*协转格式报文长度低字节*/
#define UART_RC002PKT_RMC_LEN_HIH_POS	5	/*协转格式报文长度高字节*/

/*rc006协议处理定义*/
#define UART_RC006_DELIMITER	0x95
#define	UART_RC006_HLENLEFT		10
#define UART_RC006_VER			0x01
#define UART_RC006_HEADLEN		12

#define UART_RC006DELIMITER_POS		0/*定界符在报文中的位置*/
#define UART_RC006DVERLEN_POS		1/*协议版本和协议头长度的位置*/
#define UART_RC006PKTLEN_HI_POS		2/*报文长度高字节*/
#define UART_RC006PKTLEN_LOW_POS	3/*报文长度低字节*/

#define UART_RC006PKTLEN_MIN	8/*报文最小长度*/

/*rc006报文版本和协议头剩余长度，小端定义*/
typedef struct _uart_rc006_verlen
{
#ifdef OTP_LITTLE_ENDIAN
	OTP_UINT8 hlenleft:5;/*协议头字节数，含framflag到协议头最后一个字节*/
	OTP_UINT8 ver:3;	/*协议版本号*/
#else
	OTP_UINT8 ver:3;	/*协议版本号*/
	OTP_UINT8 hlenleft:5;/*协议头字节数，含framflag到协议头最后一个字节*/
#endif
}OTP_PACK_ALIGN(1)uart_rc006_verlen;

#define UART_RCV_TIMEOUT	100	/*50ms*/



extern STATUS svc_uart_send_enable(OTP_UINT8 uid);
extern STATUS svc_uart_send_disable(OTP_UINT8 uid);




/*****************************************************************************
			函数定义
*****************************************************************************/


/******************************************************************************************
*uart_init		--串口初始化函数
*--
*Input:
*	uid		--串口号，合法值为0或者1
*	pcfg	--串口的初始化参数
*Output:
*	无
*Return:
*	OK/ERROR	表示执行成功与否
*
*/
STATUS uart_port_init(OTP_UINT8 uid,const  uart_config_struct *pcfg);


/****************************************************************************
* --uart_interrupt() UART0中断服务程序。
* Input:
*	uid --串口编号
*	pinfo --串口信息处理结构指针
*Output:
*	None
*Return:
*	None
*/
OTP_VOID uart_interrupt(OTP_UINT8 uid, uart_info *pinfo);


/*******************************************************************************
*uart_rcv_process  接收报文处理，按照相应的协议，对接收缓冲区的报文进行处理，并调用
*		注册的rcv处理函数；
*Input:
*	prt		--报文处理使用的协议，根据协议类型进行报文接收；
*	prinfo	--接收缓冲区相关信息指针；
*Output:
*	None	
*Return:
*	OK/ERROR，表示执行成功和失败
*/
STATUS uart_rcv_process(OTP_VOID *arg, uart_protocol_type prt, uart_rcv_info *prinfo);


/*******************************************************************************
*uart_msg_send 发送报文处理
*Input:
*	uid		--发送数据的URAT端口号
*	pinfo 	--串口信息处理结构指针
*	buf		--需要发送的数据指针
*	buflen	--发送的数据长度
*Output:
*	None	
*Return:
*	OK/ERROR，表示执行成功和失败
*/
STATUS uart_msg_send(OTP_UINT8 uid, uart_info *pinfo,const  OTP_UINT8 *buf, OTP_UINT32 buflen);




#endif


