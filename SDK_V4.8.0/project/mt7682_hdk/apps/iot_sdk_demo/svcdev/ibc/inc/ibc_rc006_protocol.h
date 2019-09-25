#ifndef _IBC_RC006_PROTOCOL_H
#define _IBC_RC006_PROTOCOL_H
#include "memory_map.h"	/*panwei 2011-3-7*/

/*

发送帧中的操作类型码:
	0x01	查询				必须支持
	0x02	配置				有配置时支持
	0x03	命令				有命令时支持
	0x04	查询版本号			必须支持
	0x05	以太网带宽配置		以太网复用器支持
	0x06	以太网带宽配置查询	以太网复用器支持
	0x07	反向配置查询		必须支持
	0x08	设置报文环回		必须支持
	0x09	取消报文环回		必须支持
	0x0A	设备在位查询		必须支持
	0x0B	中断处理			必须支持
	0x0C	业务盘配置结束		必须支持
	0x0D	增加远端设备标识	PCM支持
	0x0E	删除远端设备标识	PCM支持
	0x10	业务盘心跳命令		必须支持
	0x11	业务盘硬复位		必须支持
	0x12	业务盘软复位
	0x13	读取CPU和内存使用率
	0x14	业务盘文件操作命令
	0x15	保存配置文件
	0x16	清除配置文件
	0x17	纯粹转发净荷，不加协议头，响应消息的净荷中为远端全报文
	0x18	发送给远端的同步消息，局端单盘等待远端有响应以后再发响应消息给网管盘
	0x19	启动下挂设备的告警中断		PCM支持
	0x1A	关闭下挂设备的告警中断		PCM支持
	0x1B	查询下挂设备列表（儿子级）	PCM支持
	0x1C	停止下挂设备通信			PCM支持
	0x1D	启动下挂设备通信			PCM支持
	0x1E	查询设备基本信息			版本查询的增强功能
	0x1F	查询业务盘的内存信息
	0x20	修改业务盘的内存信息
	0x21	配置同步请求		RC3000-15主备盘之间使用
	0x22	配置同步			RC3000-15主备盘之间使用
	0x23	业务盘系统命令		基于ROTP平台的业务盘支持，实现热补丁等与系统相关的命令
	0x24	扩展配置			有扩展配置时支持，在OPCOM3500E-STM4上使用
	0x25	dxc主备切换信息		在rc3000-15上用作主备切换相关信息的传递

	0x30~0x35	PCM支持，用于拓扑发现下挂设备

	0x70	硬件测试保留	用作硬件部门的内部测试
		
*/

#define IBC_OPCODE_QUERY			0x01
#define IBC_OPCODE_CONFIG			0x02
#define IBC_OPCODE_COMMAND			0x03
#define IBC_OPCODE_QUERY_VER		0x04
#define IBC_OPCODE_BANDWIDTH_CFG 	0x05
#define IBC_OPCODE_QUERY_BANDWIDTH 	0x06
#define IBC_OPCODE_REVERSE_CFG		0x07
#define IBC_OPCODE_LOOPBACK_SET		0x08
#define IBC_OPCODE_LOOPBACK_CANCEL	0x09
#define IBC_OPCODE_RDEV_PRESENCE	0x0A
#define IBC_OPCODE_INTERRUPT		0x0B
#define IBC_OPCODE_CONFIG_DONE		0x0C
#define IBC_OPCODE_ADD_DEVICEID		0x0D
#define IBC_OPCODE_DEL_DEVICEID		0x0E
#define IBC_OPCODE_HEART_BEAT		0x10
#define IBC_OPCODE_RESET_HARD		0x11
#define IBC_OPCODE_RESET_SOFT		0x12
#define IBC_OPCODE_QUERY_CPU_MEM	0x13
#define IBC_OPCODE_SVC_FILE_MANAGE	0x14
#define IBC_OPCODE_SAVE_STARTUP_CONFIG	0x15
#define IBC_OPCODE_ERASE_STARTUP_CONFIG	0x16
#define IBC_OPCODE_FORWARD_PAYLOAD	0x17
#define IBC_OPCODE_SYNC_REMOTE_MSG	0x18
#define IBC_OPCODE_STARTUP_DEV_ALM_INT	0x19
#define IBC_OPCODE_STOP_DEV_ALM_INT	0x1A
#define IBC_OPCODE_QUERY_DEV_LIST	0x1B
#define IBC_OPCODE_STOP_DEV_COMM	0x1C
#define IBC_OPCODE_STARTUP_DEV_COMM	0x1D
#define IBC_OPCODE_QUERY_DEVINFO	0x1E
#define IBC_OPCODE_QUERY_MEMINFO	0x1F
#define IBC_OPCODE_MODIFY_MEMINFO	0x20
#define IBC_OPCODE_REQ_CONFIG_SYNC	0x21
#define IBC_OPCODE_CONIFG_SYNC		0x22
#define IBC_OPCODE_SYS_CMD			0x23
#define IBC_OPCODE_EXT_CONFIG		0x24
#define IBC_OPCODE_DXCSWITCH		0x25
#define IBC_OPCODE_QUERY_MFINFO		0x29
#define IBC_OPCODE_DERIVE_HANDSHAKE		0x2e
#define IBC_OPCODE_DEV_DEBUG		0x71
/*

响应帧中的扩展响应字节：

	0x00	正常响应
	0x01	环回报文
	0x02	不支持的协议版本
	0x03	长度和校验和错
	0x04	跳过操作类型码，单盘不支持或在当前阶段不处理
	0x05	操作类型码错
	0x06	设备不在位（网管盘不会立即更新设备在位信息，以后也许有用）
	0x07	转发消息失败
	0x08  	远端设备紧急事件
	0x09	净荷数据错误
	0x0a	操作执行错误
*/
#define IBC_EXRSP_OK				0x00
#define IBC_EXRSP_LOOPBACK			0x01
#define IBC_EXRSP_ERROR_VERSION		0x02
#define IBC_EXRSP_ERROR_LEN_CRC		0x03
#define IBC_EXRSP_SKIP_OPCODE		0x04
#define IBC_EXRSP_ERROR_OPCODE		0x05
#define IBC_EXRSP_ERROR_DISAPPEAR	0x06
#define IBC_EXRSP_ERROR_FORWARDING	0x07
#define IBC_EXRSP_RDEV_URGENCE		0x08
#define IBC_EXRSP_PAYLOAD_ERROR		0x09
#define IBC_EXRSP_OP_ERROR			0x0a
#define IBC_EXRSP_OPERATING			0x0b
#define IBC_EXRSP_SKIP_CMD			0x0c
#define IBC_EXRSP_ERROR_NOT_MATCH 	0x0e

