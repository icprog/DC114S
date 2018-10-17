#ifndef _MOORGEN_CONFIG_SERVER_H_
#define _MOORGEN_CONFIG_SERVER_H_

#include "mgOS.h"
#include "mgKernel.h"

#define CONFIG_UDP_SERVER_PORT	(32000)

#define UDP_CONFIG_SIZE    512


#define CONFIG_REQ	    "CONFIG_REQ"
#define CONFIG_ACK	    "CONFIG_ACK"
#define WIFI_SET		"WIFI_SET"
#define WIFI_ACK	    "WIFI_ACK"
#define INFO_REQ        "INFO_REQ"
#define INFO_ACK        "INFO_ACK"

#define WIFI_CONFIG_MSG_TYPE                    "msgType"
#define WIFI_CONFIG_WIFI_TYPE                   "WIFI"
#define WIFI_CONFIG_SSID_TYPE                   "ssid"
#define WIFI_CONFIG_PASSWORD_TYPE               "password"
#define WIFI_CONFIG_USER_TOKEN_TYPE	            "userAccessToken"
#define WIFI_CONFIG_TIME_ZONE_TYPE              "timeZone"
#define WIFI_CONFIG_DEVICE_TYPE                 "device"
#define WIFI_CONFIG_TYPE_TYPE                   "type"
#define WIFI_CONFIG_DEVICETYPE_TYPE             "deviceType"
#define WIFI_CONFIG_MAC_TYPE                    "mac"
#define WIFI_CONFIG_VERSION_TYPE                "version"
#define WIFI_CONFIG_RET_TYPE                    "ret"
#define WIFI_CONFIG_DOMAINNAME_TYPE             "domainName"


typedef struct
{
    char     msgType[24];
    char     ssid[32];
    char     password[64];
    char     devType[MG_DEVICE_TYPE_SIZE];
    char     userAccessToken[MAX_USER_TOKEN_SIZE];
    char     timeZone[MAX_TIMEZONE_SIZE];
    char     domainName[MAX_DOMAIN_NAME_SZIE];

}config_data_t;

typedef struct
{
    int  socket;
    int  flag;
    int  addrLen;
    int  recvLen;
    int  sendLen;
    char   recvBuf[UDP_CONFIG_SIZE];
    char   sendBuf[UDP_CONFIG_SIZE];
    struct  sockaddr_in recvAddr;
}udp_config_t;



#endif

