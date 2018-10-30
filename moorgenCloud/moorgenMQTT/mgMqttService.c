#include "mgMqttService.h"
#include "mgKernel.h"
#include "mgOS.h"
#include "moorgenService.h"
#include "moorgenHttpOTA.h"
#include "HTTPUtils.h"
#include "SocketUtils.h"
#include "StringUtils.h"
#include "mgBase64.h"
#include "product.h"

#define MQTT_ASTIME_FLUSH_TIME  17280

#define MQTT_CLIENT_ID           "MG_MQTT_Client"
#define MQTT_CLIENT_PASSWORD     CONFIG_CHANNEL_CODE
#define MQTT_SERVER              MQTT_DOMAIN_NAME
#define MQTT_SERVER_PORT         MQTT_PORT_SLL

static mg_thread_t mqtt_client;
static mg_thread_t mqtt_recv_thread;
static mqtt_service_t mqtt_service;
static mg_timer_t  mqtt_timer_task;

static void  mqtt_disconnect_time_set(int disconnectTime);

static void  mqtt_status_set(mqtt_status_t mqtt_status);

char contrmode[40];		//store controlMode   and  deviceType


/*"Host: connectoreu.dooya.com\r\n" */
#define SIMPLE_POST_REQUEST \
    "POST /certCenter/certService/getCert?certType=2&isPlain=1 HTTP/1.1\r\n" \
    "Host: %s\r\n" \
    "\r\n"

static OSStatus onReceivedData( struct _HTTPHeader_t * httpHeader,
                                uint32_t pos,
                                uint8_t *data,
                                size_t len,
                                void * userContext );
static void onClearData( struct _HTTPHeader_t * inHeader, void * inUserContext );
extern char mg_mqtt_server_ssl_cert_str[9216];

typedef struct _http_context_t
{
    char *content;
    uint64_t content_length;
} http_context_t;

int simple_http_get( char* host, char* query );



mqtt_packet_t *mqtt_packet_create(int msgType,int content_size,uint8_t *content_data)
{
    mqtt_packet_t *mqtt_packet=NULL;

    if(content_size>CONTENT_SIZE)
        return NULL;



    mqtt_packet=(mqtt_packet_t *)malloc(sizeof(mqtt_packet_t)+content_size+1);
    if(mqtt_packet!=NULL)
    {
        mqtt_packet->msgType=msgType;
        mqtt_packet->content_size=content_size;
        if(content_size>0)
        {
            memcpy(mqtt_packet->content_data,content_data,content_size);
            mqtt_packet->content_data[content_size]='\0';
        }

    }

    return mqtt_packet;
}

void mqtt_packet_release(mqtt_packet_t *mqtt_packet_p)
{
    if(mqtt_packet_p!=NULL)
    {
        free(mqtt_packet_p);

    }
}


static int mqtt_device_activate_req()
{
    json_object *send_json_object = NULL;
    mqtt_packet_t *mqtt_packet_send_p=NULL;
    MOORGEN_SYS_INFO *sysInfo=moorgen_sys_info_get();

    char *contentData;



    if(sysInfo->is_activate==1)
    {
        mqtt_status_set(mqtt_registered);
        return 0;
    }
    else
        mqtt_status_set(mqtt_registering);

    mqtt_log("mqtt_device_activate_req");


	send_json_object = json_object_new_object();
	require(send_json_object != NULL, ERROR);


	json_object_object_add(send_json_object,REGISTER_TYPE_MAC, json_object_new_string(sysInfo->device_mac));
	json_object_object_add(send_json_object,REGISTER_TYPE_DEVICETYPE, json_object_new_string(sysInfo->moorgen_des.deviceType));
	json_object_object_add(send_json_object,REGISTER_TYPE_CHANNEL_CODE, json_object_new_string(CONFIG_CHANNEL_CODE));

	contentData = json_object_to_json_string(send_json_object);

    mqtt_packet_send_p=mqtt_packet_create(MQTT_MSGTYPE_REGISTER,strlen(contentData),(uint8_t *)contentData);
	require(mqtt_packet_send_p != NULL, ERROR);



	mqtt_send_to_queue(mqtt_packet_send_p);
	json_object_put(send_json_object);

    return 0;

ERROR:
    if(mqtt_packet_send_p!=NULL)
        free(mqtt_packet_send_p);

    if(send_json_object!=NULL)
		json_object_put(send_json_object);

    return -1;

}

static int mqtt_device_activate_rsp(mqtt_packet_t *mqtt_packet_recv_p)
{
    int ret=-1;
	json_object *content_json_ob = NULL;
	json_object *ob = NULL;
    char *strData=NULL;

    mqtt_log("mqtt_device_activate_rsp");

	content_json_ob = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(content_json_ob != NULL, exit);

	ob = json_object_object_get(content_json_ob, MQTT_TYPE_RETCODE);
	require(ob != NULL, exit);
	strData= json_object_get_string(ob);

    mqtt_log("ret:%s",strData);

	if (strcmp(strData, MQTT_RETCODE_SUCESS)!=0)
	{
		goto exit;
	}

	ob = json_object_object_get(content_json_ob, REGISTER_TYPE_DEVICECODE);
	require(ob != NULL, exit);
	strData= json_object_get_string(ob);

    mqtt_log("deviceCode:%s",strData);

	moorgen_deviceCode_set(strData);
    ret=0;

exit:

    if(content_json_ob!=NULL)
 		json_object_put(content_json_ob);


    return ret;

}



static int mqtt_device_login_req()
{
    json_object *send_json_object = NULL;
    mqtt_packet_t *mqtt_packet_send_p=NULL;

    MOORGEN_SYS_INFO *sysInfo=moorgen_sys_info_get();
    char *contentData;



    mqtt_log("mqtt_device_login_req");

    //moorgen_sys_info_update();


	send_json_object = json_object_new_object();
	require(send_json_object != NULL, ERROR);


	json_object_object_add(send_json_object,LOGIN_TYPE_FWVERSION, json_object_new_string(sysInfo->firmware));
	//json_object_object_add(send_json_object,LOGIN_TYPE_SERIALNO, json_object_new_string(sysInfo->moorgen_des.serialNo));
	json_object_object_add(send_json_object,LOGIN_TYPE_INTRANET, json_object_new_string(sysInfo->localIp));
	json_object_object_add(send_json_object,LOGIN_TYPE_SSID, json_object_new_string(sysInfo->ssid));

	contentData = json_object_to_json_string(send_json_object);

    mqtt_packet_send_p=mqtt_packet_create(MQTT_MSGTYPE_LOGIN,strlen(contentData),(uint8_t *)contentData);
	require(mqtt_packet_send_p != NULL, ERROR);


	mqtt_send_to_queue(mqtt_packet_send_p);
	json_object_put(send_json_object);

    return 0;

ERROR:
    if(mqtt_packet_send_p!=NULL)
        free(mqtt_packet_send_p);

    if(send_json_object!=NULL)
		json_object_put(send_json_object);

    return -1;

}