/*
	命令结果字，这里只定义一个OK，其他的由业务模块根据实际情况自己定义
	业务模块也可以不使用该定义，完全自己协调定义，这里只是为了方便
*/
#define IBC_OPREPLY_OK				0x00

/*公共中断类型
	BIT 0: 请求配置，此位置1表示存在请求配置
	BIT 1: 命令执行错误，此位置1表示命令执行错，"中断响应信息"中为执行错误的报文
	BIT 2: 远端设备紧急告警，本业务盘对应的远端设备报告了紧急告警，
		   远端设备的标识在"中断响应信息"中，格式与"设备在位查询"中远端设备标识一致
	BIT 3: 下挂设备消失，表明此业务盘下挂设备消失，具体下挂情况通过"下挂设备查询"获得。
	BIT 4: 业务盘准备好，表示单盘已经准备好，此单盘已经处于工作状态。
	       应用于业务盘无需下发配置的场合，如业务盘被配置为从模式，或者业务盘无配置项。
	BIT 5:业务盘请求重新传输文件，此位置1表示业务盘传输文件失败，需要重新传输文件，
		   "中断响应信息"中为要下载的操作类型、文件类型和保留二个字节。
			
	私有中断类型按照bit定义,表明与业务相关的中断类型，如设备紧急告警等，由具体设备的通信规程描述。
	只有私有中断类型为非0时，才表明存在与业务相关的中断类型。
*/
#define IBC_INTCOMMON_CFGREQUEST		((OTP_UINT16)(0x0001<<0))
#define IBC_INTCOMMON_CMDFIAIED			((OTP_UINT16)(0x0001<<1))
#define IBC_INTCOMMON_RDEV_URGENCE		((OTP_UINT16)(0x0001<<2))
#define IBC_INTCOMMON_RDEV_DISAPPEAR	((OTP_UINT16)(0x0001<<3))
#define IBC_INTCOMMON_CARDREADY			((OTP_UINT16)(0x0001<<4))
#define IBC_INTCOMMON_REQUEST_TRANSMIT	((OTP_UINT16)(0x0001<<5))


/*响应包中的保留字节的固定值*/
#define IBC_CMDACK_RESERVE	0x80

/*ibc_payload_xxx系列结构中,data字段相对于结构地址的偏移量*/
#define IBC_PAYLOAD_DATA_OFFSET(name)	((OTP_UINT32)(&((ibc_payload_##name*)0)->data))


/********************** RC004板间通信的公共定义********************/

/* RC004板间通信帧头字节, 存在两种帧头 */
/* 在PDH/RCMS等设备中为0x55,  在PCM等设备中为0xAA */
#define IBC_RC004_FRAME_HEADER_CHAR1		0x55
#define IBC_RC004_FRAME_HEADER_CHAR2		0xAA
#define IBC_RC004_FRAME_HEADER_LEN			0x02

/*RC004发送帧中的操作类型码:
	0x01	查询
	0x02	配置
	0x03	命令
	0x04	查询版本号
	0x05	以太网带宽配置		以太网复用器支持
	0x06	以太网带宽配置查询	以太网复用器支持
*/
#define IBC_RC004_OPCODE_QUERY				0x01
#define IBC_RC004_OPCODE_CONFIG				0x02
#define IBC_RC004_OPCODE_COMMAND			0x03
#define IBC_RC004_OPCODE_QUERY_VER			0x04
#define IBC_RC004_OPCODE_BANDWIDTH_CFG 		0x05
#define IBC_RC004_OPCODE_QUERY_BANDWIDTH 	0x06

/*RC004协议公共部分的发送帧 IBC_OPCODE_PUBLIC_004 */
typedef struct ibc_payload_004_public
{
	OTP_UINT8 frameHeader;  /* 004协议的起始字符 */
	OTP_UINT8 frameLen;     /* 004协议的操作信息帧长度 */
	OTP_UINT8 deviceId;     /* 004协议设备ID*/
	OTP_UINT8 opcode;		/* 004协议的操作类型码*/
	OTP_UINT8 data[];		/* 发送信息（0~n）*/
}OTP_PACK_ALIGN(1) ibc_payload_004_public;

/*RC004板间通讯的响应帧有两种格式*/
/*一种为PDH/RCMS等设备，其响应帧格式为:
  Start_Char（0x55），Pkg_Length，Device_Type, [Operate_Inf], Sum；*/
