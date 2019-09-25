
#ifndef __INCLUDE_IBC_OPTTYPE_H
#define	__INCLUDE_IBC_OPTTYPE_H


typedef struct S_IBC_OPTTYPE_PAYLOAD_STRUCT
{
#ifdef OTP_LITTLE_ENDIAN
	OTP_UINT8	typeClass:7;
	OTP_UINT8	reserved_1:1;

	OTP_UINT8	typeHigh:4;		/*类型种类*/
	OTP_UINT8	reserved_2:4;	/*校验位,不处理*/
	OTP_UINT8	typeLow:4;		/*类型种类*/
	OTP_UINT8	reserved_3:4;	/*校验位,不处理*/

	
#else
	OTP_UINT8	reserved_1:1;
	OTP_UINT8	typeClass:7;
	
	OTP_UINT8	reserved_2:4;	/*校验位,不处理*/
	OTP_UINT8	typeHigh:4;		/*类型种类*/
	
	OTP_UINT8	reserved_3:4;	/*校验位,不处理*/
	OTP_UINT8	typeLow:4;		/*类型种类*/
#endif

}OTP_PACK_ALIGN(1)S_IBC_OPTTYPE_PAYLOAD;

typedef struct S_IBC_OPTTYPE_ACK_STRUCT
{
	OTP_UINT8	slot;
	OTP_UINT8	checkSum;
	OTP_UINT8	identify;
	OTP_UINT8	data;	
}OTP_PACK_ALIGN(1)S_IBC_OPTTYPE_ACK;



typedef STATUS (*ibc_opttype_rcv_func)(OTP_UINT8 *pPayload);
/*******************************************************************************
*ibc_opttype_init  存储光头类型模块,初始化函数
*Input:
*Output:
*	None	
*Return:
*	None
*/
OTP_VOID ibc_opttype_init(void);
/*******************************************************************************
*ibc_opttype_install  存储光头类型模块,安装回调函数函数
*Input:
*Output:
*	None	
*Return:
*	None
*/
OTP_VOID ibc_opttype_install(ibc_opttype_rcv_func callback);

/*******************************************************************************
*ibc_opttype_rcv  存储光头类型模块,接收处理函数函数
*Input:
*Output:
*	None	
*Return:
*	None
*/
STATUS ibc_opttype_rcv(OTP_VOID *arg, uart_protocol_type prt, OTP_UINT8 *pbuf, OTP_UINT16 len, OTP_UINT8 ver);

#endif

