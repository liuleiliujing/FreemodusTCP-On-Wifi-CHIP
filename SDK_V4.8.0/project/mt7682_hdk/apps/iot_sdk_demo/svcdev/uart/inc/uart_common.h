#ifndef _UART_COMMON_H
#define _UART_COMMON_H


/********************************************************************************************************************/
/********************************************************************************************************************/

#define UART_RCV_TEMPBUF_LEN	256


/*端口号*/
enum
{
	UART1_ID = 0,
	UART2_ID = 1,
	UART3_ID = 2,	
	UARTIDMAX = 3,
};


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
typedef STATUS (*uart_rcv_func)(OTP_VOID *arg, uart_protocol_type prt, OTP_UINT8 *pbuf, OTP_UINT16 datalen, OTP_UINT8 funcversion);


/*定义串口处理的一些标志*/
typedef struct _uart_buf_flag
{
	OTP_UINT8 rsvd:7;
	OTP_UINT8 bufof:1;/*暂存表溢出*/
}uart_buf_flag;


/*暂存缓冲区*/
typedef struct _uart_rcv_tempbuf
{
	OTP_UINT16	pinput; 	/*接收缓冲输入的指针值*/
	OTP_UINT16	poutput;	/*接收缓冲输出的指针值*/
	OTP_UINT8	buf[UART_RCV_TEMPBUF_LEN];
}uart_rcv_tempbuf;

/*接收处理缓冲区*/
typedef struct _uart_rcv_procbuf
{
	OTP_UINT16	outcnt;		/*处理的字节数*/
	OTP_UINT16	pktlen;		/*协议报文的长度*/
	OTP_UINT16	datalen;	/*缓冲区的长度*/
	OTP_UINT8	*pdata;		/*缓冲区指针*/
	uart_rcv_func	rcv;	/*接收报文处理函数*/
}uart_rcv_procbuf;


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



typedef struct _uart_info
{
	uart_protocol_type prt;
	uart_rcv_info 	rinfo;
	uart_send_info	sinfo;
	OTP_VOID	*arg;		/*自定义指针*/
}uart_info;



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
}uart_rc006_verlen;

#define UART_RCV_TIMEOUT	100	/*50ms*/



/*****************************************************************************
			函数定义
*****************************************************************************/

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

/********************************************************************************************************************/
/********************************************************************************************************************/


#endif