typedef struct ibc_payload_frame1_004_ack
{
	OTP_UINT8 frameHeader;  /* 004协议的起始字符 */
	OTP_UINT8 frameLen;     /* 004协议的操作信息帧长度 */
	OTP_UINT8 devt;     	/* 004协议设备类型*/
	OTP_UINT8 data[];		/* 发送信息（0~n）*/
}OTP_PACK_ALIGN(1) ibc_payload_frame1_004_ack;

/*一种为PCM设备，其响应帧格式为:
  Start_Char(0xAA)，Pkg_Length，Device_ID, Operate_Code, DeviceType，[Operate_ID, Operate_Inf]，Sum*/
typedef struct ibc_payload_frame2_004_ack
{
	OTP_UINT8 frameHeader;  /* 004协议的起始字符 */
	OTP_UINT8 frameLen;     /* 004协议的操作信息帧长度 */
	OTP_UINT8 deviceId;     /* 004协议设备ID*/
	OTP_UINT8 opcode;		/* 004协议的操作类型码*/
	OTP_UINT8 devt;			/* 004协议中的模块类型*/
	OTP_UINT8 data[];		/* 发送信息（0~n）*/
}OTP_PACK_ALIGN(1) ibc_payload_frame2_004_ack;

/* 最大的板间通讯协议报文长度*/
#define IBC_PAYLOAD_PROTOCOL_MAX_LEN	4096

/************************* 净荷部分的公共格式 **********************/

/*	查询发送帧 IBC_OPCODE_QUERY */
typedef struct ibc_payload_query
{
	OTP_UINT16 	devt;		/*设备类型*/
	OTP_UINT8 	reserve;	/*保留*/
	OTP_UINT8 	command;	/*命令字(1)*/
	OTP_UINT8 	data[];		/*查询信息（0~n）*/
}OTP_PACK_ALIGN(1) ibc_payload_query;

/*	查询响应帧 */
typedef struct ibc_payload_query_ack
{
	OTP_UINT16 devt;		/*模块类型*/
	OTP_UINT8 reserve;		/*保留，固定为IBC_CMDACK_RESERVE*/
	OTP_UINT8 commandack;	/*响应命令字*/
	OTP_UINT8 data[];		/*后续特定字节块（0~n）*/
}OTP_PACK_ALIGN(1) ibc_payload_query_ack;


/***************************************************************************/


/*	配置发送帧 IBC_OPCODE_CONFIG */
typedef struct ibc_payload_config
{
	OTP_UINT16 	devt;		/*设备类型*/
	OTP_UINT8 	reserve;	/*保留*/
	OTP_UINT8 	command;	/*命令字(1)*/
	OTP_UINT8 	data[];	/*配置信息（0~n）*/
}OTP_PACK_ALIGN(1) ibc_payload_config;

/*	配置发送帧 给004远端设备的版本*/
typedef struct ibc_payload_config_004
{
	OTP_UINT8 type004;		/*板卡类型（1）*/
	OTP_UINT8 data[];		/*跟004协议一致的其他字节（0~n）*/
}OTP_PACK_ALIGN(1) ibc_payload_config_004;

/*	配置发送帧 给002远端设备的版本*/
typedef struct ibc_payload_config_002
{
	OTP_UINT8 data[1];		/*完整002报文，长度不为0*/
}OTP_PACK_ALIGN(1) ibc_payload_config_002;


/*	配置响应帧 */
typedef struct ibc_payload_config_ack
{
	OTP_UINT16 devt;		/*模块类型(2)*/
	OTP_UINT8 reserve;		/*保留，固定为IBC_CMDACK_RESERVE*/
	OTP_UINT8 commandack;	/*响应命令字*/
	OTP_UINT8  opreply;		/*操作结果字节(1)*/
	OTP_UINT8 data[];		/*配置响应信息（0~n）*/
}OTP_PACK_ALIGN(1) ibc_payload_config_ack;

/*	配置响应帧，远端设备的版本 */
typedef struct ibc_payload_config_rdev_ack
{
	OTP_UINT8 data[1];		/*"配置响应信息块"是局端单盘根据自己的实现来填的，
							通常情况下应该不存在。
							如果局端单盘采用等待设备回响应消息以后才给网管盘回
							响应消息的话，就有可能在"配置响应信息块"中加上设备
							返回的净荷。*/
}OTP_PACK_ALIGN(1) ibc_payload_config_rdev_ack;


/***************************************************************************/




/*	命令发送帧 IBC_OPCODE_COMMAND*/
typedef struct ibc_payload_command
{
	OTP_UINT16 	devt;		/*设备类型*/
	OTP_UINT8 	reserve;	/*保留*/
	OTP_UINT8 	command;	/*命令字(1)*/
	OTP_UINT8 data[];		/*命令信息(0~n)*/
}OTP_PACK_ALIGN(1) ibc_payload_command;

/*	命令发送帧 给004远端设备的版本*/
typedef struct ibc_payload_command_004
{
	OTP_UINT8 type004;		/*板卡类型（1）*/
	OTP_UINT8 data[];		/*跟004协议一致的其他字节（0~n）*/
}OTP_PACK_ALIGN(1) ibc_payload_command_004;



/*	命令响应帧 */
typedef struct ibc_payload_command_ack
{
	OTP_UINT16 devt;		/*模块类型(2)*/
	OTP_UINT8 reserve;		/*保留，固定为IBC_CMDACK_RESERVE*/
	OTP_UINT8 commandack;	/*响应命令字*/
	OTP_UINT8  opreply;		/*操作结果字节(1)*/
	OTP_UINT8 data[];		/*命令响应信息（0~n）*/
}OTP_PACK_ALIGN(1) ibc_payload_command_ack;

