
#ifndef		__INCLUDE_SVC_DEV_H
#define		__INCLUDE_SVC_DEV_H


#ifdef __cplusplus
extern "C" {
#endif






/*UART1 从DMA中接收消息的数据长度*/
#define	UART1_RECEIVE_LEN 	512

typedef struct {
    int message_len;
    uint8_t receive_data[UART1_RECEIVE_LEN];
} uart1_receive_message_struct_t;



/*接收数据消息的数据长度*/
#define	SVC_RECEIVE_DATA_MSG_LEN 	512

typedef struct {
	int message_len;
	uint8_t  message_type;
	uint8_t receive_data[SVC_RECEIVE_DATA_MSG_LEN];
} svc_receive_data_msg_struct_t;

/*接收控制消息的数据长度*/
#define	SVC_RECEIVE_MANAGE_MSG_LEN 	512

typedef struct {
	int message_len;
	uint8_t  message_type;	
	uint8_t receive_manage[SVC_RECEIVE_MANAGE_MSG_LEN];
} svc_receive_manage_msg_struct_t;












/********************************************************************
* svc_dev_init -- 业务盘相关操作初始化
*
* PARAMETERS - As follow:
*
* RETURNS:	
*
* EXAMPLE:
* ...
*/
void svc_dev_init(void);


#ifdef __cplusplus
}
#endif


#endif

