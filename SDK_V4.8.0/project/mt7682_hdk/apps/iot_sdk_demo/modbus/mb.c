/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006-2018 Christian Walter <cwalter@embedded-solutions.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbfunc.h"
#include "mbport.h"

#if MB_RTU_ENABLED == 0
//#include "mbrtu.h"
#endif
#if MB_ASCII_ENABLED == 0
//#include "mbascii.h"
#endif
#if MB_TCP_ENABLED == 1
#include "mbtcp.h"
#endif

#ifndef MB_PORT_HAS_CLOSE
#define MB_PORT_HAS_CLOSE 0
#endif

/* ----------------------- Static variables ---------------------------------*/

static UCHAR    ucMBAddress;       //modbus´Ó»úµØÖ·£¬0-255
static eMBMode  eMBCurrentMode;   //modbus Ð­ÒéÕ»ÀàÐÍ
static enum
{
    STATE_ENABLED,
    STATE_DISABLED,
    STATE_NOT_INITIALIZED
} eMBState = STATE_NOT_INITIALIZED;

/* Functions pointer which are initialized in eMBInit( ). Depending on the
 * mode (RTU or ASCII) the are set to the correct implementations.
 */
 
/* Mb.c ÊÇÒ»¸ö¿ò¼Ü£¬°üº¬Èý¸öÐ­Òé£¬¾ßÌåÊµÏÖÓÉÃ¿¸öÐ­ÒéÀïÃæµÄº¯ÊýÈ¥ÊµÏÖ
 * ÕâÀïÊ¹ÓÃº¯ÊýÖ¸Õë£¬½«ÉÏ²ãÓëµ×²ã·Ö¿ª
 */
static peMBFrameSend peMBFrameSendCur;   //·¢ËÍº¯Êý  
static pvMBFrameStart pvMBFrameStartCur;   //¿ªÊ¼º¯Êý
static pvMBFrameStop pvMBFrameStopCur;    //Í£Ö¹º¯Êý
static peMBFrameReceive peMBFrameReceiveCur; //½ÓÊÕº¯Êý
static pvMBFrameClose pvMBFrameCloseCur;   //¹Ø±Õº¯Êý

/* Callback functions required by the porting layer. They are called when
 * an external event has happend which includes a timeout or the reception
 * or transmission of a character.
 */
BOOL( *pxMBFrameCBByteReceived ) ( void );       //½ÓÊÕ»Øµ÷º¯Êý
BOOL( *pxMBFrameCBTransmitterEmpty ) ( void );    //·¢ËÍ»Øµ÷º¯Êý
BOOL( *pxMBPortCBTimerExpired ) ( void );   //¶¨Ê±Æ÷³¬Ê±»Øµ÷º¯Êý

BOOL( *pxMBFrameCBReceiveFSMCur ) ( void );     //½ÓÊÕÖÐ¶Ïº¯Êý
BOOL( *pxMBFrameCBTransmitFSMCur ) ( void );   //·¢ËÍÖÐ¶Ïº¯Êý

/* An array of Modbus functions handlers which associates Modbus function
 * codes with implementing functions.
 */
 //¶¨ÒåÒ»¸ömodbus¹¦ÄÜÂë´¦Àíº¯ÊýÊý×é
static xMBFunctionHandler xFuncHandlers[MB_FUNC_HANDLERS_MAX] = {
#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0
    {MB_FUNC_OTHER_REPORT_SLAVEID, eMBFuncReportSlaveID},
#endif
#if MB_FUNC_READ_INPUT_ENABLED > 0
    {MB_FUNC_READ_INPUT_REGISTER, eMBFuncReadInputRegister},
#endif
#if MB_FUNC_READ_HOLDING_ENABLED > 0
    {MB_FUNC_READ_HOLDING_REGISTER, eMBFuncReadHoldingRegister},      //03
#endif
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_REGISTERS, eMBFuncWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_REGISTER, eMBFuncWriteHoldingRegister},    //06
#endif
#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
    {MB_FUNC_READWRITE_MULTIPLE_REGISTERS, eMBFuncReadWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_READ_COILS_ENABLED > 0
    {MB_FUNC_READ_COILS, eMBFuncReadCoils},
#endif
#if MB_FUNC_WRITE_COIL_ENABLED > 0
    {MB_FUNC_WRITE_SINGLE_COIL, eMBFuncWriteCoil},
#endif
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_COILS, eMBFuncWriteMultipleCoils},
#endif
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
    {MB_FUNC_READ_DISCRETE_INPUTS, eMBFuncReadDiscreteInputs},