/*	命令响应帧，远端设备的版本 */
typedef struct ibc_payload_command_rdev_ack
{
	OTP_UINT8 data[1];		/*"命令响应信息块"是局端单盘根据自己的实现来填的，
							通常情况下应该不存在。
							如果局端单盘采用等待设备回响应消息以后才给网管盘回
							响应消息的话，就有可能在"配置响应信息块"中加上设备
							返回的净荷。*/
}OTP_PACK_ALIGN(1) ibc_payload_command_rdev_ack;



/***************************************************************************/


/*	查询版本号发送帧 IBC_OPCODE_QUERY_VER*/



/*	查询版本号响应帧 */
typedef struct ibc_payload_query_ver_ack
{
	/* wangjiong add 2006-10-17: 新增了devt，与文档和各个板卡的实现保持一致 */
	OTP_UINT16 	devt;		/*模块类型*/
	
	OTP_UINT8	softver;	/*软件版本*/
	OTP_UINT8	hardver;	/*硬件版本*/
	OTP_UINT8	firmver;	/*固件版本*/
}OTP_PACK_ALIGN(1) ibc_payload_query_ver_ack;


/***************************************************************************/


/*	带宽配置发送帧 IBC_OPCODE_BANDWIDTH_CFG 只有给004远端设备的版本*/
typedef struct ibc_payload_bandwidth_004
{
	OTP_UINT8 type004;		/*板卡类型（1）*/
	OTP_UINT8 data[];		/*跟004协议一致的其他字节（0~n）*/
}OTP_PACK_ALIGN(1) ibc_payload_bandwidth_004;


/*	带宽配置响应帧 */
typedef struct ibc_payload_bandwidth_rdev_ack
{
	OTP_UINT8 data[1];		/*"带宽配置响应信息块"是局端单盘根据自己的实现来填的，
							通常情况下应该不存在。
							如果局端单盘采用等待设备回响应消息以后才给网管盘回
							响应消息的话，就有可能在"配置响应信息块"中加上设备
							返回的净荷。*/
}OTP_PACK_ALIGN(1) ibc_payload_bandwidth_rdev_ack;


/***************************************************************************/

/*	以太网带宽配置查询发送帧 IBC_OPCODE_QUERY_BANDWIDTH 只有给004远端设备的版本*/



/*	带宽配置响应帧 */
typedef struct ibc_payload_query_bandwidth_rdev_ack
{
	OTP_UINT8 data[1];		/*"带宽查询响应信息块"是局端单盘根据自己的实现来填的。*/
}OTP_PACK_ALIGN(1) ibc_payload_query_bandwidth_rdev_ack;


/***************************************************************************/



/*	反向配置查询发送帧 IBC_OPCODE_REVERSE_CFG */



/*	反向配置查询响应帧 */
typedef struct ibc_payload_reverse_cfg_ack
{
	OTP_UINT16 devt;		/*模块类型(2)*/
	OTP_UINT8 data[];		/*配置信息（0~n）*/
}OTP_PACK_ALIGN(1) ibc_payload_reverse_cfg_ack;

/* 反向配置查询响应帧净荷的最大长度*/
#define IBC_PAYLOAD_REVERSE_CFG_ACK_MAX_LEN	4096



/***************************************************************************/


/*	设置报文环回发送帧 IBC_OPCODE_LOOPBACK_SET */


/*	设置报文环回响应帧 */



/***************************************************************************/



/*	取消报文环回发送帧 IBC_OPCODE_LOOPBACK_CANCEL*/


/*	取消报文环回响应帧 */



/***************************************************************************/



/*	设备在位查询发送帧 IBC_OPCODE_RDEV_PRESENCE*/



/*	设备在位查询响应帧 */
typedef struct ibc_payload_rdev_presence_ack
{
	OTP_UINT16	presencenum;/*在位设备总数（2）*/
	rdev_rid	rids[];	/*远端设备标识(0~n)*/
}OTP_PACK_ALIGN(1) ibc_payload_rdev_presence_ack;
/* 设备在位查询响应帧净荷的最大长度*/
#define IBC_PAYLOAD_RDEV_PRESENCE_ACK_MAX_LEN	2048



/***************************************************************************/



/*	中断处理发送帧 IBC_OPCODE_INTERRUPT*/



/*	中断处理响应帧 */
typedef struct ibc_payload_interrupt_ack
{
	OTP_UINT16 	devt;		/*模块类型*/
	OTP_UINT16 	intcommon;	/*公共中断类型, IBC_INTCOMMON_CFGREQUEST等*/
	OTP_UINT8 	intprivate;	/*私有中断类型, 不为0时表示有私有中断，需要传递给业务模块处理*/
	OTP_UINT8 data[];		/*中断响应信息（0~n）*/
}OTP_PACK_ALIGN(1) ibc_payload_interrupt_ack;
/* 中断处理响应帧净荷的最大长度*/
#define IBC_PAYLOAD_INTERRUPT_ACK_MAX_LEN	2048


/***************************************************************************/



/*	配置结束发送帧 IBC_OPCODE_CONFIG_DONE */



/*	配置结束响应帧 */



/***************************************************************************/


/*增加下挂设备的每跳标识*/
typedef struct rdev_add_item
{
	OTP_UINT16	devt;			/*设备类型，为dev_type枚举值*/
	OTP_UINT8 	channel;		/*下挂设备对应的通道号,0表示默认通道*/
	OTP_UINT8	devid;			/*设备ID，局部意义*/
}OTP_PACK_ALIGN(1)rdev_add_item;/*定义为紧缩的，方便板间通信结构定义*/

