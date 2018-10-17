#include "moorgenLocalServer.h"
#include "moorgenService.h"
#include "json.h"



#define udp_log(M, ...) 	custom_log("UDP", M, ##__VA_ARGS__)



static mg_thread_t mg_local_client;
static udp_server_t  udp_server;
static local_data_t  local_data;

static void local_client_send(const char *data, int len)
{
    
    memcpy(udp_server.buf,data,len);

    if((len%16)>0)
        len= (len/16+1)*16;

    if(len>UDP_DATA_SIZE) return;
    udp_server.bufLen=len;
    
    mgAesEncrypt((uint8_t *)udp_server.buf,(uint8_t *)udp_server.aes_buf,moorgen_userkey_get(),udp_server.bufLen);
    
    sendto(udp_server.socket,udp_server.aes_buf,udp_server.bufLen,0,(struct sockaddr *)&udp_server.recvAddr,sizeof(struct  sockaddr_in));
}

static void local_device_ctrl_req( )
{
    DEV_SUB_OBJECT *dev_sub_ob_p;
    json_object *ob = NULL;
    uint8_t  devDP[MG_DEV_DP_MAX];

	if(strlen(local_data.devMac)==0) return;
	if(strlen(local_data.devType)==0) return;
	if(strlen(local_data.deviceData)==0) return;

    
	ob = json_tokener_parse(local_data.deviceData);
	if(NULL==ob) return;
	

    if(strncmp(dev_master_ob_p->devMac,local_data.devMac,MG_DEV_MAC_MAX)==0)
    {
        if(strncmp(dev_master_ob_p->devType,local_data.devType,MG_DEVICE_TYPE_SIZE)!=0)
            goto exit;
         
        dev_master_ob_p->dev_ops.dev_op_dp_to_struct(dev_master_ob_p->devType,(void *)devDP,(void *)ob);
        dev_master_ob_p->dev_ops.dev_op_dp_ctrl(DEVICE_MASTER,(void *)dev_master_ob_p,(void *)devDP);
    }
    else 
    {
        if(dev_master_ob_p->hub_flag!=1)
             goto exit;

        dev_sub_ob_p=device_sub_object_search(local_data.devMac);
        if(dev_sub_ob_p!=NULL)
        {
            if(strncmp(dev_sub_ob_p->devType,local_data.devType,MG_DEVICE_TYPE_SIZE)!=0)
                 goto exit;
        
            dev_master_ob_p->dev_ops.dev_op_dp_to_struct(dev_sub_ob_p->devType,(void *)devDP,(void *)ob);
            dev_master_ob_p->dev_ops.dev_op_dp_ctrl(DEVICE_SUB,(void *)dev_sub_ob_p,(void *)devDP);

        }

    }

	
exit:
    if(ob!=NULL)
    	json_object_put(ob);

    return;
}


static void local_device_ctrl_rsp( )
{
    json_object *send_json_object = NULL;
	json_object *data_point_obj =NULL;
    DEV_SUB_OBJECT *dev_sub_ob_p;

    const char *contentData;
    int contentLen;

	send_json_object = json_object_new_object();
	require(send_json_object != NULL, ERROR);


	json_object_object_add(send_json_object,MSG_TYPE, json_object_new_string(DEVICE_CONTROL_MSGTYE_ACK));
	json_object_object_add(send_json_object,SN_TYPE, json_object_new_int(local_data.sn));
	json_object_object_add(send_json_object,DEVICEMAC_TYPE, json_object_new_string(local_data.devMac));
	json_object_object_add(send_json_object,DEVICETYPE_TYPE, json_object_new_string(local_data.devType));

    data_point_obj=json_object_new_object();
	require(data_point_obj != NULL, ERROR);

    if(strncmp(dev_master_ob_p->devMac,local_data.devMac,MG_DEV_MAC_MAX)==0)
    {
        if(strncmp(dev_master_ob_p->devType,local_data.devType,MG_DEVICE_TYPE_SIZE)!=0)
            goto ERROR;
         
        dev_master_ob_p->dev_ops.dev_op_dp_to_json(dev_master_ob_p->devType,(void *)dev_master_ob_p->devDP,(void *)data_point_obj);

    }
    else 
    {
        if(dev_master_ob_p->hub_flag!=1)
             goto ERROR;

        dev_sub_ob_p=device_sub_object_search(local_data.devMac);
        if(dev_sub_ob_p!=NULL)
        {
            if(strncmp(dev_sub_ob_p->devType,local_data.devType,MG_DEVICE_TYPE_SIZE)!=0)
                 goto ERROR;
        
            dev_master_ob_p->dev_ops.dev_op_dp_to_json(dev_sub_ob_p->devType,(void *)dev_sub_ob_p->devDP,(void *)data_point_obj);

        }

    }
	json_object_object_add(send_json_object, DEVICEDATA_TYPE, data_point_obj);

	
	contentData = json_object_to_json_string(send_json_object);
	if((contentLen=strlen(contentData))>UDP_DATA_SIZE)
	    goto ERROR;

    local_client_send(contentData,contentLen);
	    
ERROR:


    if(send_json_object!=NULL)
		json_object_put(send_json_object);

   

}


