#ifndef _IBC_H
#define _IBC_H

typedef STATUS (*xxx_msg_send)(OTP_UINT8 *pbur, OTP_UINT32 sendlen);

/*从黄玉清的对端通信模块中移来*/
/*lijingfang 2009/02/03*/
/*定义通信对象*/
typedef	enum	_comm_object
{
	comm_to_uart1,	
	comm_to_uart2,	
	comm_to_uart3
}comm_object;

/*定义通信来源的相关信息*/
typedef	struct _comm_source_info
{
	xxx_msg_send	send;	/*消息响应的发送函数*/
	comm_object		comm_obj;	/*通信对象*/
}comm_source_info;


/*****************************************************************************
*ibc_init_all		ibc初始化函数
*Input:
*     protocol  -- ibc使用的协议类型
*	uartid	--ibc使用的串口编号，0，1；
**Output:
*	None
*Return:
*	OK			--表示执行成功
*	ERROR		--表示执行错误
*/
STATUS ibc_init_all(uart_protocol_type protocol, OTP_UINT8 uartid);


#endif