/*	增加远端设备标识发送帧 IBC_OPCODE_ADD_DEVICEID */
typedef struct ibc_payload_add_device
{
	OTP_UINT8		hop;		/*跳数，既然要发该消息，至少有一跳*/
	rdev_add_item 	items[1];	/*设备标识项数组，既然要发该消息，至少有一跳*/
}OTP_PACK_ALIGN(1) ibc_payload_add_device;


/*	增加远端设备标识响应帧 */



/***************************************************************************/



/*删除下挂设备的每跳标识*/
typedef struct rdev_del_item
{
	OTP_UINT16	devt;			/*设备类型，为dev_type枚举值*/
	OTP_UINT8 	channel;		/*下挂设备对应的通道号,0表示默认通道*/
	OTP_UINT8	devid;			/*设备ID，局部意义*/
}OTP_PACK_ALIGN(1)rdev_del_item;/*定义为紧缩的，方便板间通信结构定义*/

/*	删除远端设备标识发送帧 IBC_OPCODE_DEL_DEVICEID */
typedef struct ibc_payload_del_device
{
	OTP_UINT8		hop;		/*跳数，既然要发该消息，至少有一跳*/
	rdev_del_item 	items[1];	/*设备标识项数组，既然要发该消息，至少有一跳*/
}OTP_PACK_ALIGN(1) ibc_payload_del_device;


/*	删除远端设备标识响应帧 */



/***************************************************************************/



/*	心跳命令发送帧 IBC_OPCODE_HEART_BEAT */



/*	心跳命令响应帧 */



/***************************************************************************/



/*	硬复位发送帧 IBC_OPCODE_RESET_HARD */


/*	硬复位响应帧 */


/***************************************************************************/


/*	软复位发送帧 IBC_OPCODE_RESET_SOFT */



/*	软复位响应帧 */



/***************************************************************************/




/*	读取CPU和内存使用率发送帧 IBC_OPCODE_QUERY_CPU_MEM */



/*	读取CPU和内存使用率响应帧 */
typedef struct ibc_payload_query_cpu_mem_ack
{
	OTP_UINT16 	devt;		/*设备类型*/
	OTP_UINT8 	cpurate;	/*CPU使用率*/
	OTP_UINT8 	memrate;	/*内存使用率*/
}OTP_PACK_ALIGN(1) ibc_payload_query_cpu_mem_ack;


/***************************************************************************/



/*	业务盘文件操作命令发送帧 IBC_OPCODE_SVC_FILE_MANAGE */
typedef struct _ibc_payload_svcfile_manage
{
	OTP_UINT16 	devt;		/*模块类型(2)*/
	OTP_UINT8 	reserve;	/*保留，固定为0x00 (1)*/
	OTP_UINT8 	command;	/*命令字(1)*/
	OTP_UINT8 	fileType;	/*文件类型(1)*/
	OTP_UINT8 	data[];	/*发送字节块（n）*/
}OTP_PACK_ALIGN(1) ibc_payload_svcfile_manage;

/*	业务盘文件操作命令响应帧 IBC_OPCODE_SVC_FILE_MANAGE */
typedef struct _ibc_payload_svcfile_manage_ack
{
	OTP_UINT8 	reserve;	/*保留，固定为IBC_CMDACK_RESERVE*/
	OTP_UINT8 	commandack;	/*响应命令字(1)*/
	OTP_UINT8 	data[];		/*响应字节块（n）*/
}OTP_PACK_ALIGN(1) ibc_payload_svcfile_manage_ack;

/*	业务盘文件操作命令的下载过程中、下载结束的发送字节块定义 */
typedef struct _ibc_payload_svcfile_manage_loaddata
{
	OTP_UINT8 	sessionID;		/**/
	OTP_UINT16 	serialNumber;	/*序号 (2)*/
	OTP_UINT16	dataLen;		/*文件字节块长度(2)*/
	OTP_UINT8 	info[];			/*文件字节块（n）*/
}OTP_PACK_ALIGN(1) ibc_payload_svcfile_manage_loaddata;

typedef struct _ibc_payload_svcfile_manage_mcu_loaddata
{
	OTP_UINT32	address;		/*信息块下载目的地址 (4)*/	
	OTP_UINT8 	info[];			/*文件字节块（n）*/
}OTP_PACK_ALIGN(1) ibc_payload_svcfile_manage_mcu_loaddata;

/*	业务盘文件操作命令的下载开始的应答信息定义 */
typedef struct _ibc_payload_svcfile_manage_start_ack
{
	OTP_UINT8 	sessionID;	/**/
	OTP_UINT16 	ibcMaxLen;	/*板间消息的最大长度(2)*/
	OTP_UINT32 	fileLen;		/*文件总长度(4)*/
}OTP_PACK_ALIGN(1) ibc_payload_svcfile_manage_start_ack;

/*	业务盘文件操作命令的 	开始下载、开始上传、删除文件的发送信息定义 */
typedef struct _ibc_payload_svcfile_manage_loaddata_start_info
{
	OTP_UINT32 	fileLen;			/*文件字长度(4)*/
	OTP_UINT8 	filename[];		/*文件名（n）*/
}OTP_PACK_ALIGN(1) ibc_payload_svcfile_manage_loaddata_start_info;


