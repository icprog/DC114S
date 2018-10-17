/**
******************************************************************************
* @file     
* @author  Nicky Zhang
* @version V1.0.0
* @date    
* @brief   
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2018 MOORGEN Inc.

******************************************************************************
*/

#ifndef __MOORGEN_HTTP_OTA_H
#define __MOORGEN_HTTP_OTA_H

#define HTTP_OTA_PORT 80

typedef enum {

	MG_OTA_UPDATE_SUCCESS = 0,			// 成功
	MG_OTA_CHECK_FAILURE = 14,             //检查OTA失败
	MG_OTA_NO_NEW_VERSION = 15,            //检查发现无新版本
	MG_OTA_UPDATE_FAILURE = 16,             //更新失败
	MG_OTA_MD5_FAIL =17,				// MD5校验失败
	MG_OTA_NO_MEM =18,				// OTA空间不足
	MG_OTA_NO_FILE=19,				// 未找到OTA镜像

} MG_OTA_EVENT_TYPE;


typedef struct {
    char     url[200];
    char     httpIp[20];
    int      httpPort;
    uint8_t  md5_recv[16];
    char     filename[100];
    int      result;
    uint32_t filelen;
    uint32_t flashaddr; // the flash address of this file
    uint8_t  *filebuf;
    char     mac[20];
    char     deviceType[16];

} http_file_info_t;


void moorgen_ota_server_start(char *url,char *md5Str,char *mac ,char *deviceType);


#endif
