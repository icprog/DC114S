#ifndef _MOORGEN_LOCAL_SERVER_H_
#define _MOORGEN_LOCAL_SERVER_H_

#include "mgOS.h"
#include "mgKernel.h"

#define LOCAL_UDP_SERVER_PORT	(32100)

#define UDP_DATA_SIZE    1024


#define DEVICE_CONTROL_MSGTYPE	    "DEVICE_CONTROL_REQ"
#define DEVICE_CONTROL_MSGTYE_ACK	"DEVICE_CONTROL_ACK"
#define DEVICE_STATUS_MSGTYPE		"DEVICE_DATA_REQ"
#define DEVICE_STATUS_MSGTYE_ACK	"DEVICE_DATA_ACK"
#define SCENE_CONTROL_MSGTYPE		"SCENE_REQ"
#define CAL_TIME_MSGTYPE	        "CHECK_TIME"

#define MSG_TYPE                    "msgType"
#define SN_TYPE                     "sn"
#define DEVICEMAC_TYPE              "deviceMac"
#define DEVICETYPE_TYPE             "deviceType"
#define DEVICEDATA_TYPE	            "deviceData"
#define SCENECODE_TYPE              "sceneCode"
#define TIME_TYPE                   "time"


typedef struct
{
    char     msgType[24];
    char     devMac[MG_DEV_MAC_MAX];
    char     devType[MG_DEVICE_TYPE_SIZE];
    char     sceneCode[MG_CODE_SIZE];
    char     timeStr[24];
    char     deviceData[200];

    int      sn;
}local_data_t;

typedef struct
{
    int  socket;
    int  addrLen;
    int  bufLen;
    char   buf[UDP_DATA_SIZE];
    char   aes_buf[UDP_DATA_SIZE];

    struct  sockaddr_in recvAddr;
}udp_server_t;

int moorgen_udp_server_start( void );



#endif