static void local_device_data_rsp( )
{
    
    json_object *send_json_object = NULL;
	json_object *data_point_obj =NULL;
    DEV_SUB_OBJECT *dev_sub_ob_p;

    const char *contentData;
    int contentLen;

	send_json_object = json_object_new_object();
	require(send_json_object != NULL, ERROR);


	json_object_object_add(send_json_object,MSG_TYPE, json_object_new_string(DEVICE_STATUS_MSGTYE_ACK));
	json_object_object_add(send_json_object,SN_TYPE, json_object_new_int(local_data.sn));
	json_object_object_add(send_json_object,DEVICEMAC_TYPE, json_object_new_string(local_data.devMac));
	json_object_object_add(send_json_object,DEVICETYPE_TYPE, json_object_new_string(local_data.devType));

    data_point_obj=json_object_new_object();
	require(send_json_object != NULL, ERROR);

    if(strncmp(dev_master_ob_p->devMac,local_data.devMac,MG_DEV_MAC_MAX)==0)
    {
        if(strncmp(dev_master_ob_p->devType,local_data.devType,MG_DEVICE_TYPE_SIZE)!=0)
            goto ERROR;
         
        dev_master_ob_p->dev_ops.dev_op_dp_to_json(dev_master_ob_p->devType,(void *)dev_master_ob_p->devDP,(void *)data_point_obj);

    }
    else 
    {
        if(dev_master_ob_p->hub_flag!=1)
             goto ERROR;

        dev_sub_ob_p=device_sub_object_search(local_data.devMac);
        if(dev_sub_ob_p!=NULL)
        {
            if(strncmp(dev_sub_ob_p->devType,local_data.devType,MG_DEVICE_TYPE_SIZE)!=0)
                 goto ERROR;
        
            dev_master_ob_p->dev_ops.dev_op_dp_to_json(dev_sub_ob_p->devType,(void *)dev_sub_ob_p->devDP,(void *)data_point_obj);

        }

    }
	json_object_object_add(send_json_object, DEVICEDATA_TYPE, data_point_obj);

	
	contentData = json_object_to_json_string(send_json_object);
	if((contentLen=strlen(contentData))>UDP_DATA_SIZE)
	    goto ERROR;

    local_client_send(contentData,contentLen);
	    
ERROR:


    if(send_json_object!=NULL)
		json_object_put(send_json_object);
}

static void local_scene_req( )
{
	if(strlen(local_data.sceneCode)==0) return;
    
    scene_object_exe(local_data.sceneCode);
}

static void local_time_set()
{
    MG_TIME_DATA mgTime;
    time_to_mgTime(local_data.timeStr,&mgTime);
    mg_rtc_set_time(&mgTime);
    astronomical_today_flush(&mgTime);
}


