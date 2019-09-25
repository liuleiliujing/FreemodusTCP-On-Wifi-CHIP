/* ------------------------ System includes ------------------------------- */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* ------------------------ FreeModbus includes --------------------------- */
#include "mb.h"
#include "freertos.h"



/* ----------------------- Static variables ---------------------------------*/
#define mainCOM_TEST_BAUD_RATE  ( ( unsigned portLONG ) 38400 )

#define mainMB_TASK_PRIORITY    ( tskIDLE_PRIORITY + 3 )
#define REG_INPUT_START         100
#define REG_INPUT_NREGS         4
#define REG_HOLDING_START       0x01
#define REG_HOLDING_NREGS       100

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];
static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];
/* ------------------------ Static functions ------------------------------ */

/* ------------------------ Implementation -------------------------------- */
static UCHAR Vendor[11] = "Modbus TCP";
uint16_t  wifi_connect_flag=0;
uint16_t wifi_dhcp_flag=0;
uint16_t modbus_connect_flag =0;
void
modbus_task( void *arg )
{
     eMBErrorCode    xStatus;

     while(wifi_connect_flag == 0)
    {
    		printf("\r\n --modbus wifi not connected !\r\n");
		vTaskDelay(1000 / portTICK_RATE_MS);  
    }
    while(wifi_dhcp_flag == 0)
    {
    		printf("\r\n --!wifi not get dhcp\r\n");
		vTaskDelay(5000 / portTICK_RATE_MS);  
    }
     //MX_LWIP_Init();
    //struct ip4_addr_t  xIpAddr, xNetMast, xGateway;
    //struct netif    gnetif;

    //IP4_ADDR( &xIpAddr, 192, 168, 253, 2 );
    //IP4_ADDR( &xNetMast, 255, 255, 255, 0 );
    //IP4_ADDR( &xGateway, 192, 168, 253, 1 );
    //netif_add( &gnetif, &xIpAddr, &xNetMast, &xGateway, NULL, gnetif, tcpip_input );
    /* Make it the default interface */
    //netif_set_default( &gnetif );
    /* Bring it up */
    //netif_set_up( &gnetif );
    for(int i=0; i < 100; i++)
    {
	    usRegHoldingBuf[i] = i+100;
    }


    for( ;; )
    {
        if( eMBTCPInit( MB_TCP_PORT_USE_DEFAULT ) != MB_ENOERR )
        {
             
        }
	  if(eMBSetSlaveID( 1, TRUE, Vendor, sizeof(Vendor)) != MB_ENOERR)
	 {
	 	
	 }
         if( eMBEnable(  ) != MB_ENOERR )
        {
           
        }
	 while(modbus_connect_flag==0)
	 {
	 	printf("\r\n --!no modbus client connected\r\n");
		vTaskDelay(1000 / portTICK_RATE_MS);  
	 }
        //if (modbus_connect_flag==1)
        //{
	     printf("\r\n --modbus poll  start !\r\n");	
	     do
            {
			xStatus = eMBPoll(  );
			vTaskDelay(8 / portTICK_RATE_MS);  
            }
            while( xStatus == MB_ENOERR );
        //}
	printf("\r\n --modbus end !\r\n");
        /* An error occured. Maybe we can restart. */
        ( void )eMBDisable(  );
        ( void )eMBClose(  );
	vTaskDelay(1000 / portTICK_RATE_MS);  
    }

}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
            /* Pass current register values to the protocol stack. */
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}
