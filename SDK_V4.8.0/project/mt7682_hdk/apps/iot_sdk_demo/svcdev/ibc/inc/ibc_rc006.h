
#ifndef _IBC_RC006_H
#define _IBC_RC006_H

/*RC006协议IBC模块使用的缓冲用于各个业务模块数据打包*/
#ifndef IBC_RC006_MSG_BUF_LEN
#define IBC_RC006_MSG_BUF_LEN		1024
#endif

/*rc006协议头和扩展协议头的最大长度*/
#define IBC_RC006_MSG_HEAD_LEN_MAX	30

/*定义无效的同步号*/
#define IBC_INVALID_SYNCID 16

typedef struct ibc_msg_rc006
{
	struct
	{
		OTP_UINT8 framflag;	/*起始字符*/
#ifdef OTP_LITTLE_ENDIAN
		OTP_UINT8 hlenleft:5;/*协议头字节数，含framflag到协议头最后一个字节*/
		OTP_UINT8 ver:3;	/*协议版本号*/
#else
		OTP_UINT8 ver:3;	/*协议版本号*/
		OTP_UINT8 hlenleft:5;/*协议头字节数，含framflag到协议头最后一个字节*/
#endif

		OTP_UINT16 dlenleft; /*数据包后继长度字节数，从下一个字节开始到校验和*/
		
		OTP_UINT8 destslot;	/*目的槽位号*/
		OTP_UINT8 destmod;	/*目的模块号*/
		OTP_UINT8 srcslot;	/*源槽位号*/
		OTP_UINT8 srcmod;	/*源模块号*/
		
		union{
				OTP_UINT16	id;	/*序列号数值*/
				
				/*为序列号做的具体解释*/
				struct{
					OTP_UINT8 win;	/*窗口序列号*/
					OTP_UINT8 sync;	/*同步消息号*/
				}OTP_PACK_ALIGN(1)p;
		}OTP_PACK_ALIGN(1)s;
		/*响应序列号*/
		union{
				OTP_UINT16	id;	/*序列号数值*/
				
				/*为序列号做的具体解释*/
				struct{
					OTP_UINT8 win;	/*窗口序列号*/
					OTP_UINT8 sync;	/*同步消息号*/
				}OTP_PACK_ALIGN(1)p;
		}OTP_PACK_ALIGN(1)sack;

	}OTP_PACK_ALIGN(1)head;	
	
	/*数据区，也是净荷*/
	OTP_UINT8 data[];		/* !! 必须是最后一个字段,且紧接head*/
	
}OTP_PACK_ALIGN(1)ibc_rc006_msg;

#define swinseq 		s.p.win
#define swinseqack 		sack.p.win
#define ssyncid 		s.p.sync
#define ssyncidack 		sack.p.sync

/*ibc_msg结构中data字段相对于结构地址的偏移量*/
#define IBC_DATA_OFFSET	((OTP_UINT32)(&((ibc_rc006_msg*)0)->data))

/*ibc_msg结构中head字段相对于结构地址的偏移量*/
#define IBC_HEAD_OFFSET	((OTP_UINT32)(&((ibc_rc006_msg*)0)->head))

/*
	V1板板间通信协议头固定字段长度
*/
#define IBC_HEAD_LEN_V1_FIX		/*lint -save -e545 -e413 -e737*/( IBC_DATA_OFFSET - IBC_HEAD_OFFSET )/*lint -restore*/


/*

在IBC层检查报文的错误代码
	0x00	正常响应
	0x01	不支持的协议版本
	0x02	长度和校验和错
*/
#define IBC_PACKET_OK				0x00
#define IBC_PACKET_ERROR_VERSION	0x01
#define IBC_PACKET_ERROR_LEN_CRC	0x02


/*板间通信起始字符 10010101*/
#define IBC_FRAME_FLAG		0X95

/*板间通信版本号 001*/
#define IBC_VERSION		0x01

/*协议头全长与协议头中hlenleft字段值的转化*/
#define IBC_HLENLEFT2HEADLEN(hlenleft) ((hlenleft)+2)
#define /*lint -save -e545 -e413*/IBC_HEADLEN2HLENLEFT(headlen) ((headlen)-2)/*lint -restore*/

/*报文全长与协议头中dlenleft字段值的转化*/
#define IBC_DLENLEFT2DATALEN(dlenleft) ((dlenleft)+4)
#define IBC_DATALEN2DLENLEFT(datalen) ((datalen)-4)

typedef struct _ibc_last_msg_info
{
	OTP_UINT8 lsseq;	
	OTP_UINT8 lsscid;
	OTP_UINT8 lrseq;
	OTP_UINT8 lrscid;
}ibc_last_msg_info;


typedef OTP_UINT8* IBC_DATA;
typedef OTP_UINT8* IBC_MSG;



/*申请板间报文块的时候返回这个类型，实际是IBC消息头指针*/
typedef OTP_UINT8* IBC_PACKET;

/*注册在IBC的模块号*/
#define IBC_INVALID_MOD		0	/*无效模块号*/
#define IBC_MOD_ADAPTER		1	/*适配层*/

/* 板间消息CRC字段的字节数 */
#define IBC_CRC_LEN_V1	1


typedef struct _ibc_adp_info
{
	/*自身是否正在做环回，在业务盘用*/
	BOOL	isloopback;
}ibc_adp_info;