#endif
};

/* ----------------------- Start implementation -----------------------------*/

/* -------- ÊµÏÖmodbusÐ­Òé³õÊ¼»¯£¬°üÀ¨RTUºÍASC|| -----------*/
eMBErrorCode
eMBInit( eMBMode eMode, UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    /* check preconditions */
    if( ( ucSlaveAddress == MB_ADDRESS_BROADCAST ) ||
        ( ucSlaveAddress < MB_ADDRESS_MIN ) || ( ucSlaveAddress > MB_ADDRESS_MAX ) )
    {
        eStatus = MB_EINVAL;
    }
    else
    {
        ucMBAddress = ucSlaveAddress;

        switch ( eMode )
        {
#if MB_RTU_ENABLED > 0
        case MB_RTU:
            pvMBFrameStartCur = eMBRTUStart;
            pvMBFrameStopCur = eMBRTUStop;
            peMBFrameSendCur = eMBRTUSend;
            peMBFrameReceiveCur = eMBRTUReceive;
            pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBPortClose : NULL;
            pxMBFrameCBByteReceived = xMBRTUReceiveFSM;
            pxMBFrameCBTransmitterEmpty = xMBRTUTransmitFSM;
            pxMBPortCBTimerExpired = xMBRTUTimerT35Expired;

            eStatus = eMBRTUInit( ucMBAddress, ucPort, ulBaudRate, eParity );
            break;
#endif
#if MB_ASCII_ENABLED > 0
        case MB_ASCII:
            pvMBFrameStartCur = eMBASCIIStart;
            pvMBFrameStopCur = eMBASCIIStop;
            peMBFrameSendCur = eMBASCIISend;
            peMBFrameReceiveCur = eMBASCIIReceive;
            pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBPortClose : NULL;
            pxMBFrameCBByteReceived = xMBASCIIReceiveFSM;
            pxMBFrameCBTransmitterEmpty = xMBASCIITransmitFSM;
            pxMBPortCBTimerExpired = xMBASCIITimerT1SExpired;

            eStatus = eMBASCIIInit( ucMBAddress, ucPort, ulBaudRate, eParity );
            break;
#endif
        default:
            eStatus = MB_EINVAL;
        }

        if( eStatus == MB_ENOERR )
        {
            if( !xMBPortEventInit(  ) )
            {
                /* port dependent event module initalization failed. */
                eStatus = MB_EPORTERR;
            }
            else
            {
                eMBCurrentMode = eMode;
                eMBState = STATE_DISABLED;
            }
        }
    }
    return eStatus;
}


