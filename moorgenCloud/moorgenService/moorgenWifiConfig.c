#include "moorgenService.h"
#include "mgOS.h"
#include "moorgenWifiConfig.h"
#include "json.h"




#define config_log(M, ...) //custom_log("config_log", M, ##__VA_ARGS__)


static mg_thread_t   mg_config_thread=NULL;
static udp_config_t  udp_config;
static config_data_t config_data;
int is_mg_config_server_established=0;

static void config_client_send()
{
       config_log("send:%s",udp_config.sendBuf);

    sendto(udp_config.socket,udp_config.sendBuf,udp_config.sendLen,0,(struct sockaddr *)&udp_config.recvAddr,sizeof(struct  sockaddr_in));
}

void moorgen_wifi_config_rsp(int result)
{

    json_object *send_json_object = NULL;
	json_object *sub_obj =NULL;

    MOORGEN_SYS_INFO *moorgen_sys_info=moorgen_sys_info_get();
    
    const char *contentData;
    int contentLen;

	send_json_object = json_object_new_object();		//创建一个新的Json对象
	require(send_json_object != NULL, ERROR);
    

    if(strcmp(config_data.msgType, CONFIG_REQ)==0)		//如果消息类型为 : CONFIG_REQ
    {
        if(1==result)									//并且设备类型一致，ssid,  password,   timeZone 参数不为空
        {
            strcpy(moorgen_sys_info->user_access_tokern,config_data.userAccessToken);	//更新moorgen_sys_info中的user_access_tokern
            strcpy(moorgen_sys_info->timezone,config_data.timeZone);					//更新moorgen_sys_info中的timezone
            moorgen_sys_info->is_bind_user=1;	
            moorgen_domain_name_set(config_data.domainName);					//更新domainName，并写入到EEPROM中
        }
    	//更新键值为WIFI_CONFIG_MSG_TYPE的值为CONFIG_ACK
    	json_object_object_add(send_json_object,WIFI_CONFIG_MSG_TYPE, json_object_new_string(CONFIG_ACK));	

    }
    else
    	json_object_object_add(send_json_object,WIFI_CONFIG_MSG_TYPE, json_object_new_string(WIFI_ACK));


    sub_obj=json_object_new_object();
	require(sub_obj != NULL, ERROR);
	//mac , devicetype , firware
	json_object_object_add(sub_obj,WIFI_CONFIG_MAC_TYPE,json_object_new_string(moorgen_sys_info->device_mac));
	json_object_object_add(sub_obj,WIFI_CONFIG_TYPE_TYPE,json_object_new_string(moorgen_sys_info->moorgen_des.deviceType));
	json_object_object_add(sub_obj,WIFI_CONFIG_VERSION_TYPE,json_object_new_string(moorgen_sys_info->firmware));
	
	json_object_object_add(send_json_object, WIFI_CONFIG_DEVICE_TYPE, sub_obj);

	json_object_object_add(send_json_object, WIFI_CONFIG_RET_TYPE, json_object_new_int(result));

	contentData = json_object_to_json_string(send_json_object);	//将json格式的send_json_object 转成字符串格式
	if((contentLen=strlen(contentData))>UDP_CONFIG_SIZE)		//发送长度>512(自己定义的)
	    goto ERROR;

    memcpy(udp_config.sendBuf,contentData,contentLen);		//将要发送的数据 存入udp sendbuff
    udp_config.sendLen=contentLen;

    config_client_send();		//发送，调用socket接口的sendto()函数发送


    
    

    udp_config.flag=0;

    mg_rtos_thread_msleep(500);

	    
ERROR:

    if(send_json_object!=NULL)
		json_object_put(send_json_object);

}

static void moorgen_config_req()
{

    MOORGEN_SYS_INFO *moorgen_sys_info=moorgen_sys_info_get();

    config_log("moorgen_config_req");


    if(strlen(config_data.ssid)==0)
        goto error;
    if(strlen(config_data.devType)==0)
        goto error;
    if(strlen(config_data.userAccessToken)==0)
        goto error;
    if(strlen(config_data.timeZone)==0)
        goto error;

    if(strlen(config_data.domainName)==0)
        goto error;

    if(strcmp(config_data.devType,moorgen_sys_info->moorgen_des.deviceType)!=0)	//设备类型不一致
    {
        moorgen_wifi_config_rsp(-1);
        return;
        
    }
	//至此，以上ssid,  devType,  userAccessToken,  timeZone,  donmainName 满足条件
    udp_config.flag=1;

    moorgen_wifi_config_rsp(1);    


    mg_wifi_config_suc(config_data.ssid,config_data.password);

    return;
error:
    moorgen_wifi_config_rsp(0);    


}