static int mqtt_device_login_rsp(mqtt_packet_t *mqtt_packet_recv_p)
{
    int ret=-1;
	json_object *content_json_ob = NULL;
	json_object *ob = NULL;
    char *strData=NULL;
    MOORGEN_SYS_INFO *sysInfo=moorgen_sys_info_get();


    mqtt_log("mqtt_device_login_rsp");

	content_json_ob = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(content_json_ob != NULL, exit);

	ob = json_object_object_get(content_json_ob, MQTT_TYPE_RETCODE);
	require(ob != NULL, exit);
	strData= json_object_get_string(ob);
	if (strcmp(strData, MQTT_RETCODE_SUCESS)!=0)
	{
		goto exit;
	}

	ob = json_object_object_get(content_json_ob, LOGIN_TYPE_ACCESSTOKEN);
	require(ob != NULL, exit);
	strData= json_object_get_string(ob);
    strncpy(sysInfo->access_token,strData,37);


	ob = json_object_object_get(content_json_ob, LOGIN_TYPE_REFRESHTOKEN);
	require(ob != NULL, exit);
	strData= json_object_get_string(ob);
    strncpy(sysInfo->refresh_token,strData,37);

    ret=0;
exit:

    if(content_json_ob!=NULL)
 		json_object_put(content_json_ob);


    return ret;

}




void mqtt_command_result_error_msg(const error_code_type_t errorCode, const char *slave_mac, const char *slave_type,const char *fw_version)
{

    json_object *msg_json_object = NULL;
    char *contentData;

	msg_json_object = json_object_new_object();
	if(msg_json_object==NULL) return;
	json_object_object_add(msg_json_object,ERR_CODE_TYPE, json_object_new_int(errorCode));
	if(slave_mac!=NULL)
    	json_object_object_add(msg_json_object,SLAVE_MAV_TYPE,json_object_new_string(slave_mac));

    if(slave_type!=NULL)
    	json_object_object_add(msg_json_object,SLAVE_DEVICE_TYPE, json_object_new_string(slave_type));

    if(fw_version!=NULL)
    	json_object_object_add(msg_json_object,SLAVE_FIRWARE_VER, json_object_new_string(fw_version));

	contentData = json_object_to_json_string(msg_json_object);

	strncpy(mqtt_service.mqtt_command_result.deviceMsg,contentData,strlen(contentData));
	json_object_put(msg_json_object);

}

int  mqtt_command_result(char *actionType,	char *serverMsg)
{
    if(mqtt_service.mqtt_command_result.lock)
        return -1;
    mqtt_service.mqtt_command_result.lock=1;


    strncpy(mqtt_service.mqtt_command_result.actionType,actionType,8);

    if(serverMsg!=NULL)
        strncpy(mqtt_service.mqtt_command_result.serverMsg,serverMsg,68);
    else
        strncpy(mqtt_service.mqtt_command_result.serverMsg,"",68);

    memset(mqtt_service.mqtt_command_result.deviceMsg,0,sizeof(mqtt_service.mqtt_command_result.deviceMsg));

    return 0;
}


void mqtt_command_result_req(int result)
{
    json_object *send_json_object = NULL;
    mqtt_packet_t *mqtt_packet_send_p=NULL;
    char *contentData;


    mqtt_log("mqtt_command_result_req");

    if(mqtt_service.mqtt_command_result.lock!=1) return;

    mqtt_service.mqtt_command_result.actionResult=result;


    mqtt_service.mqtt_command_result.lock=0;
	send_json_object = json_object_new_object();
	require(send_json_object != NULL, ERROR);

	json_object_object_add(send_json_object,COMMAND_RESULT_TYPE_ACTION,json_object_new_string(mqtt_service.mqtt_command_result.actionType));
	json_object_object_add(send_json_object,COMMAND_RESULT_TYPE_RESULT, json_object_new_int(mqtt_service.mqtt_command_result.actionResult));
	json_object_object_add(send_json_object,COMMAND_RESULT_TYPE_SERVERMSG, json_object_new_string(mqtt_service.mqtt_command_result.serverMsg));
	json_object_object_add(send_json_object,COMMAND_RESULT_TYPE_DEVICEMSG, json_object_new_string(mqtt_service.mqtt_command_result.deviceMsg));

	contentData = json_object_to_json_string(send_json_object);

    mqtt_packet_send_p=mqtt_packet_create(MQTT_MSGTYPE_COMMAND_RESULT,strlen(contentData),(uint8_t *)contentData);
	require(mqtt_packet_send_p != NULL, ERROR);



	mqtt_send_to_queue(mqtt_packet_send_p);
	json_object_put(send_json_object);

    return;

ERROR:
    if(mqtt_packet_send_p!=NULL)
        free(mqtt_packet_send_p);

    if(send_json_object!=NULL)
		json_object_put(send_json_object);

    return ;


}




static void mqtt_bind_user_req()
{
    json_object *send_json_object = NULL;
    mqtt_packet_t *mqtt_packet_send_p=NULL;
    char *contentData;
    MOORGEN_SYS_INFO *sysInfo=moorgen_sys_info_get();

    if(sysInfo->is_bind_user!=1)
    {
        mqtt_status_set(mqtt_bound);
        return;
    }
    mqtt_log("mqtt_bind_user_req");


    mqtt_status_set(mqtt_binding);

    sysInfo->is_bind_user=0;

	send_json_object = json_object_new_object();
	require(send_json_object != NULL, ERROR);

	json_object_object_add(send_json_object,BIND_USER_TYPE_USER_ACCESSTOKEN,json_object_new_string(sysInfo->user_access_tokern) );
	json_object_object_add(send_json_object,BIND_USER_TYPE_TIMEZONE,json_object_new_string(sysInfo->timezone) );

	contentData = json_object_to_json_string(send_json_object);

    mqtt_packet_send_p=mqtt_packet_create(MQTT_MSGTYPE_BIND_USER,strlen(contentData),(uint8_t *)contentData);
	require(mqtt_packet_send_p != NULL, ERROR);


	mqtt_send_to_queue(mqtt_packet_send_p);
	json_object_put(send_json_object);

    return;

ERROR:
    if(mqtt_packet_send_p!=NULL)
        free(mqtt_packet_send_p);

    if(send_json_object!=NULL)
		json_object_put(send_json_object);

    return ;
}