#define FILE_DOWNLOAD_MCU_IBC_MAX_LEN	260		/* 板间通讯文件下载最大长度*/
#define FILE_DOWNLOAD_FPGA_IBC_MAX_LEN	256		/* 板间通讯文件下载最大长度*/
#define FILE_DOWNLOAD_ADDRESS_SIZE		4		/* (下载目的地址宽度)*/
#define FILE_DOWNLOAD_DATA_MIN_LEN		FILE_DOWNLOAD_ADDRESS_SIZE		/* (下载目的地址)板间通讯文件下载有效净核的最小长度*/
#define FILE_DOWNLOAD_SESSION_ID			0x2A	/* 文件下载sessionID*/
#define FILE_DOWNLOAD_FILE_MAX_LEN		0		/* 文件总长度*/
#define FILE_DOWNLOAD_FLASH_ADDRESS		0x08002000		/* 下载目的FLASH地址*/



/* 业务盘文件操作命令字 */
#define IBC_FILE_CMD_NULL					0x00	/* 文件命令为空*/
#define IBC_FILE_CMD_DOWNLOAD_START		0x01	/* 文件开始下载*/
#define IBC_FILE_CMD_DOWNLOADING			0x02	/* 文件下载过程中*/
#define IBC_FILE_CMD_DOWNLOAD_END		0x03	/* 文件下载结束*/
#define IBC_FILE_CMD_UPLOAD_START			0x04	/* 文件开始上传*/
#define IBC_FILE_CMD_UPLOADING			0x05	/* 文件上传过程中*/
#define IBC_FILE_CMD_UPLOAD_END			0x06	/* 文件上传结束*/
#define IBC_FILE_CMD_DELETE_FILE			0x07	/* 文件删除文件*/
#define IBC_FILE_CMD_QUERY_RESULT			0x08	/* 文件查询结果*/

/* 业务盘文件类型*/
#define IBC_FILE_TYPE_NULL					0x00	/* 升级文件为空*/
#define IBC_FILE_TYPE_FPGAUPDATE			0x01	/* 升级fpga文件*/
#define IBC_FILE_TYPE_SYSUPDATE			0x02	/* 升级系统软件*/
#define IBC_FILE_TYPE_STARTUP_CFG			0x03	/* 升级配置文件*/
#define IBC_FILE_TYPE_COMMON_FILE			0x04	/* 普通文件（全路径文件）*/
#define IBC_FILE_TYPE_BOOTROM				0x05	/* 升级bootrom文件*/
#define IBC_FILE_TYPE_FPGA2UPDATE			0x06	/* 升级fpga2文件*/
#define IBC_FILE_TYPE_FILELIST				0x07	/* 获取文件列表*/
#define IBC_FILE_TYPE_MCUUPDATE			0x08	/* 升级MCU文件*/

/* 业务盘文件查询结果*/
#define IBC_FILE_RESULT_SUCCESS			0x00	/* 文件操作成功*/
#define IBC_FILE_RESULT_UNSUCCESS			0x01	/* 文件操作失败*/
#define IBC_FILE_RESULT_WR_FLASH_ERROR	0x04	/* 写flash错误*/
#define IBC_FILE_RESULT_FILE_TYPE_ERROR	0x05	/* 无效的文件类型*/
#define IBC_FILE_RESULT_SESSIONID_ERROR	0x0F	/* sessionid不匹配*/
#define IBC_FILE_RESULT_NUMBER_ERROR		0x10	/* 文件块序列号不匹配*/
#define IBC_FILE_RESULT_CRC8_ERROR		0x13	/* CRC8校验错*/


/*下载文件类型*/
typedef enum _e_ibc_fileload_type
{
	IBC_NULL_FILE		= 0,
	IBC_HEX_FILE		= 1,
	IBC_BIN_FILE 		= 2
	
}e_ibc_fileload_type;

/*下载文件类型*/
typedef enum _e_ibc_fileload_device_type
{
	IBC_NULL_DEVICE	= 0,
	IBC_MCU_DEVICE	= 1,
	IBC_FPGA_DEVICE	= 2
	
}e_ibc_fileload_device_type;

/*ibc_file_downlaod_process	--文件下载处理函数
*Input:
*	filetype		--文件类型
*	devicetype	--设备类型
*	command	--下载命令
*	pdata		--下载数据指针
*	length		--数据长度
*Output:
*	无
*Return:
*	OK 		--正确
*	ERROR 	--错误
*/
typedef STATUS (* ibc_file_downlaod_process)(	OTP_UINT16 file_type,
							OTP_UINT16 device_type,
							OTP_UINT16 command_code,
							OTP_UINT8* pdata,
							OTP_UINT32 length);


/*文件下载管理通用变量定义*/
typedef struct ibc_payload_svcfile_manage_loaddata_global_info
{
	OTP_UINT16 	serialNumber;		/*接收帧序号(2)*/
	OTP_UINT8	FileType;			/*文件类型(1)*/
	OTP_UINT8	QueryResult;		/*下载命令应答(1)*/	
}OTP_PACK_ALIGN(1) ibc_payload_svcfile_manage_loaddata_global_info;



/***************************************************************************/


/*	保存启动配置发送帧 IBC_OPCODE_SAVE_STARTUP_CONFIG */



/*	保存启动配置响应帧 */


/***************************************************************************/


/*	清除启动配置发送帧 IBC_OPCODE_ERASE_STARTUP_CONFIG */



/*	清除启动配置响应帧 */


/***************************************************************************/


/*	启动下挂设备的告警中断发送帧 IBC_OPCODE_STARTUP_DEV_ALM_INT */



/*	启动下挂设备的告警中断响应帧 */


/***************************************************************************/


/*	关闭下挂设备的告警中断发送帧 IBC_OPCODE_STOP_DEV_ALM_INT */


/*	关闭下挂设备的告警中断响应帧 */



/***************************************************************************/


/*	查询下挂设备列表（儿子级）发送帧 IBC_OPCODE_QUERY_DEV_LIST */