static void local_client_event(char *data)
{
	json_object *request_obj = NULL;
	json_object *ob = NULL;


    memset(&local_data,0,sizeof(local_data));
    
	request_obj = json_tokener_parse(data);
	if(NULL==request_obj) return;

	ob = json_object_object_get(request_obj, MSG_TYPE);
	if(ob!=NULL)
    	strncpy(local_data.msgType,json_object_get_string(ob),sizeof(local_data.msgType));

	ob = json_object_object_get(request_obj, SN_TYPE);
	if(ob!=NULL)
    	local_data.sn= json_object_get_int(ob);

	ob = json_object_object_get(request_obj, DEVICEMAC_TYPE);
	if(ob!=NULL)
    	strncpy(local_data.devMac,json_object_get_string(ob),sizeof(local_data.devMac));

    ob = json_object_object_get(request_obj, DEVICETYPE_TYPE);
	if(ob!=NULL)
    	strncpy(local_data.devType,json_object_get_string(ob),sizeof(local_data.devType));


    ob = json_object_object_get(request_obj, DEVICEDATA_TYPE);
	if(ob!=NULL)
    	strncpy(local_data.deviceData,json_object_get_string(ob),sizeof(local_data.deviceData));

    ob = json_object_object_get(request_obj, SCENECODE_TYPE);
	if(ob!=NULL)
    	strncpy(local_data.sceneCode,json_object_get_string(ob),sizeof(local_data.sceneCode));

    ob = json_object_object_get(request_obj, TIME_TYPE);
	if(ob!=NULL)
    	strncpy(local_data.timeStr,json_object_get_string(ob),sizeof(local_data.timeStr));

	json_object_put(request_obj);
	
	if(!strcmp(local_data.msgType, DEVICE_CONTROL_MSGTYPE))
	{
		local_device_ctrl_req();
		local_device_ctrl_rsp();
	}
	else if(!strcmp(local_data.msgType, DEVICE_STATUS_MSGTYPE))
	{
		local_device_data_rsp();
	}
	else if(!strcmp(local_data.msgType, SCENE_CONTROL_MSGTYPE))
	{
		local_scene_req();
	}
	else if(!strcmp(local_data.msgType, CAL_TIME_MSGTYPE))
	{
		local_time_set();
	}

}



static int local_data_check(char *data)
{


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

static void local_client_recv()
{
    fd_set  fds;

    while(1)
    {
        
        FD_ZERO(&fds);
        FD_SET(udp_server.socket, &fds);

        if(select(udp_server.socket+1, &fds, NULL, NULL, NULL)<=0)
            continue;
        if(FD_ISSET(udp_server.socket, &fds))
        {
            memset(udp_server.buf,0,UDP_DATA_SIZE);
            udp_server.bufLen= recvfrom(udp_server.socket, udp_server.buf, UDP_DATA_SIZE, 0,(struct sockaddr *)&udp_server.recvAddr,(socklen_t *)&udp_server.addrLen);
            if( udp_server.bufLen <0)
                continue;

            if(moorgen_is_local_server_get()!=1)	//不处于local sererr 状态下
                continue;
  
            mgAesDecrypt((uint8_t *)udp_server.buf,(uint8_t *)udp_server.aes_buf,moorgen_userkey_get(),udp_server.bufLen);
            udp_log("%s",udp_server.aes_buf);
           
            if(local_data_check((char *)udp_server.aes_buf)<0)	//解密后的数据buf 中如果不包含'{'  或者 '}'
            {
                udp_log("local_data_check error");
                continue;
            }
            local_client_event(udp_server.aes_buf);

        }
    }
    
}


 void local_client_thread( mg_thread_arg_t arg )
{
    struct sockaddr_in localAddr;
    udp_server.socket=-1;
    int retry_cnt = 3;

    
  //  mg_rtos_thread_sleep(3);
    udp_log("local_client_thread ");

    memset(&localAddr,0,sizeof(struct sockaddr_in));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr= INADDR_ANY;
    localAddr.sin_port= htons(LOCAL_UDP_SERVER_PORT);


    while (retry_cnt--) {
        udp_server.socket= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (udp_server.socket >= 0)
            break;
        /* Wait some time to allow some sockets to get released */
         mg_rtos_thread_sleep(1);
    }

        
    if(udp_server.socket<0)
    {
        udp_log("udp_server socket<0 ");

        goto exit;
    }
    if(bind(udp_server.socket, (struct sockaddr *)&localAddr, sizeof(localAddr))<0)
    {
        close(udp_server.socket);
        udp_log("bind error ");

        goto exit;

    }
       
    udp_server.addrLen=sizeof(struct sockaddr_in);

    local_client_recv();


    
exit:
    
    udp_log("local_client error");

	mg_rtos_delete_thread(NULL );

}


int moorgen_udp_server_start( void )
{
	OSStatus err = kNoErr;

    udp_log("moorgen_udp_server_start ");

	/* Start local server listener thread*/

	err = mg_rtos_create_thread(&mg_local_client, MG_APPLICATION_PRIORITY, "mg_local_client", local_client_thread, 0x1000, (uint32_t) NULL );
	require_noerr_string( err, exit, "ERROR: Unable to start the local client thread." );


    return 0;
 exit:

	return -1;
}