static int mqtt_bind_user_rsp(mqtt_packet_t *mqtt_packet_recv_p)
{
    int ret=-1;
	json_object *content_json_ob = NULL;
	json_object *ob = NULL;
    char *strData=NULL;


    mqtt_log("mqtt_bind_user_rsp");

	content_json_ob = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(content_json_ob != NULL, exit);

	ob = json_object_object_get(content_json_ob, BIND_USER_KEYT);
	require(ob != NULL, exit);
	strData= json_object_get_string(ob);

	moorgen_is_bind_reset();

    mqtt_log("userCode:%s",strData);


    moorgen_userkey_set(strData);


    ret=0;
exit:

    if(content_json_ob!=NULL)
 		json_object_put(content_json_ob);


    return ret;
}









static void mqtt_check_time_req()
{
    mqtt_packet_t *mqtt_packet_send_p=NULL;

    mqtt_log("mqtt_check_time_req");


    mqtt_packet_send_p=mqtt_packet_create(MQTT_MSGTYPE_GET_SYSTEM_TIME,0,NULL);
	require(mqtt_packet_send_p != NULL, ERROR);

	mqtt_send_to_queue(mqtt_packet_send_p);

    return;

ERROR:
    if(mqtt_packet_send_p!=NULL)
        free(mqtt_packet_send_p);
}


static void mqtt_check_time_rsp(mqtt_packet_t *mqtt_packet_recv_p)
{
	json_object *content_json_ob = NULL;
	json_object *ob = NULL;
    char *strData=NULL;
    MG_TIME_DATA mgTime;

    mqtt_log("mqtt_check_time_rsp");

	content_json_ob = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(content_json_ob != NULL, exit);

	ob = json_object_object_get(content_json_ob, SYSTEM_TIME_TYPE_TYPE_TIME);
	require(ob != NULL, exit);
	strData= json_object_get_string(ob);

    mqtt_log("time:%s",strData);

    time_to_mgTime(strData,&mgTime);

    mg_rtc_set_time(&mgTime);


exit:

    if(content_json_ob!=NULL)
 		json_object_put(content_json_ob);


}

static void mqtt_get_user_code_req()
{

    mqtt_packet_t *mqtt_packet_send_p=NULL;

    mqtt_log("mqtt_check_time_req");


    mqtt_packet_send_p=mqtt_packet_create(MQTT_MSGTYPE_GET_USER_CODE,0,NULL);
	require(mqtt_packet_send_p != NULL, ERROR);

	mqtt_send_to_queue(mqtt_packet_send_p);

    return;

ERROR:
    if(mqtt_packet_send_p!=NULL)
        free(mqtt_packet_send_p);

}


static void mqtt_get_user_code_rsp(mqtt_packet_t *mqtt_packet_recv_p)
{

	json_object *content_json_ob = NULL;
	json_object *ob = NULL;
    char *strData=NULL;


    mqtt_log("mqtt_get_user_code_rsp");

	content_json_ob = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(content_json_ob != NULL, exit);

	ob = json_object_object_get(content_json_ob, BIND_USER_KEYT);
	require(ob != NULL, exit);
	strData= json_object_get_string(ob);
    moorgen_userkey_set(strData);

exit:

    if(content_json_ob!=NULL)
 		json_object_put(content_json_ob);



}



void mqtt_rate_push_report(char *mac, char *deviceType ,uint8_t value )
{

     json_object *send_json_object = NULL;
    mqtt_packet_t *mqtt_packet_send_p=NULL;
    char *contentData;


    mqtt_log("mqtt_rate_push_report");
    if(NULL==mac || NULL==deviceType)
        return ;


	send_json_object = json_object_new_object();
	require(send_json_object != NULL, ERROR);

	json_object_object_add(send_json_object,OTA_UPDATE_TYPE_MAC,json_object_new_string(mac));
	json_object_object_add(send_json_object,OTA_UPDATE_TYPE_DEVICETYPE, json_object_new_string(deviceType));
	json_object_object_add(send_json_object,OTA_UPDATE_TYPE_RATE, json_object_new_int(0));
	json_object_object_add(send_json_object,OTA_UPDATE_TYPE_Value, json_object_new_int(value));

	contentData = json_object_to_json_string(send_json_object);

    mqtt_packet_send_p=mqtt_packet_create(MQTT_MSGTYPE_RATE_PUSH_REPORT,strlen(contentData),(uint8_t *)contentData);
	require(mqtt_packet_send_p != NULL, ERROR);



	mqtt_send_to_queue(mqtt_packet_send_p);
	json_object_put(send_json_object);

    return;

ERROR:
    if(mqtt_packet_send_p!=NULL)
        free(mqtt_packet_send_p);

    if(send_json_object!=NULL)
		json_object_put(send_json_object);

    return ;
}




//服务器下发
static void mqtt_req_reboot()
{

    mqtt_log("mqtt_req_reboot");

    ;//mg_os_reboot();
}

static void mqtt_req_relogin()
{
    mqtt_status_set(mqtt_deleteing);

}

static void mqtt_req_reset()
{
    mqtt_log("mqtt_req_reboot");

}

static void mqtt_req_slave_report()
{

    mqtt_log("mqtt_req_slave_report");
    mqtt_slave_report_req();
}




static void mqtt_req_update(mqtt_packet_t *mqtt_packet_recv_p)
{
	json_object *content_json_ob = NULL;
	json_object *ob = NULL;
    char *url;
    char *md5;
    char *serverMsg=NULL;
    int err=3;



	content_json_ob = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(content_json_ob != NULL, exit);


	ob = json_object_object_get(content_json_ob, OTA_UPDATE_TYPE_MAC);
	if(ob!=NULL)
	{
    	if(strcmp(dev_master_ob_p->devMac,json_object_get_string(ob))!=0)
    	    goto exit;
	}
	ob = json_object_object_get(content_json_ob, OTA_UPDATE_TYPE_URL);
	require(ob != NULL, exit);
	url=json_object_get_string(ob);

	ob = json_object_object_get(content_json_ob, OTA_UPDATE_TYPE_MD5);
	require(ob != NULL, exit);
	md5=json_object_get_string(ob);

    ob = json_object_object_get(content_json_ob, OTA_UPDATE_TYPE_SERVERMSG);
	if(ob!=NULL)
    	serverMsg= json_object_get_string(ob);

    while(err--)
    {
        if(mqtt_command_result(UPDATE_FIRMWARE,serverMsg)>=0)
            break;
        mg_rtos_thread_msleep(100);
    }

    moorgen_ota_server_start(url,md5,dev_master_ob_p->devMac,moorgen_dev_type_get());

exit:

    if(content_json_ob!=NULL)
 		json_object_put(content_json_ob);


    return ;
}

void mqtt_ota_update_rsp(int result,char *version)
{
    mqtt_log("mqtt_ota_update_rsp");

    mqtt_command_result_error_msg(result,dev_master_ob_p->devMac,dev_master_ob_p->devType,version);


    if(result==0)
        mqtt_command_result_req(1);
    else
       mqtt_command_result_req(0);

}