static void moorgen_wifi_set()
{

   MOORGEN_SYS_INFO *moorgen_sys_info=moorgen_sys_info_get();
    config_log("moorgen_wifi_set");

    if(strlen(config_data.ssid)==0)
        return;
    if(strlen(config_data.devType)==0)
        return;

    config_log("moorgen_wifi_set");

    if(strcmp(config_data.devType,moorgen_sys_info->moorgen_des.deviceType)!=0)	//设备类型不一致
    {
        moorgen_wifi_config_rsp(-1);
        return;
        
    }

    udp_config.flag=1;
    moorgen_wifi_config_rsp(1);    

    mg_wifi_config_suc(config_data.ssid,config_data.password);
}


static void moorgen_info_req()
{

    json_object *send_json_object = NULL;
	json_object *sub_obj =NULL;

    MOORGEN_SYS_INFO *moorgen_sys_info=moorgen_sys_info_get();
    
    const char *contentData;
    int contentLen;

	send_json_object = json_object_new_object();
	require(send_json_object != NULL, ERROR);
    


	json_object_object_add(send_json_object,WIFI_CONFIG_MSG_TYPE, json_object_new_string(INFO_ACK));


    sub_obj=json_object_new_object();
	require(sub_obj != NULL, ERROR);

	json_object_object_add(sub_obj,WIFI_CONFIG_MAC_TYPE,json_object_new_string(moorgen_sys_info->device_mac));
	json_object_object_add(sub_obj,WIFI_CONFIG_TYPE_TYPE,json_object_new_string(moorgen_sys_info->moorgen_des.deviceType));
	json_object_object_add(sub_obj,WIFI_CONFIG_VERSION_TYPE,json_object_new_string(moorgen_sys_info->firmware));
	
	json_object_object_add(send_json_object, WIFI_CONFIG_DEVICE_TYPE, sub_obj);


	contentData = json_object_to_json_string(send_json_object);
	if((contentLen=strlen(contentData))>UDP_CONFIG_SIZE)
	    goto ERROR;

    memcpy(udp_config.sendBuf,contentData,contentLen);
    udp_config.sendLen=contentLen;

    config_client_send();

ERROR:

    if(send_json_object!=NULL)
		json_object_put(send_json_object);
}






static void config_client_event(char *data)
{
	json_object *request_obj = NULL;
	json_object *ob = NULL;
	json_object *sub = NULL;


    memset(&config_data,0,sizeof(config_data));
    
	request_obj = json_tokener_parse(data);	//将data字符串构造为一个json对象，默认引用计数为1
	if(NULL==request_obj) return;				//需要在使用完，调用一次json_object_put

	ob = json_object_object_get(request_obj, WIFI_CONFIG_MSG_TYPE);	//获取"msgType"(消息类型)子对象
	if(ob!=NULL)
	{
    	strncpy(config_data.msgType,json_object_get_string(ob),sizeof(config_data.msgType));	//复制到config_data.msgType
	    config_log("msgType:%s",config_data.msgType);

    }


	sub = json_object_object_get(request_obj,  WIFI_CONFIG_WIFI_TYPE);	//"WIFI"子对象，并判断是否存在
	if(sub!=NULL)
	{
	    ob=json_object_object_get(sub,WIFI_CONFIG_SSID_TYPE);						//获取 "ssid"
	    if(ob!=NULL)
	    {
        	strncpy(config_data.ssid,json_object_get_string(ob),sizeof(config_data.ssid));	
		    config_log("ssid:%s",config_data.ssid);

        }


        ob=json_object_object_get(sub,WIFI_CONFIG_PASSWORD_TYPE);					//获取 "password"
	    if(ob!=NULL)
	    {
        	strncpy(config_data.password,json_object_get_string(ob),sizeof(config_data.password));	
		    config_log("password:%s",config_data.password);

        }
	}


    ob=json_object_object_get(request_obj,WIFI_CONFIG_USER_TOKEN_TYPE);		//获取  "userAccessToken"
    if(ob!=NULL)
    {
    	strncpy(config_data.userAccessToken,json_object_get_string(ob),sizeof(config_data.userAccessToken)); 
	    config_log("userAccessToken:%s",config_data.userAccessToken);
    }


    ob=json_object_object_get(request_obj,WIFI_CONFIG_TIME_ZONE_TYPE);		//获取 "timeZone"
    if(ob!=NULL)
    {
    	strncpy(config_data.timeZone,json_object_get_string(ob),sizeof(config_data.timeZone));   
    	config_log("timeZone:%s",config_data.timeZone);
    }	


    ob=json_object_object_get(request_obj,WIFI_CONFIG_DEVICETYPE_TYPE);		//获取"device"
    if(ob!=NULL)
    {
    	strncpy(config_data.devType,json_object_get_string(ob),sizeof(config_data.devType));
    	config_log("devType:%s",config_data.devType);

    }

    ob=json_object_object_get(request_obj,WIFI_CONFIG_DOMAINNAME_TYPE);		//获取"domainName"
    if(ob!=NULL)
    {
    	strncpy(config_data.domainName,json_object_get_string(ob),sizeof(config_data.domainName));
    	config_log("domainName:%s",config_data.domainName);

    }
    

	json_object_put(request_obj);		//释放json内存空间
	
	if(!strcmp(config_data.msgType, CONFIG_REQ))	//  "CONFIG_REQ"
	{
		moorgen_config_req();
	}
	else if(!strcmp(config_data.msgType, WIFI_SET))	//"WIFI_SET"
	{
		moorgen_wifi_set();
	}
	else if(!strcmp(config_data.msgType, INFO_REQ))	//"INFO_REQ"
	{
		moorgen_info_req();
	}


}