/* -------- ÊµÏÖmodbus TCP ³õÊ¼»¯ -----------*/
#if MB_TCP_ENABLED > 0
eMBErrorCode
eMBTCPInit( USHORT ucTCPPort )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( ( eStatus = eMBTCPDoInit( ucTCPPort ) ) != MB_ENOERR )
    {
        eMBState = STATE_DISABLED;
    }
    else if( !xMBPortEventInit(  ) )
    {
        /* Port dependent event module initalization failed. */
        eStatus = MB_EPORTERR;
    }
    else
    {
        pvMBFrameStartCur = eMBTCPStart;
        pvMBFrameStopCur = eMBTCPStop;
        peMBFrameReceiveCur = eMBTCPReceive;
        peMBFrameSendCur = eMBTCPSend;
        pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBTCPPortClose : NULL;
        ucMBAddress = MB_TCP_PSEUDO_ADDRESS;
        eMBCurrentMode = MB_TCP;
        eMBState = STATE_DISABLED;
    }
    return eStatus;
}
#endif
//×¢²áÐÂµÄ¹¦ÄÜÂëºÍÏàÓ¦µÄ´¦Àíº¯Êýµ½¹¦ÄÜÂëÊý×éÖÐ£¬
eMBErrorCode
eMBRegisterCB( UCHAR ucFunctionCode, pxMBFunctionHandler pxHandler )
{
    int             i;
    eMBErrorCode    eStatus;

    if( ( 0 < ucFunctionCode ) && ( ucFunctionCode <= 127 ) )
    {
        //ENTER_CRITICAL_SECTION(  );
        if( pxHandler != NULL )
        {
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                if( ( xFuncHandlers[i].pxHandler == NULL ) ||
                    ( xFuncHandlers[i].pxHandler == pxHandler ) )
                {
                    xFuncHandlers[i].ucFunctionCode = ucFunctionCode;
                    xFuncHandlers[i].pxHandler = pxHandler;
                    break;
                }
            }
            eStatus = ( i != MB_FUNC_HANDLERS_MAX ) ? MB_ENOERR : MB_ENORES;
        }
        else
        {
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                if( xFuncHandlers[i].ucFunctionCode == ucFunctionCode )
                {
                    xFuncHandlers[i].ucFunctionCode = 0;
                    xFuncHandlers[i].pxHandler = NULL;
                    break;
                }
            }
            /* Remove can't fail. */
            eStatus = MB_ENOERR;
        }
       // EXIT_CRITICAL_SECTION(  );
    }
    else
    {
        eStatus = MB_EINVAL;
    }
    return eStatus;
}