//ms
static void mqtt_req_disconnect(mqtt_packet_t *mqtt_packet_recv_p)
{
	json_object *content_json_ob = NULL;
	json_object *ob = NULL;
    int disconnectTime=0;

	content_json_ob = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(content_json_ob != NULL, exit);
	ob = json_object_object_get(content_json_ob, SYSTEM_TIME_TYPE_TYPE_TIME);
	if(ob!=NULL)
    	disconnectTime= json_object_get_int(ob);

    mqtt_disconnect_time_set(disconnectTime);
    mqtt_status_set(mqtt_deleteing);

exit:

    if(content_json_ob!=NULL)
 		json_object_put(content_json_ob);


    return ;
}




static void mqtt_req_astronomical_time_notify()
{
    mqtt_log("mqtt_req_astronomical_time_notify");

}


static void mqtt_req_unbind()
{

    mqtt_log("mqtt_req_unbind");
}


static void mqtt_req_clear_all_scene()
{
    mqtt_log("mqtt_req_clear_all_scene");

}

static void mqtt_req_clear_all_timer()
{
    mqtt_log("mqtt_req_clear_all_timer");

}



void mqtt_send_to_queue( mqtt_packet_t *mqtt_packet_send_p)
{
    mqtt_packet_t *mqtt_packet_recv_p=NULL;
    OSStatus err = kNoErr;

    if(mqtt_service.mqtt_status<mqtt_connected || mqtt_service.mqtt_status==mqtt_deleteing)
    {
        mqtt_packet_release(mqtt_packet_send_p);
		mqtt_packet_send_p = NULL;
		return;
    }

/*
	if(mg_rtos_is_queue_full(&(mqtt_service.mqtt_msg_send_queue) == true)
	{
         mg_rtos_pop_from_queue( &mqtt_service.mqtt_msg_recv_queue, &mqtt_packet_recv_p,0);
         if(mqtt_packet_recv_p!=NULL)
            free(mqtt_packet_recv_p);

	}
*/

	err = mg_rtos_push_to_queue(&(mqtt_service.mqtt_msg_send_queue), &mqtt_packet_send_p, 500);
	if(err != kNoErr)
	{

        mqtt_packet_release(mqtt_packet_send_p);
		mqtt_packet_send_p = NULL;
		mqtt_log("push req to mqtt_msg_send_queue error");
	}


}



static void mqtt_rsp_packet(mqtt_packet_t *mqtt_packet_recv_p)
{
    switch(mqtt_packet_recv_p->msgType)
    {
    case MQTT_MSGTYPE_ACTIVITE:
        if(mqtt_device_activate_rsp(mqtt_packet_recv_p)<0)
            mqtt_status_set(mqtt_deleteing);
        else
            mqtt_status_set(mqtt_registered);
        break;
    case MQTT_MSGTYPE_LOGIN:
        if(mqtt_device_login_rsp(mqtt_packet_recv_p)<0)
            mqtt_status_set(mqtt_deleteing);
        else
            mqtt_status_set(mqtt_logined);
        break;

    case MQTT_MSGTYPE_SLAVE_REPORT:
        break;
    case MQTT_MSGTYPE_DEVICE_ONLINE:
    break;
    case MQTT_MSGTYPE_BIND_USER:
    /*
        if(mqtt_bind_user_rsp(mqtt_packet_recv_p)<0)
            mqtt_status_set(mqtt_deleteing);
        else
            mqtt_status_set(mqtt_bound);
        */
        mqtt_bind_user_rsp(mqtt_packet_recv_p);
        mqtt_status_set(mqtt_bound);
        break;
    case MQTT_MSGTYPE_GET_SINGLE_SCENE:
        mqtt_get_single_scene_rsp(mqtt_packet_recv_p);
        break;
    case MQTT_MSGTYPE_GET_SINGLE_TIMER:
        mqtt_get_single_timer_rsp(mqtt_packet_recv_p);
        break;
    case MQTT_MSGTYPE_GET_SYSTEM_TIME:
        mqtt_check_time_rsp(mqtt_packet_recv_p);
        break;
    case MQTT_MSGTYPE_GET_SCENES_CODE:
        mqtt_get_scene_codes_rsp(mqtt_packet_recv_p);
        mqtt_get_timers_codes_req();
        break;
    case MQTT_MSGTYPE_GET_TIMERS_CODE:
        mqtt_get_timers_codes_rsp(mqtt_packet_recv_p);
        mqtt_status_set(mqtt_running);
        break;
    case MQTT_MSGTYPE_GET_USER_CODE:
        mqtt_get_user_code_rsp(mqtt_packet_recv_p);
        break;
    case MQTT_MSGTYPE_GET_ASTRONOMICAL_TIME:				//设置日落,日出时间
        mqtt_get_astronomical_time_rsp(mqtt_packet_recv_p);
        break;
    default:
    break;
    }
}

static void mqtt_req_packet(mqtt_packet_t *mqtt_packet_recv_p)
{
	switch(mqtt_packet_recv_p->msgType)
    {
    case MQTT_MSGTYPE_REBOOT:
        mqtt_req_reboot();
        break;
    case MQTT_MSGTYPE_RELOGIN:
        mqtt_status_set(mqtt_deleteing);
        break;

    case MQTT_MSGTYPE_RESET:

        break;
    case MQTT_MSGTYPE_DEVICE_REPORT_SUBSCRIBE:
        mqtt_slave_report_req();
        break;
    case MQTT_MSGTYPE_START_COMMISSION:
        mqtt_req_start_commission(mqtt_packet_recv_p);
        break;
   case MQTT_MSGTYPE_DELETE_SLAVE:
        mqtt_req_delete_slave(mqtt_packet_recv_p);
        break;
    case MQTT_MSGTYPE_UPDATE:						//OTA更新固件
        mqtt_req_update(mqtt_packet_recv_p);
        break;
    case MQTT_MSGTYPE_UPDATE_TIMER:
        mqtt_req_reget_time(mqtt_packet_recv_p);
        break;
    case MQTT_MSGTYPE_DISCONNECT:
        mqtt_req_disconnect(mqtt_packet_recv_p);
        break;
    case MQTT_MSGTYPE_DEVICE_CONTROL:
        mqtt_req_device_control(mqtt_packet_recv_p);
        break;
    case MQTT_MSGTYPE_UPDATE_SCENE:
        mqtt_req_reget_scene(mqtt_packet_recv_p);
        break;
    case MQTT_MSGTYPE_SCENE_CONTROL:
        mqtt_req_scene_control(mqtt_packet_recv_p);
        break;
    case MQTT_MSGTYPE_DEL_SCENE:
        mqtt_req_scene_del(mqtt_packet_recv_p);
        break;
    case MQTT_MSGTYPE_DEL_TIMER:
        mqtt_req_timer_del(mqtt_packet_recv_p);
        break;
    case MQTT_MSGTYPE_REGET_USER_CODE:
        mqtt_get_user_code_req();
        break;
     case MQTT_MSGTYPE_UNBIND:
        mqtt_req_unbind();
        break;
    default:
    break;
    }
}