/*	查询下挂设备列表（儿子级）响应帧 */
typedef struct ibc_payload_query_dev_list_ack
{
	OTP_UINT8	devicenum;	  /* 下挂设备总数,为0时表示没有下挂设备，单字节 */
	rdev_rid_item	rids[];	  /*下挂设备列表(0~n)*/
}OTP_PACK_ALIGN(1) ibc_payload_query_dev_list_ack;
/* 下挂设备查询响应帧净荷的最大长度*/
#define IBC_PAYLOAD_RDEV_LIST_ACK_MAX_LEN	2048


/***************************************************************************/


/*	停止下挂设备通信命令 IBC_OPCODE_STOP_DEV_COMM */


/*	停止下挂设备通信命令响应帧 */



/***************************************************************************/


/*	启动下挂设备通信命令 IBC_OPCODE_STARTUP_DEV_COMM */



/*	启动下挂设备通信命令响应帧 */



/***************************************************************************/

/*	查询设备基本信息发送帧 IBC_OPCODE_QUERY_DEVINFO*/


/* xujingru-2007-12-21 修改了固件版本的位域定义，小版本占用一个字节
   关于业务盘版本的范围说明:
   1、软件版本:
      主板本:1-15;功能版本:0-63;bug版本号:0-63
   2、固件版本:
      大版本:1-15;小版本:0-255;
   3、硬件版本:
      大版本:1-15;小版本:0-9*/
/*	查询设备基本信息响应帧 

	软件版本信息bit位定义(包含bootrom、rotp、软件版本)
	Bit15~Bit12	XX:主版本号,表示软件系统发生重大调整或者硬件做了重大调整;
	Bit11~Bit6	YY:功能版本号,表示软件功能特性的增加或者改进;
	Bit5~Bit0	ZZ:BUG版本号,表示在XX版本标识下进行的bug修改的次数;

	固件版本信息bit位定义(包含FPGA、CPLD等)
	Bit15~Bit12	保留
	Bit15~Bit8  大版本定义，表示固化程序代码功能发生重大调整或者硬件做了重大调整，
				或者不能保持向下兼容，该标识定义固化程序代码的重大升级；
				表示数字，从1开始。	
	Bit7~Bit0	小版本定义，表示一般功能特性的增减或者bug的修改，
				该标识定义固化程序代码的一般功能特性的增减或者bug的修改；
				表示数字，从0开始。
	
	硬件版本信息bit位定义
	Bit15~Bit12	保留
	Bit11~Bit8  大版本定义，取值范围为"0001"～"1111"，"0000"禁止使用；
	-----------------------------------------------------------------			
				大版本的二进制编码对应表
				编码	版本
				0000	禁止使用
				0001	A
				0010	B
				0011	C
				0100	D
				0101	E
				0110	F
				0111	G
				1000	H
				1001	I
				1010	J
				1011	K
				1100	L
				1101	M
				1110	N
				1111	O
	------------------------------------------------------------------
	Bit7~Bit4	保留
	Bit3~Bit0	小版本定义，取值范围为"0000"～"1001"，"1010"～"1111"禁止使用
	------------------------------------------------------------------
				小版本的二进制编码对应表				
				编码	版本
				0000	0
				0001	1
				0010	2
				0011	3
				0100	4
				0101	5
				0110	6
				0111	7
				1000	8
				1001	9
	------------------------------------------------------------------

	软件的编译时间字符串 : Mon  data year time eg: Dec  6 2006 14:26:31

	固件的生成时间:	相对于2000年1月1日0时0分0秒
	bit位定义:
	
	Bit31~Bit26	year:2000开始递增;
	Bit25~Bit22	month:1~12;
	Bit21~Bit17	day:1~31;
	
	Bit16~Bit12	hour:0~23;
	Bit11~Bit6	minute:0~59;
	Bit5~Bit0	second:0~59;
	
*/

typedef struct ibc_payload_query_devinfo_ack
{
	OTP_UINT16 	devt;		/*模块类型*/	
	OTP_UINT16 	devtext;	/*模块类型扩展，不通单盘自己定义，缺省为0*/

	/*软件相关信息*/
	OTP_UINT16	bootromver;	/*bootrom版本*/
	OTP_UINT16	rotpver;	/*rotp版本*/
	OTP_UINT16	softver;	/*软件版本*/
	OTP_UINT8	softtime[32];	/* 编译时间的字符串*/

	OTP_UINT16	hardver;	/*硬件版本*/
	
	/*固件相关信息，对于cpld和fpga如果没有则缺省添0*/
	OTP_UINT8	firmwarecount;	/* 固件的数目 */
	
	OTP_UINT16	descriptionlen;	/* 描述信息长度 */
	
	struct firmware{
		OTP_UINT8	type;	/* 0 : cpld ,   1 : fpga ,	2:unknow; */
		OTP_UINT16	ver;	/*版本*/
		OTP_UINT32	time;	/*生成时间,相对于1970年1月1日0时0分0秒的秒数*/
	} OTP_PACK_ALIGN(1)firmware[];

}OTP_PACK_ALIGN(1) ibc_payload_query_devinfo_ack;
/* 查询设备基本信息响应帧净荷的最大长度*/
#define IBC_PAYLOAD_QUERY_DEVINFO_ACK_MAX_LEN	1024

/***************************************************************************/

/*	查询设备的内存信息发送帧 IBC_OPCODE_QUERY_MEMINFO*/
typedef struct ibc_payload_query_meminfo
{
	OTP_UINT32 adrs;		/* address to display (if 0, display next block */
    OTP_UINT16 nunits;		/* number of units to print (if 0, use default) */
    OTP_UINT8 width; 		/* width of displaying unit (1, 2, 4, 8) */
}OTP_PACK_ALIGN(1) ibc_payload_query_meminfo;