eMBErrorCode
eMBClose( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState == STATE_DISABLED )
    {
        if( pvMBFrameCloseCur != NULL )
        {
            pvMBFrameCloseCur(  );
        }
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

//Ê¹ÄÜ½ÓÊÕÖÐ¶ÏºÍ¿ªÆô¶¨Ê±Æ÷£¬½ÓÊÕÖÐ¶ÏÓÃÀ´½ÓÊÕÖ÷Õ¾·¢ËÍµÄÊý¾Ý£¬¶¨Ê±Æ÷ÓÃÀ´³¬Ê±¼ì²â
eMBErrorCode
eMBEnable( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState == STATE_DISABLED )
    {
        /* Activate the protocol stack. */
        pvMBFrameStartCur(  );
        eMBState = STATE_ENABLED;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

//¹Ø±ÕUART½ÓÊÕÖÐ¶ÏºÍ·¢ËÍÖÐ¶Ï£¬¹Ø±Õ¶¨Ê±Æ÷
eMBErrorCode
eMBDisable( void )
{
    eMBErrorCode    eStatus;

    if( eMBState == STATE_ENABLED )
    {
        pvMBFrameStopCur(  );
        eMBState = STATE_DISABLED;
        eStatus = MB_ENOERR;
    }
    else if( eMBState == STATE_DISABLED )
    {
        eStatus = MB_ENOERR;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBPoll( void )
{
    static UCHAR   *ucMBFrame;   //½ÓÊÕºÍ·¢ËÍ±¨ÎÄÊý¾Ý»º³åÇø
    static UCHAR    ucRcvAddress;    //modbus ´Ó»úµØÖ·
    static UCHAR    ucFunctionCode;    // ¹¦ÄÜÂë
    static USHORT   usLength;              //±¨ÎÄ³¤¶È
    static eMBException eException;      //´íÎóÂëÏìÓ¦Ã¶¾Ù

    int             i;
    eMBErrorCode    eStatus = MB_ENOERR;    //modbusÐ­ÒéÕ»´íÎóÂë
    eMBEventType    eEvent;             //ÊÂ¼þ±êÖ¾Ã¶¾Ù
    
    /* Check if the protocol stack is ready. */
    if( eMBState != STATE_ENABLED )    //¼ì²éÐ­ÒéÕ»ÊÇ·ñÊ¹ÄÜ
    {
        return MB_EILLSTATE;         //Ð­ÒéÕ»Î´Ê¹ÄÜ£¬·µ»ØÐ­ÒéÕ»ÎÞÐ§´íÎóÂë
    }
    /* Check if there is a event available. If not return control to caller.
     * Otherwise we will handle the event. */
    if( xMBPortEventGet( &eEvent ) == TRUE )    //²éÑ¯ÄÄ¸öÊÂ¼þ·¢Éú
    {
	 //printf("\r\n @@%s,LINE=%d,event=%d\r\n",__func__,__LINE__,eEvent);
	 switch ( eEvent )
        {
        case EV_READY:      //Æô¶¯Íê³
            break;

        case EV_FRAME_RECEIVED:
            eStatus = peMBFrameReceiveCur( &ucRcvAddress, &ucMBFrame, &usLength );
            if( eStatus == MB_ENOERR )
            {
                /* Check if the frame is for us. If not ignore the frame. */
		  //ÅÐ¶ÏÊÇ·ñÊÇ±¾»úµØÖ·»ò¹ã²¥µØÖ·
                if( ( ucRcvAddress == ucMBAddress ) || ( ucRcvAddress == MB_ADDRESS_BROADCAST ) )
                {
                    ( void )xMBPortEventPost( EV_EXECUTE );  //ÐÞ¸ÄÊÂ¼þ±êÖ¾ÎªEV_EXECUTE
                    //printf("\r\n @@%s,LINE=%d,event=%d\r\n",__func__,__LINE__,eEvent);
                }
            }
            break;

        case EV_EXECUTE:     //¶Ô½ÓÊÕµ½µÄ±¨ÎÄ½øÐÐ´¦ÀíÊÂ¼þ
            ucFunctionCode = ucMBFrame[MB_PDU_FUNC_OFF]; //»ñÈ¡PDUµÚÒ»¸ö×Ö½Ú£¬Îª¹¦ÄÜÂë
            eException = MB_EX_ILLEGAL_FUNCTION;    // ¸³´íÎóÂë´¦ÖÃÎªÎÞÐ§µÄ¹¦ÄÜÂë
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                /* No more function handlers registered. Abort. */
                if( xFuncHandlers[i].ucFunctionCode == 0 )
                {
                    break;
                }
                else if( xFuncHandlers[i].ucFunctionCode == ucFunctionCode )    //¸ù¾Ý±¨ÎÄÖÐµÄ¹¦ÄÜÂë½øÐÐ´¦Àí
                {
                    eException = xFuncHandlers[i].pxHandler( ucMBFrame, &usLength );  //¶Ô½ÓÊÕµ½µÄ±¨ÎÄ½øÐÐ½âÎö
                    break;
                }
            }

            /* If the request was not sent to the broadcast address we
             * return a reply. */
            //µ±ÇëÇó±¨ÎÄÖÐµÄµØÖ·²»ÊÇ¹ã²¥µØÖ·£¬ÐèÒª·µ»ØÏìÓ¦±¨ÎÄ
            if( ucRcvAddress != MB_ADDRESS_BROADCAST )
            {
                if( eException != MB_EX_NONE )    //½ÓÊÕµ½±¨ÎÄÓÐÎó
                {
                    /* An exception occured. Build an error frame. */
                    usLength = 0;  //ÏìÓ¦·¢ËÍÊý¾ÝµÄÊ×µØÖ·Îª´Ó»úµØÖ·
                    ucMBFrame[usLength++] = ( UCHAR )( ucFunctionCode | MB_FUNC_ERROR ); //ÏìÓ¦·¢ËÍÊý¾ÝÖ¡µÄµÚ¶þ¸ö×Ö½Ú£¬¹¦ÄÜÂë×î¸ßÎ»ÖÃ1
                    ucMBFrame[usLength++] = eException;  //ÏìÓ¦·¢ËÍÊý¾ÝµÄµÚÈý¸ö×Ö½ÚÎª´íÎóÂë±êÊ¶
                }
               // if( ( eMBCurrentMode == MB_ASCII ) && MB_ASCII_TIMEOUT_WAIT_BEFORE_SEND_MS )
                //{
                    //vMBPortTimersDelay( MB_ASCII_TIMEOUT_WAIT_BEFORE_SEND_MS );
                //}                
                eStatus = peMBFrameSendCur( ucMBAddress, ucMBFrame, usLength );  //modbus ´Ó»úÏìÓ¦º¯Êý£¬·¢ËÍÏìÓ¦¸øÖ÷»ú
            }
            break;

        case EV_FRAME_SENT:   //·¢ËÍÍê³É
            break;
        }
    }
    return MB_ENOERR;
}