static void mqtt_recv_message_thread( mg_thread_arg_t arg )
{
    OSStatus err = kNoErr;
    mqtt_packet_t *mqtt_packet_recv_p=NULL;

//	mqtt_log("mqtt_recv_message_thread 1\r\n");

    while(1)
    {
	mqtt_log("******doing recv thread********");
        err=mg_rtos_pop_from_queue( &mqtt_service.mqtt_msg_recv_queue, &mqtt_packet_recv_p,MG_WAIT_FOREVER);
//		mqtt_log("mqtt_recv_message_thread 2\r\n");

        if(err!=kNoErr)
            continue;

        mqtt_log("msgType:%d",mqtt_packet_recv_p->msgType);
        if(mqtt_packet_recv_p->content_size!=0)
            mqtt_log("recv:%s",mqtt_packet_recv_p->content_data);

	if(strstr((char *)mqtt_packet_recv_p->content_data,"controlMode\":") != 0)
	{
		strncpy(contrmode,(char *)mqtt_packet_recv_p->content_data,sizeof(contrmode));		//copy  the info what   contronMode  and  deviceType
	}
        if(mqtt_packet_recv_p->msgType < MQTT_MSGTYPE_SUBSCRIBE_START)	//接收到的消息类型:      需回复
        {
            mqtt_rsp_packet(mqtt_packet_recv_p);
        }
        else															//接收到的消息类型:      请求
        {
            mqtt_req_packet(mqtt_packet_recv_p);
        }

        mqtt_packet_release(mqtt_packet_recv_p);
        mqtt_packet_recv_p=NULL;

    }
}



int mqtt_status_get()
{
    return mqtt_service.mqtt_status;

}

static void  mqtt_status_set(mqtt_status_t mqtt_status)
{
    if(mqtt_status!=mqtt_unconnect  &&  mqtt_service.mqtt_status>=mqtt_status)
        return;

    mqtt_service.mqtt_status=mqtt_status;

    switch(mqtt_service.mqtt_status)
    {
    case mqtt_connecting:
        break;
    case mqtt_connected:
        mqtt_service.timeOut=0;
        mqtt_service.keepaliveTime=0;
        mqtt_service.keepaliveTimeout=0;

        mqtt_device_activate_req();
        moorgen_service_led_set(LED_MQTT_CONNECTED);
        break;
    case mqtt_registered:
        mqtt_device_login_req();
        break;
    case mqtt_logined:
        mqtt_bind_user_req();
        break;
    case mqtt_bound :
        timer_mqtt_exe_set();
        mqtt_check_time_req();
        mqtt_slave_report_req();
        mqtt_device_all_online_req();
        mqtt_get_scene_codes_req();
        break;
    case mqtt_running:
        moorgen_is_local_server_set(0);
        mqtt_device_alldp_report();

        break;

    case mqtt_unconnect:
        moorgen_service_led_set(LED_MQTT_DISCONNECTED);

        timer_mqtt_exe_reset();
        moorgen_is_local_server_set(1);

        break;
    default:
    break;
    }


}


static void  mqtt_disconnect_time_set(int disconnectTime)
{

    mqtt_log("mqtt_disconnect_time_set:%d",disconnectTime);

    mqtt_service.diconnect_time=disconnectTime;


}

static OSStatus mqtt_client_release()
{
    OSStatus err = kNoErr;
    mqtt_packet_t *mqtt_packet_send_p=NULL;

    mqtt_status_set(mqtt_unconnect);
    if ( mqtt_service.mqtt_C.isconnected ) MQTTDisconnect(&mqtt_service.mqtt_C);

    mqtt_service.mqtt_N.disconnect(&mqtt_service.mqtt_N);  // close connection

    if ( MQTT_SUCCESS != MQTTClientDeinit( &mqtt_service.mqtt_C ) ) {
        mqtt_log("MQTTClientDeinit failed!");
        err = kDeletedErr;
    }
    while ( mg_rtos_is_queue_empty( &(mqtt_service.mqtt_msg_send_queue) ) == false )
    {
        // get msg from send queue
        err=mg_rtos_pop_from_queue( &mqtt_service.mqtt_msg_send_queue, &mqtt_packet_send_p, 0 );
        if(err!=kNoErr)
            continue;


        mqtt_packet_release(mqtt_packet_send_p);
        mqtt_packet_send_p = NULL;


    }


    return err;
}


static char  *mqtt_add_msgID( )
{
    static uint32_t msgID_count=0;
    static char msgIDstr[MSGID_SIZE];

    if(msgID_count>0xefffffff)
        msgID_count=1;

	sprintf(msgIDstr, "%ld", ++msgID_count);


    return msgIDstr;
}


static OSStatus mqtt_message_send(mqtt_packet_t *mqtt_packet_send_p)
{
    OSStatus err = kGeneralErr;
    MQTTMessage publishData = MQTTMessage_publishData_initializer;
    int data_len;
    uint8_t *data;
    int ret = 0;
    char *deviceCode=moorgen_deviceCode_get();
    char *msgID=mqtt_add_msgID();

	tlv_box_t *tlv_mqtt_box = tlv_box_create();

    if(tlv_mqtt_box==NULL)
		goto exit;

    mqtt_log("send msgType:%d",mqtt_packet_send_p->msgType);


    if(mqtt_packet_send_p->content_size!=0)
        mqtt_log("send:%s",mqtt_packet_send_p->content_data);



	tlv_box_put_int(tlv_mqtt_box,  TAG_MSGTYPE,  mqtt_packet_send_p->msgType);
	if (strlen(msgID) != 0)
		tlv_box_put_string(tlv_mqtt_box, TAG_MSGID, (char *)msgID);

    if(mqtt_packet_send_p->msgType!=MQTT_MSGTYPE_ACTIVITE)
    {
    	if (strlen(deviceCode) != 0)
    		tlv_box_put_bytes(tlv_mqtt_box, TAG_DEVICECODE, (unsigned char *)deviceCode, strlen(deviceCode));
    }
	if (mqtt_packet_send_p->content_size!= 0)
		tlv_box_put_bytes(tlv_mqtt_box, TAG_CONTENT, (unsigned char *)mqtt_packet_send_p->content_data, mqtt_packet_send_p->content_size);

	if (tlv_box_serialize(tlv_mqtt_box) != 0)
	{
		mqtt_log("box serialize failed !\n");
		goto exit;
	}

    data=tlv_box_get_buffer(tlv_mqtt_box);
    data_len=tlv_box_get_size(tlv_mqtt_box);


    // upload data qos0
    publishData.qos = QOS0;
    publishData.retained = 0;
    publishData.payload = (void*) data;
    publishData.payloadlen = data_len;

    ret = MQTTPublish(&mqtt_service.mqtt_C, MQTT_CLIENT_PUB_TOPIC, &publishData );

    if ( MQTT_SUCCESS == ret ) {
        err = kNoErr;
    } else if ( MQTT_SOCKET_ERR == ret ) {
        err = kConnectionErr;
    } else {
        err = kUnknownErr;
    }

exit:

    if (NULL != tlv_mqtt_box)
	{
		tlv_box_destroy(tlv_mqtt_box);
	}

    return err;

}