static int config_data_check(char *data)
{

    config_log("recv:%s",data);

	if(strchr(data,'{')==NULL )
	{
		return -1;
	}

    if(strchr(data,'}')==NULL )
	{
		return -1;
	}

	return 0;
}

static void config_client_recv()
{
    fd_set  fds;

    while(1)
    {
        
        FD_ZERO(&fds);
        FD_SET(udp_config.socket, &fds);

        if(select(udp_config.socket+1, &fds, NULL, NULL, NULL)<=0)
            continue;
        if(FD_ISSET(udp_config.socket, &fds))
        {
            memset(udp_config.recvBuf,0,UDP_CONFIG_SIZE);
            udp_config.recvLen= recvfrom(udp_config.socket, udp_config.recvBuf, UDP_CONFIG_SIZE, 0,(struct sockaddr *)&udp_config.recvAddr,( socklen_t *)&udp_config.addrLen);
            if( udp_config.recvLen <0)
                continue;
            if(1==udp_config.flag)
                 continue;
                 
            if(config_data_check((char *)udp_config.recvBuf)<0)
            {
                config_log("local_data_check eeror");
                continue;
            }
            config_client_event(udp_config.recvBuf);

        }
    }
    
}



static void config_client_thread( mg_thread_arg_t arg )
{
    struct sockaddr_in localAddr;
    udp_config.socket=-1;
    udp_config.flag=0;
    int retry_cnt = 3;


    
    memset(&localAddr,0,sizeof(struct sockaddr_in));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr= INADDR_ANY;
    localAddr.sin_port= htons(CONFIG_UDP_SERVER_PORT);	//32000
    
    while (retry_cnt--) {
        udp_config.socket= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);	//IPV4  , udp, 协议udp
        if (udp_config.socket >= 0)
            break;
        /* Wait some time to allow some sockets to get released */
         mg_rtos_thread_sleep(1);
    }

    if(udp_config.socket<0)
        goto exit;

    if(bind(udp_config.socket, (struct sockaddr *)&localAddr, sizeof(localAddr))<0)	//bind
    {
        close(udp_config.socket);
        goto exit;

    }
       
    udp_config.addrLen=sizeof(struct sockaddr_in);

    config_client_recv();


    
exit:

    if(udp_config.socket>=0) 
        close(udp_config.socket);

	mg_rtos_delete_thread(NULL );

}


void moorgen_wifi_config_start( void )
{

    
	if( is_mg_config_server_established )
	{
        return;
	}

  	is_mg_config_server_established = 1;

    config_log("moorgen_wifi_config_start");

	/* Start local server listener thread*/

	 mg_rtos_create_thread(&mg_config_thread, MG_APPLICATION_PRIORITY, "mg_config_client", config_client_thread, 0x1000, (uint32_t) NULL );


}

void moorgen_wifi_config_stop( void )
{

    config_log("moorgen_wifi_config_stop");
    
  	is_mg_config_server_established = 0;
    if(mg_config_thread !=NULL)
    {


        if(udp_config.socket>=0) 
            close(udp_config.socket);
     
        mg_rtos_delete_thread( &mg_config_thread );
        mg_config_thread=NULL;
    }

}

