/**
******************************************************************************
* @file    uartProtocol.h
* @author  Nicky Zhang
* @version V1.0.0
* @date    23-Oct-2017
* @brief   
******************************************************************************
*
*  Copyright (c) 2015 DOOYA Inc.
*
******************************************************************************
*/
#ifndef _UART_PROTOCOL_H_
#define _UART_PROTOCOL_H_


#include "mico.h"

#define dev_log(M, ...)                    custom_log("dev", M, ##__VA_ARGS__)

typedef unsigned char   UINT8;
typedef unsigned short  UINT16;
typedef unsigned long   UINT32;
typedef unsigned long long int UINT64;
typedef signed   long   INT32;
typedef signed   char   INT8;
typedef signed   short  INT16;
typedef unsigned char   BOOLEAN;
typedef UINT32          UINTPTR;
typedef UINT32          TIME_STAMP;

#define UART_RECV_BUFFER_LEN   100
#define UART_SEND_BUFFER_LEN   12

#define UART_PROT_MIN_LEN       7


#define  LIST_SEND_MAX           50
#define  LIST_SEND_ID_INVALID    0xFF
#define  LIST_SEND_ID_MAX        (LIST_SEND_MAX+1)
#define  LIST_SEND_ID_BIT        (LIST_SEND_ID_MAX/32 + 1)
#define  LIST_DATA_LEN           UART_SEND_BUFFER_LEN

#define UART_PROT_BEGIN_OFFSET  0x00
#define UART_PROT_ID_OFFSET     0x01
#define UART_PROT_CMD_OFFSET    0x03
#define UART_PROT_ADDR_OFFSET   0x04
#define UART_PROT_DATA_OFFSET   0x05


typedef  struct tagLIST_SEND_DATA {
    UINT8   prev;
	UINT8   next;
    UINT16  datalen;
    UINT8   data[LIST_DATA_LEN];
}LIST_SEND_DATA;


typedef  struct tagLIST_SEND_DMG {
    LIST_SEND_DATA  data[LIST_SEND_ID_MAX];
	UINT32          bitSet[LIST_SEND_ID_BIT];
	volatile UINT16 inUse;
    UINT16          total;
}LIST_SEND_DMG;


typedef enum{
    WIFI_EASYLINK       =0x01,
    WIFI_CONFIG_SUC     =0x02,
    WIFI_AP_CONNECTING  =0x03,
    WIFI_AP_UP          =0x04,
    WIFI_AP_DOWN        =0x05

}WIFI_STATUS;



typedef enum{

    READ_CMD    =   0x01,
    WRITE_CMD   =   0x02,
    CTRL_CMD    =   0x03,
    UPDATE_CMD  =   0x04,
	MAX_CMD
}UART_CMD;

typedef enum{

    OPEN_ADDR   =   0x01,
    CLOSE_ADDR  =   0x02,
    PAUSE_ADDR  =   0x03,
    POSITION_ADDR=   0x04,
    SET_ADDR    =   0x05,
	MAX_CTRL_ADDR

}CTRL_ADDR;


typedef enum{

    ID_L_ADDR       =   0x00,
    ID_H_ADDR       =   0x01,
    CURRENT_ADDR    =   0x02,
    DIRECTION_ADDR  =   0x03,
    MANUAL_ADDR     =   0x04,
    STATUS_ADDR     =   0x05,
    ANGLE_ADDR      =   0x06,
    ANGLE_PARA_ADDR =   0x07,
    RES__ADDR       =   0x08,
    TRIP_ADDR       =   0x09,
    TYPE_ADDR       =   0xf0,
    MAX_REG_ADDR

}REG_ADDR;



typedef struct tagUART_PROT_DATA{
    
    UINT8   cmd;
    UINT8   fun_addr;
    UINT8   data[1];

}UART_PROT_DATA;




typedef struct tagIS_UART_OB{
    
   volatile UINT8 motorRun;
    UINT8   res;
    UINT8   uRecvLen;
    UINT8   uSendLen;
    UINT8   uRecvBuffer[UART_RECV_BUFFER_LEN];
    UINT8   uSendBuffer[UART_SEND_BUFFER_LEN];


}IS_UART_OB;


typedef struct tagDEVICE_STATUS{
    UINT8 type;
    UINT8 targetPosition;
    UINT8 currentPosition;
    UINT8 currentState;		//new add that  journey status
    UINT8 direction;
    UINT8 manual;
    UINT8 status;
    UINT8 angle;
    UINT8 angleDirection;
    UINT8 anglePara;
    UINT8 trip;
    UINT8 app;
    UINT8 res;
}DEVICE_STATUS;

void devUartStatusWrite(UINT8 addr , UINT8 offset,UINT8 *data);
void devUartStatusRead(UINT8 addr , UINT8 offset);
void devUartCtrl(UINT8 addr , UINT8 *data , UINT8 datalen);
void devUartTypeWrite(UINT8 type);
void devUartTypeRead(void);
void devUartWifiTestSuc();

void curatin_device_report();

void devUartInit();

#endif