// msg received from mqtt server with callback
static void mqtt_message_callback( MessageData* md )
{
    OSStatus err = kUnknownErr;
    MQTTMessage* message = md->message;
    mqtt_packet_t *mqtt_packet_recv_p=NULL;
	tlv_box_t *parsedBox = NULL;
	int length ;
	int msgType;

	uint8_t *dataPtr=NULL;

    if(1==moorgen_is_ota_get())		//正在进行OTA升级固件
        return;


	parsedBox = tlv_box_parse(message->payload, message->payloadlen);
	if(NULL==parsedBox)
	{
		mqtt_log("tlv_box_parse err!");

		goto TLV_ERROR;
    }

	if (tlv_box_get_int(parsedBox, TAG_MSGTYPE, &msgType)<0)
	{
		mqtt_log("%s: get msgType err!", __func__);
		goto TLV_ERROR;
	}



    if(tlv_box_get_bytes_ptr(parsedBox, TAG_CONTENT, &dataPtr, &length) != 0 )
	{
		mqtt_log("TLV Get content is NULL\n");

	}

   	mqtt_packet_recv_p=mqtt_packet_create(msgType,length,(uint8_t *)dataPtr);
	require(mqtt_packet_recv_p != NULL, TLV_ERROR);


	mqtt_log("*******mqtt_packet_recv_p : msgType: %d, %s***********",mqtt_packet_recv_p->msgType,mqtt_packet_recv_p->content_data);
	if(mqtt_packet_recv_p->msgType == MQTT_MSGTYPE_DEVICE_CONTROL)
	{
		if(strstr((char *)mqtt_packet_recv_p->content_data,"\"operation\":2") != 0)
		{
			mqtt_log("do pause");
			curtainPauseSet();
		}
	}
	err = mg_rtos_push_to_queue(&(mqtt_service.mqtt_msg_recv_queue), &mqtt_packet_recv_p, 500);
	if(err != kNoErr)
	{
		mqtt_packet_release(mqtt_packet_recv_p);
		mqtt_packet_recv_p = NULL;
		mqtt_log("push req to mqtt_msg_recv_queue error");
	}



TLV_ERROR:


	if (NULL != parsedBox)
	{
		tlv_box_destroy(parsedBox);
		parsedBox = NULL;
	}

	return ;

}

