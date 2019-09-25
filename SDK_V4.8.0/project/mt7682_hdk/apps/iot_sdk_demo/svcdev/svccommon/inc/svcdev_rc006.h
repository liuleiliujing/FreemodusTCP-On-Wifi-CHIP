#ifndef _SVCDEV_COMMON_RC006_H
#define _SVCDEV_COMMON_RC006_H

/*定义svcdev 支持的同时的中断数*/
#define SVCDEV_INT_NUM_MAX	5
/*panwei 2011-9-1*/
/*业务盘盘的启动阶段*/
typedef enum svcdev_startup_stage
{
	svcdevs_init,			/*业务盘初始化阶段*/
	svcdevs_int_waiting,	/*已经拉了中断，等待中断消息*/
	svcdevs_cfgdone_waiting,/*已经发了IBC_INTCOMMON_CFGREQUEST中断消息，等待配置恢复结束消息，
							只要该单盘应该从网管盘下配置，网管盘就应该有配置恢复结束消息*/
	svcdevs_working		/*应该从网管盘下配置的单盘收到了配置恢复结束消息，
							或者是不从网管盘下配置单盘已经向网管盘发了IBC_INTCOMMON_CARDREADY中断响应*/
}svcdev_startup_stage;

/* 单盘提供的清除中断信号的接口函数 */
extern STATUS svc_clear_hardware_int(void);
/* 单盘提供的设置中断信号的接口函数 */
extern STATUS svc_set_hardware_int(void);

/*
 PARAMETERS参数 - As follow如下:
*		pack	--	响应数据区所用的buf地址
*		acklen	--	响应数据所用的buf长度		
*		plen	--	报文数据实际长度
* RETURN返回值:
*
*		OK 		--	响应处理成功
*		ERROR 	--	响应处理失败,有异常
*/
typedef STATUS (*int_pack_func)(OTP_UINT8 inttype, OTP_UINT8 *pack, OTP_UINT32 acklen, OTP_UINT32 *plen);


enum
{
	SVCDEV_INT_START,
	SVCDEV_INT_CFGREQUEST,			/*请求配置*/
	SVCDEV_INT_CMFFAILED,			/*命令执行错误*/
	SVCDEV_INT_RDEVURGENT,			/*远端设备紧急告警*/
	SVCDEV_INT_RDEVDISAPPER,		/*下挂设备消失*/
	SVCDEV_INT_CARDREADY,			/*业务盘准备好，表示单盘已经准备好，此单盘已经处于工作状态*/
	SVCDEV_INT_REQ_TRANSMIT,		/*:业务盘请求重新传输文件*/
};

/*定义中断请求的数据结构*/
typedef struct _svcdev_int_info
{
	BOOL			isexist;	/*中断是否存在*/
	int_pack_func 	intpack;	/**/
	OTP_UINT8		type;
}svcdev_int_info;

/*panwei 2012-7-19*/
#define DERIVE_PRODUCT_NAME_MAXLEN	50		/*派生名称（最大50字节)*/

/*定义派生握手操作发送帧*/
typedef struct _svcdev_derive_query_info
{
	OTP_UINT8  resByte;			/*保留，固定为0x00*/
	OTP_UINT8  proNameLen;		/*派生名称长度*/
	OTP_UINT8  proName[];		/*派生名称（最大50字节)*/
}OTP_PACK_ALIGN(1)svcdev_derive_query_info;

/*定义派生握手操作响应帧*/
typedef struct _svcdev_derive_query_ack_info
{
	OTP_UINT16 devt;			/*产品设备类型*/
	OTP_UINT8  resByte;			/*保留，固定为0x80*/
	OTP_UINT8  proNameLen;		/*派生名称长度*/
	OTP_UINT8  proName[];		/*派生名称（最大50字节)*/
}OTP_PACK_ALIGN(1)svcdev_derive_query_ack_info;

#define CLEAR_HARDWARE_INT		svc_clear_hardware_int()
#define SET_HARDWARE_INT		svc_set_hardware_int()


#define SVCDEV_CONFIG_DONE_WAITTIME	300000 /*单位为毫秒*/

 /*硬复位函数*/
 STATUS ResetSystem(HTMR htmr, OTP_UINT32 uarg);

 /*panwei 2011-3-10 软复位函数*/
 STATUS SoftResetSystem(HTMR htmr, OTP_UINT32 uarg);
#endif
