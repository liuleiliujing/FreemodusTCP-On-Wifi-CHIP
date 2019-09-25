#ifndef _UART3_H
#define _UART3_H

/*发送缓冲区长度*/
#ifndef UART3_RCV_BUFLEN
#define UART3_RCV_BUFLEN		1024
#endif
/*接收缓冲区长度*/
#ifndef UART3_SEND_BUFLEN
#define UART3_SEND_BUFLEN		1024
#endif

/***************************************************************************
函数定义
***************************************************************************/
/******************************************************************************************
*uart3_init		--串口初始化函数
*--
*Input:
*	无
*Output:
*	无
*Return:
*	OK/ERROR	表示执行成功与否
*
*/
STATUS uart3_init(void);

/******************************************************************************************
*uart3_rs232_init		--串口初始化函数
*--
*Input:
*	prt	--协议类型
*	rcv --信息接收函数
*Output:
*	无
*Return:
*	OK/ERROR	表示执行成功与否
*
*/
STATUS uart3_rs232_init(uart_protocol_type prt, 
						uart_rcv_func rcv,
						uart_rcv_func opttypercv);



/*******************************************************************************
*uart3_rcv_process  接收报文处理，按照相应的协议，对接收缓冲区的报文进行处理，并调用
*		注册的rcv处理函数；
*Input:
*	None
*Output:
*	None	
*Return:
*	OK/ERROR，表示执行成功和失败
*/
STATUS uart3_rcv_process(void);


/*******************************************************************************
*uart3_msg_send 发送报文处理
*Input:
*	buf		--需要发送的数据指针
*	buflen	--发送的数据长度
*Output:
*	None	
*Return:
*	OK/ERROR，表示执行成功和失败
*/
STATUS uart3_msg_send(const OTP_UINT8 *buf, OTP_UINT32 buflen);


#endif