//mqtt 客户端线程
static void mqtt_client_thread( mg_thread_arg_t arg )
{
    OSStatus err = kUnknownErr;
    int rc = -1;
    fd_set readfds;
    mqtt_packet_t *mqtt_packet_send_p=NULL;
    struct timeval t = { 0, MQTT_YIELD_TIME*1000 };
    MOORGEN_SYS_INFO *moorgen_sys_info=moorgen_sys_info_get();
    char http_req[100];
    Certificate_config_t *acquire_certifi = NULL;
 //   int keepTime=0;

    char mqtt_sub_topic[20];	//订阅主题
    char username[MAX_MAC_SIZE + MAX_DEVICE_TYPE_SIZE];	//用户名: mac_deviceType

    MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;


    strncpy(mqtt_sub_topic,moorgen_sys_info->device_mac,20);	//将设备MAC地址作为 主题订阅
    sprintf(username, "%s-%s", moorgen_sys_info->device_mac, moorgen_sys_info->moorgen_des.deviceType);
    mqtt_log("username is %s", username);

    mqtt_log("domainName is %s", moorgen_sys_info->moorgen_des.domainName);
    if(strchr(moorgen_sys_info->moorgen_des.domainName,'.') == NULL)		//检查域名是否错误
    {
        mqtt_log("domainName error");
        mg_rtos_delete_thread( NULL );
        return;
      // mg_rtos_thread_sleep(5);
    }

 //   mqtt_log("domainName is %s", moorgen_sys_info->moorgen_des.domainName);


MQTT_start:


    mqtt_log("MQTT client started...");

    memset( &mqtt_service.mqtt_C, 0, sizeof(Client) );
    memset( &mqtt_service.mqtt_N, 0, sizeof(Network) );


    if(mqtt_service.diconnect_time>0)
    {
        mqtt_log("diconnect_time:%d",mqtt_service.diconnect_time);
        mg_rtos_thread_sleep(mqtt_service.diconnect_time);
        mqtt_service.diconnect_time=0;
    }

    while(mg_wifi_status_get()!=MG_WIFI_STA_CONNECTED)
    {
        mqtt_log("wifi down!");
        mg_rtos_thread_sleep(3);

    }

    mqtt_status_set(mqtt_unconnect);	//mqtt 运行时间清零  ， moorgen_sys_info.is_local_server 置1 


    while( 1 ){

 //  mqtt_log("MQTT_SERVER:%s",MQTT_SERVER);
//   mqtt_log("MQTT_SERVER_PORT:%d",MQTT_SERVER_PORT);
       mqtt_status_set(mqtt_connecting);						//mqtt_service.mqtt_status = mqtt_connecting

//   rc = MQTTNewNetwork( &mqtt_service.mqtt_N, MQTT_SERVER, MQTT_SERVER_PORT);
        rc = MQTTNewNetwork( &mqtt_service.mqtt_N, moorgen_sys_info->moorgen_des.domainName, MQTT_SERVER_PORT);
        if( rc == MQTT_SUCCESS ) break;
	acquire_certifi = Certificate_ReadFlash();
//	mqtt_log("read from flash : %s",acquire_certifi->certificate);
	memset(mg_mqtt_server_ssl_cert_str,0x0,sizeof(mg_mqtt_server_ssl_cert_str));
	strncpy(mg_mqtt_server_ssl_cert_str, acquire_certifi->certificate, strlen(acquire_certifi->certificate));
	rc = MQTTNewNetwork( &mqtt_service.mqtt_N, moorgen_sys_info->moorgen_des.domainName, MQTT_SERVER_PORT);
        if( rc == MQTT_SUCCESS ) break;
	mqtt_log("****download from http***********");
	memset(mg_mqtt_server_ssl_cert_str,0x0,sizeof(mg_mqtt_server_ssl_cert_str));
	sprintf(http_req,SIMPLE_POST_REQUEST,moorgen_sys_info->moorgen_des.domainName);
	if(simple_http_get(moorgen_sys_info->moorgen_des.domainName, http_req ) == kNoErr)	
		continue;
	mqtt_log("ERROR: MQTT network connection err=%d, reconnect after 5s...", rc);
    	mqtt_log("free mem:%d", mg_freemem_get());
	mqtt_status_set(mqtt_unconnect);
	mg_rtos_thread_sleep( 5 );
    }

	mqtt_log("MQTT network connection success!");



    /* 2. init mqtt client */
    //c.heartbeat_retry_max = 2;
    rc = MQTTClientInit( &mqtt_service.mqtt_C, &mqtt_service.mqtt_N, MQTT_CMD_TIMEOUT );
    require_noerr_string(rc, MQTT_reconnect, "ERROR: MQTT client init err.");

    mqtt_log("MQTT client init success!");

    /* 3. create mqtt client connection */
    connectData.willFlag = 0;
    connectData.MQTTVersion = 4;  // 3: 3.1, 4: v3.1.1
    connectData.clientID.cstring = MQTT_CLIENT_ID;
    connectData.username.cstring = username;
    connectData.password.cstring = MQTT_CLIENT_PASSWORD;
    connectData.keepAliveInterval = MQTT_CLIENT_KEEPALIVE;
    connectData.cleansession = 1;

    rc = MQTTConnect( &mqtt_service.mqtt_C, &connectData );
    require_noerr_string(rc, MQTT_reconnect, "ERROR: MQTT client connect err.");

    mqtt_log("MQTT client connect success!");

    /* 4. mqtt client subscribe */
    rc = MQTTSubscribe( &mqtt_service.mqtt_C, mqtt_sub_topic, QOS0, mqtt_message_callback );
    require_noerr_string(rc, MQTT_reconnect, "ERROR: MQTT client subscribe err.");

    mqtt_log("MQTT client subscribe success! recv_topic=[%s].", mqtt_sub_topic);
    mqtt_status_set(mqtt_connected);

    /* 5. client loop for recv msg && keepalive */
    while ( 1 )
    {
    	if (mqtt_service.mqtt_status== mqtt_deleteing)
    	{
    		goto MQTT_reconnect;
    	}

        FD_ZERO( &readfds );
        FD_SET( mqtt_service.mqtt_C.ipstack->my_socket, &readfds );

    	if(select(mqtt_service.mqtt_C.ipstack->my_socket + 1, &readfds, NULL, NULL, &t)>0)
    	{

            /* recv msg from server */
            if ( FD_ISSET( mqtt_service.mqtt_C.ipstack->my_socket, &readfds ) )
            {

                rc = MQTTYield( &mqtt_service.mqtt_C, 500 );
                require_noerr( rc, MQTT_reconnect );
                mqtt_service.keepaliveTime=0;
                mqtt_service.keepaliveTimeout =0;
            }

        }

	 /* recv msg from user worker thread to be sent to server */
        while ( mg_rtos_is_queue_empty( &(mqtt_service.mqtt_msg_send_queue) ) == false )
        {
            // get msg from send queue
            err=mg_rtos_pop_from_queue( &mqtt_service.mqtt_msg_send_queue, &mqtt_packet_send_p, 0 );
            if(err!=kNoErr)
                continue;
            // send message to server

            if(mqtt_message_send(mqtt_packet_send_p)!=kNoErr)
                 mqtt_status_set(mqtt_deleteing);

    		mqtt_packet_release(mqtt_packet_send_p);
            mqtt_packet_send_p = NULL;
            mqtt_service.keepaliveTime=0;
            mqtt_service.keepaliveTimeout =0;
        }

        mqtt_service.keepaliveTime ++;
        if(mqtt_service.keepaliveTime>50)
        {
           mqtt_service.keepaliveTime=0;
           rc = keepalive( &mqtt_service.mqtt_C );

           if(rc<0)
           {
                mqtt_service.keepaliveTimeout ++;
                mqtt_log("ERROR: keepalive err");

           }
           else
                mqtt_service.keepaliveTimeout =0;

           if(mqtt_service.keepaliveTimeout>=3)
           {
                mqtt_log("keepaliveTimeout");
                goto MQTT_reconnect;
           }
       }


    }

MQTT_reconnect:
    mqtt_log("Disconnect MQTT client, and reconnect after 5s, reason: mqtt_rc = %d, err = %d", rc, err );
    mqtt_client_release( );
    mg_rtos_thread_sleep( 5 );
    goto MQTT_start;

    mg_rtos_delete_thread( NULL );
}



static void mqttMinuteRunTask(void *arg)
{
    static uint32_t cnt=MQTT_ASTIME_FLUSH_TIME;
    static uint8_t asTimeflag=0;
    static uint8_t check_time=0;

    if(cnt>=MQTT_ASTIME_FLUSH_TIME)	//17280, 每6天更新一次天文时间
    {

        cnt=0;
        asTimeflag=1;
    }

    cnt ++;

    if(mqtt_service.mqtt_status > mqtt_connecting &&  mqtt_service.mqtt_status < mqtt_running)	//连接ing   -> 连接成功
    {
        mqtt_service.timeOut ++;
        if(mqtt_service.timeOut > 4)
            mqtt_status_set(mqtt_deleteing);
    }


    if(asTimeflag && mqtt_service.mqtt_status ==mqtt_running)
    {
        mqtt_get_astronomical_time_req();
        asTimeflag=0;

    }

    if(check_time>1440)		//1440*30s/60s/60minute = 12hour,也就是每12个小时，从MQTT服务器获取并校准系统时间
    {
        mqtt_check_time_req();
        check_time=0;

    }
    check_time ++;

}