/*	查询设备的内存信息响应帧 */
typedef struct ibc_payload_query_meminfo_ack
{
	OTP_UINT16 memlength;		/*信息串长度*/
	OTP_UINT8 meminfo[];		/*内存信息串*/
}OTP_PACK_ALIGN(1) ibc_payload_query_meminfo_ack;

/*	修改设备的内存信息发送帧 IBC_OPCODE_MODIFY_MEMINFO */
typedef struct ibc_payload_modify_meminfo
{
	OTP_UINT32 adrs;		/* address to display (if 0, display next block */
    OTP_UINT8  width; 		/* width of displaying unit (1, 2, 4, 8) */
   	OTP_UINT32 value;		/* value of modifing memory */
}OTP_PACK_ALIGN(1) ibc_payload_modify_meminfo;

/*	修改设备的内存信息响应帧 */
typedef struct ibc_payload_modify_meminfo_ack
{
	OTP_UINT16 memlength;		/*信息串长度*/
	OTP_UINT8 meminfo[];		/*内存信息串*/
}OTP_PACK_ALIGN(1) ibc_payload_modify_meminfo_ack;

/* 查询、修改设备的内存信息响应帧净荷的最大长度*/
#define IBC_PAYLOAD_MEMINFO_ACK_MAX_LEN	1024

/***************************************************************************/
/*	配置同步发送帧 IBC_OPCODE_CONIFG_SYNC */
typedef struct ibc_payload_config_sync
{
	OTP_UINT16 	devt;		/*设备类型*/
	OTP_UINT8 	reserve;	/*保留*/
	OTP_UINT8 	command;	/*命令字(1)*/
	OTP_UINT8 	data[];	/*配置信息（0~n）*/
}OTP_PACK_ALIGN(1) ibc_payload_config_sync;

/*	配置同步响应帧 */
typedef struct ibc_payload_config_sync_ack
{
	OTP_UINT16 devt;		/*模块类型(2)*/
	OTP_UINT8 reserve;		/*保留，固定为IBC_CMDACK_RESERVE*/
	OTP_UINT8 commandack;	/*响应命令字*/
	OTP_UINT8  opreply;		/*操作结果字节(1)*/
	OTP_UINT8 data[];		/*配置响应信息（0~n）*/
}OTP_PACK_ALIGN(1) ibc_payload_config_sync_ack;


/***************************************************************************/

/*	配置同步请求发送帧 IBC_OPCODE_REQ_CONFIG_SYNC */
typedef struct ibc_payload_req_config_sync
{
	OTP_UINT16 	devt;		/*设备类型*/
	OTP_UINT8 	reserve;	/*保留*/
	OTP_UINT8 	command;	/*命令字(1)*/
	OTP_UINT8 	data[];	/*配置信息（0~n）*/
}OTP_PACK_ALIGN(1) ibc_payload_req_config_sync;



/*	配置同步请求响应帧 */
typedef struct ibc_payload_req_config_sync_ack
{
	OTP_UINT16 devt;		/*模块类型(2)*/
	OTP_UINT8 reserve;		/*保留，固定为IBC_CMDACK_RESERVE*/
	OTP_UINT8 commandack;	/*响应命令字*/
	OTP_UINT8  opreply;		/*操作结果字节(1)*/
	OTP_UINT8 data[];		/*配置响应信息（0~n）*/
}OTP_PACK_ALIGN(1) ibc_payload_req_config_sync_ack;


/***************************************************************************/


/*	命令发送帧 IBC_OPCODE_SYS_CMD*/
typedef struct ibc_payload_sys_cmd
{
	OTP_UINT16 	devt;		/*设备类型*/
	OTP_UINT8 	reserve;	/*保留*/
	OTP_UINT8 	command;	/*命令字(1)*/
	OTP_UINT8 data[];		/*命令信息(0~n)*/
}OTP_PACK_ALIGN(1) ibc_payload_sys_cmd;


/*	命令响应帧 */
typedef struct ibc_payload_sys_cmd_ack
{
	OTP_UINT16 devt;		/*模块类型(2)*/
	OTP_UINT8 reserve;		/*保留，固定为IBC_CMDACK_RESERVE*/
	OTP_UINT8 commandack;	/*响应命令字*/
	OTP_UINT8  opreply;		/*操作结果字节(1)*/
	OTP_UINT8 data[];		/*命令响应信息（0~n）*/
}OTP_PACK_ALIGN(1) ibc_payload_sys_cmd_ack;

/* 业务盘系统命令字 */
#define IBC_SYS_CMD_PATCH_LOAD_MODULE	0x01	/* 加载模块 */
#define IBC_SYS_CMD_LKUP_SYMBOL			0x02	/* 查找符号对应地址 */
#define IBC_SYS_CMD_LKADDR_ENTRY		0x03	/* 查找地址对应函数 */
#define IBC_SYS_CMD_PATCH_FUNCTION		0x04	/* 函数补丁 */
#define IBC_SYS_CMD_NO_PATCH_FUNCTION	0x05	/* 取消函数补丁 */
#define IBC_SYS_CMD_PATCH_ENTRY			0x06	/* 地址补丁 */
#define IBC_SYS_CMD_NO_PATCH_ENTRY		0x07	/* 取消地址补丁 */
#define IBC_SYS_CMD_SHOW_PATCH			0x08	/* 显示补丁信息 */

#endif