/*
	业务盘上的业务模块处理从网管盘或局端业务盘发来的数据的函数。
	输入参数:
		arg			自定义参数
		msg,		原始消息包。如果需要直接处理消息包可以用它。
					当返回值为OK时必须释放，否则不能释放。
		slot,	 	接收到的消息的源槽位号
		prid,		接收到消息的远端设备标识数组
		opcode,		操作码类型
		data,		具体消息数据(协议的净荷部分)，不能释放。
		datalen,	data中有效数据的长度
	返回值:
		OK,			本函数消化了msg消息，在返回前已经释放了msg,IBC适配层模块不能再释放该消息
		ERROR,		本函数没有消化msg消息，在返回后由IBC适配层模块释放msg消息
*/
typedef STATUS (*xxx_proc_nms_msg)(OTP_VOID *arg, IBC_MSG ibcmsg,const rdev_rid* prid,OTP_UINT8 opcode,
						const OTP_UINT8 *data,OTP_UINT32 datalen, OTP_UINT8 *pack, OTP_UINT32 acklen);



/**********************************************************************************
函数定义
*/

/**********************************************************************************
*	从业务盘向网管盘发送同步消息的响应消息。隐含的目的槽位都是0
*	输入参数:
*		arg			无类型的指针，用于传递发送需要使用的函数指针，用户不需要理解
*		srcmsg,		收到的原始消息，用来向响应消息中填响应序列号和窗口号
*		prid,		扩展协议头中的远端设备标识,
*					如果为NULL，就是跳数为0，
*					否则按照指定跳数来封装扩展协议头
*		exrsp,		在响应消息中就是扩展响应字
*		data,		净荷块，函数调用者申请、填写和释放，
*		datalen,	data中有效数据的长度
*		
*		srcslot,	只在仿真环境中有用，表示发送的源槽位号
*		
*	返回值:
*		OK,			消息成功进入发送队列
*		ERROR,		消息没有进入发送队列
*/
STATUS ibcadp_send(const OTP_VOID *arg,IBC_MSG srcmsg,const rdev_rid* prid,OTP_UINT8 exrsp,const OTP_UINT8 *data,OTP_UINT32 datalen);

/*********************************************************************************
*	从网管盘上向业务模块同步发送板间消息。
*	输入参数:
*		arg	,自定义参数，传递发送报文使用的函数
*		destslot,	目的槽位号
*		prid,		远端设备标识指针，为NULL或跳数为0的时候表示发送给局端单盘，函数调用者申请、填写和释放。
*		opcode,		操作码类型
*		data,		具体消息数据(协议中的净荷)，函数调用者申请、填写和释放。
*					如果没有数据要发送，填NULL。
*		datalen,	data中有效数据的长度。如果没有数据要发送，填0。
*		prcvlen,	同时做输入和输出参数，如果不需要输出净荷，可以为NULL。
*					调用函数的时候(*prcvlen)表示rcvbuf的长度，
*					当长度大于0的时候表示调用者希望得到响应消息的净荷，否则不输出净荷。
*		priority,	发送优先级，IBC_PRIORITY_PREMIER、IBC_PRIORITY_HIGH或IBC_PRIORITY_LOW
*		timeout,	超时tick数,大于0,时间为从驱动发送完消息以后算起，超时时间不包括发送队列处理时间。
*	输出参数:
*		rcvbuf,		接收数据缓冲区
*					函数调用者申请和释放，本函数内部填写，如果不需要输出净荷，可以为NULL
*		prcvlen,	同时做输入和输出参数，如果不需要输出净荷，可以为NULL。
*					函数返回的时候(*prcvlen)表示本函数实际拷贝进rcvbuf的数据长度
*		pexrsp,		*pexrsp中为返回消息中的扩展响应字，如果调用者不关心这个，pexrsp可以为NULL
*	返回值:
*		OK,			同步发送成功，如果rcvbuf不为空，其中已经有了返回的数据。
*		IBC_ERROR_INNER	数据错误，资源不够等错误
*		IBC_ERROR_QUEUE_FULL 发送队列满
*		IBC_ERROR_INVALID_DATA 数据无效
*		IBC_ERROR_FAILED_SENDING 发送失败
*		IBC_ERROR_NO_REPLY 无响应
*		IBC_ERROR_BAD_REPLY 响应数据有错
*		IBC_ERROR_LOOPBACK 设置了板间消息环回，就不向上层传递返回的报文了
*/
STATUS ibcadp_send_sync(const OTP_VOID *arg, OTP_UINT8 destslot,const rdev_rid* prid,OTP_UINT8 opcode,
							OTP_UINT8 *data,OTP_UINT32 datalen,	OTP_INT32 timeout);

/********************************************************************************
*ibc_msg_rcv_rc006 --RC006协议接收报文处理函数
*Input:
*	arg			--自定义参数，传递发送报文使用的函数
*	pbuf		--接收到的报文头指针	
*	len			--接收的报文长度
*Output:
*	None
*Return:
*	OK			--表示执行成功
*	ERROR		--表示执行错误
*/
STATUS ibc_msg_rcv_rc006(OTP_VOID *arg,OTP_UINT8 *pbuf, OTP_UINT16 len);


/*****************************************************************************
*ibc_rc006_init		ibc初始化函数
*
*/
OTP_VOID ibc_rc006_init(void);

#endif