int mg_mqtt_service_init()
{
	OSStatus err = kGeneralErr;
	int mqtt_thread_stack_size = 0x2000;
    memset(&mqtt_service,0,sizeof(mqtt_service));



	err = mg_rtos_init_queue( &(mqtt_service.mqtt_msg_send_queue), "mqtt_msg_send_queue", sizeof(p_mqtt_packet_t_t), MAX_MQTT_SEND_QUEUE_SIZE);
	require_noerr_action( err, exit, mqtt_log("ERROR: create mqtt msg send queue err=%d.", err) );


	err = mg_rtos_init_queue( &(mqtt_service.mqtt_msg_recv_queue), "mqtt_msg_recv_queue", sizeof(p_mqtt_packet_t_t), MAX_MQTT_RECV_QUEUE_SIZE);
	require_noerr_action( err, exit, mqtt_log("ERROR: create mqtt msg recv queue err=%d.", err) );


	err = mg_rtos_create_thread(&mqtt_client, MG_APPLICATION_PRIORITY, "mqtt_client", mqtt_client_thread, mqtt_thread_stack_size, (uint32_t) NULL );
	require_noerr_string( err, exit, "ERROR: Unable to start the mqtt client thread." );

	mqtt_log("at once create mqtt thread\r\n");
	err = mg_rtos_create_thread(&mqtt_recv_thread, MG_APPLICATION_PRIORITY, "mqtt_recv", mqtt_recv_message_thread, mqtt_thread_stack_size, (uint32_t) NULL );
	require_noerr_string( err, exit, "ERROR: Unable to start the mqtt client thread." );
	mqtt_log("create mqtt thread no  err .\r\n");

	mg_init_timer(&mqtt_timer_task,  30000, mqttMinuteRunTask, NULL);
	mg_start_timer(&mqtt_timer_task);

    return 0;

exit:
    return -1;

}



int simple_http_get( char* host, char* query )
{
    OSStatus err;
    int client_fd = -1;
    fd_set readfds;
    char ipstr[16];
    struct sockaddr_in addr;
    HTTPHeader_t *httpHeader = NULL;
    http_context_t context = { NULL, 0 };
    struct hostent* hostent_content = NULL;
    char **pptr = NULL;
    struct in_addr in_addr;
     json_object *content_json_object = NULL;
     json_object *obj = NULL;
	char *NewCertificate = NULL;
	char *p1,*p2;
//	char Context[60]={'\0'};
	char *Context = NULL;

    hostent_content = gethostbyname( host );
    require_action_quiet( hostent_content != NULL, exit, err = kNotFoundErr);
    pptr=hostent_content->h_addr_list;
    in_addr.s_addr = *(uint32_t *)(*pptr);
    strcpy( ipstr, inet_ntoa(in_addr));
	mqtt_log("HTTP server address: %s, host ip: %s", host, ipstr);
//    http_client_log("HTTP server address: %s, host ip: %s", host, ipstr);

    /*HTTPHeaderCreateWithCallback set some callback functions */
    httpHeader = HTTPHeaderCreateWithCallback( 9216, onReceivedData, onClearData, &context );  //9K 
    require_action( httpHeader, exit, err = kNoMemoryErr );

    client_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    addr.sin_family = AF_INET;
    addr.sin_addr = in_addr;
    addr.sin_port = htons(8091);				//port
    err = connect( client_fd, (struct sockaddr *)&addr, sizeof(addr) );
    require_noerr_string( err, exit, "connect http server failed" );

    /* Send HTTP Request */
    send( client_fd, query, strlen( query ), 0 );

    FD_ZERO( &readfds );
    FD_SET( client_fd, &readfds );

    mqtt_log("simple_http_get doing");
    select( client_fd + 1, &readfds, NULL, NULL, NULL );
    if ( FD_ISSET( client_fd, &readfds ) )
    {
        /*parse header*/
        err = SocketReadHTTPHeader( client_fd, httpHeader );
        switch ( err )
        {
   		case kNoErr:
              	PrintHTTPHeader( httpHeader );
                	err = SocketReadHTTPBody( client_fd, httpHeader );/*get body data*/
               	 require_noerr( err, exit );
               	 /*get data and print*/
//			mqtt_log( "Content Data: %s", context.content );
			content_json_object  = json_tokener_parse(context.content );
			obj = json_object_object_get(content_json_object,"data");
			if(obj != NULL)
				NewCertificate = json_object_get_string(obj);
			mqtt_log("NewCertificate %s",NewCertificate);

			mqtt_log("before length %d",strlen(NewCertificate));
			Context = (char *)malloc(strlen(NewCertificate));
			if(Context == NULL)
			{
				mqtt_log("MALLOC ERR");
			}
			memset(Context,0x0,strlen(Context));
			p2 = NewCertificate;
			while((p1=strchr(NewCertificate,'\n') ) !=  NULL)
			{
				strncat(Context,NewCertificate,p1-p2);
				strcpy(NewCertificate,p1+1);	//更新新的NewCertificate
				p2 = NewCertificate;  		//指向新的数据区
			}
			strcat(Context,NewCertificate);	//将剩余不包含"\r\n"的数据拷贝
			mqtt_log("after length %d",strlen(Context));
			mgbase64_decode(Context,mg_mqtt_server_ssl_cert_str );	//NewCertificate
			Certificate_WriteFlash(mg_mqtt_server_ssl_cert_str);		//write into flash 
			free(Context);
              	  break;
    		case EWOULDBLOCK:
     		case kNoSpaceErr:
       	case kConnectionErr:
		default:
//                http_client_log("ERROR: HTTP Header parse error: %d", err);
			mqtt_log("ERROR: HTTP Header parse error: %d", err);
               	 break;
        }
    }

    exit:
//    http_client_log( "Exit: Client exit with err = %d, fd: %d", err, client_fd );
	mqtt_log( "Exit: Client exit with err = %d, fd: %d", err, client_fd );
	SocketClose( &client_fd );
	HTTPHeaderDestory( &httpHeader );
	return err;
}


/*one request may receive multi reply*/
static OSStatus onReceivedData( struct _HTTPHeader_t * inHeader, uint32_t inPos, uint8_t * inData,
                                size_t inLen, void * inUserContext )
{
    OSStatus err = kNoErr;
    http_context_t *context = inUserContext;
    if ( inHeader->chunkedData == false )
    { //Extra data with a content length value
        if ( inPos == 0 && context->content == NULL )
        {
            context->content = calloc( inHeader->contentLength + 1, sizeof(uint8_t) );
            require_action( context->content, exit, err = kNoMemoryErr );
            context->content_length = inHeader->contentLength;

        }
        memcpy( context->content + inPos, inData, inLen );
    } else
    { //extra data use a chunked data protocol
//        http_client_log("This is a chunked data, %d", inLen);
        if ( inPos == 0 )
        {
            context->content = calloc( inHeader->contentLength + 1, sizeof(uint8_t) );
            require_action( context->content, exit, err = kNoMemoryErr );
            context->content_length = inHeader->contentLength;
        } else
        {
            context->content_length += inLen;
            context->content = realloc( context->content, context->content_length + 1 );
            require_action( context->content, exit, err = kNoMemoryErr );
        }
        memcpy( context->content + inPos, inData, inLen );
    }

    exit:
    return err;
}

/* Called when HTTPHeaderClear is called */
static void onClearData( struct _HTTPHeader_t * inHeader, void * inUserContext )
{
    UNUSED_PARAMETER( inHeader );
    http_context_t *context = inUserContext;
    if ( context->content )
    {
        free( context->content );
        context->content = NULL;
    }
}




