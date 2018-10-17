#include "mgMqttService.h"
#include "mgOS.h"
#include "moorgenService.h"
#include "mgKernel.h"


static TIMER_OBJECT mqtt_timer_ob;
static ASTRONOMICAL_TIME astime;

void mqtt_get_single_timer_req(char *timerCode)
{
    json_object *send_json_object = NULL;
    mqtt_packet_t *mqtt_packet_send_p=NULL;
    char *contentData;

    if(NULL==timerCode)
        return;

    mqtt_log("mqtt_get_single_timer_req");


	send_json_object = json_object_new_object();
	require(send_json_object != NULL, ERROR);

	json_object_object_add(send_json_object,TIMER_TYPE_CODE,json_object_new_string(timerCode) );

	contentData = json_object_to_json_string(send_json_object);

    mqtt_packet_send_p=mqtt_packet_create(MQTT_MSGTYPE_GET_SINGLE_TIMER,strlen(contentData),(uint8_t *)contentData);
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


void mqtt_get_single_timer_rsp(mqtt_packet_t *mqtt_packet_recv_p)
{
    json_object *response_object = NULL;
	json_object *rules_array = NULL;
	json_object *ob = NULL;
	json_object *tmp_obj;
    char *timerCode;
    char *str;
    char *ptr;
    char strTime[20];
    int rules_num;
    int i,bit;
    DEV_SUB_OBJECT *dev_sub_ob_p;
    MG_TIME_DATA curTime;
    TIMER_HEAD *timer_head=NULL;

    mqtt_log("mqtt_get_single_timer_rsp");

	response_object = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(response_object != NULL, exit);



	ob = json_object_object_get(response_object, MQTT_TYPE_RETCODE);
	require(ob != NULL, exit);
	if (strcmp(json_object_get_string(ob), MQTT_RETCODE_SUCESS)!=0)	//retcode"0000" 表示成功，"1111"  : 失败
	{
		goto exit;
	}

    memset(&mqtt_timer_ob,0,sizeof(SCENE_OBJECT));	//SCENE_OBJECT ???
    for(i=0;i<MG_DEVICE_MAX;i++)
        mqtt_timer_ob.item.devItem[i].devID=MG_ID_INVALID;

	ob = json_object_object_get(response_object, TIMER_TYPE_CODE);	//"timerCode"
	require(ob != NULL, exit);
    timerCode= json_object_get_string(ob);

    strncpy(mqtt_timer_ob.head.timerCode,timerCode,MG_CODE_SIZE);		


    //mqtt_log("timerCode:%s",mqtt_timer_ob.head.timerCode);



	ob = json_object_object_get(response_object, TIMER_TYPE_MODIFY_TIME);//"modifyTime"
	require(ob != NULL, exit);
    str= json_object_get_string(ob);
    time_to_mgTime(str,&mqtt_timer_ob.head.modifyTime);


	ob = json_object_object_get(response_object, TIMER_TYPE_REPEAT);		//  "repeat"  : 1
	require(ob != NULL, exit);
    mqtt_timer_ob.head.repeatType= json_object_get_int(ob);

   // mqtt_log("repeatType:%d", mqtt_timer_ob.head.repeatType);


    ob = json_object_object_get(response_object, TIMER_TYPE_DAYS);		// "days" : [7,1,4,5,6]
    if(ob!=NULL)
    {
        rules_num=json_object_array_length(ob);
    //mqtt_log("rules_num:%d", rules_num);

    	for(i=0; i< rules_num; i++)
    	{
    		tmp_obj = json_object_array_get_idx(ob, i);	//根据索引值i，得到数组内容（对象object）
    		if(tmp_obj!=NULL)
    		{
        		bit = json_object_get_int(tmp_obj);		//object -> int

        		mqtt_timer_ob.head.repeatDays |= 0x01<<bit;	//置位相应的bit(7,1,4,5,6)
    		}
    	}

    }
   // mqtt_log("repeatDays:%d", mqtt_timer_ob.head.repeatDays);



	ob = json_object_object_get(response_object, TIMER_TYPE_TIME);	//  "time" : 0  日出， 1 :日落or    "time" : "11:05:00"
	require(ob != NULL, exit);
    str= json_object_get_string(ob);
    if(strlen(str)<2)						//设置日落日出 定时器模式
    {
        mqtt_timer_ob.head.timerType=atoi(str);
        if(REPEAT_NONE==mqtt_timer_ob.head.repeatType)
        {
            mg_rtc_get_time(&curTime);
            mqtt_timer_ob.head.timerTime.year=curTime.year;
            mqtt_timer_ob.head.timerTime.month=curTime.month;
            mqtt_timer_ob.head.timerTime.day=curTime.day;
        }
    }
    else								//设置时间定时器模式
    {
        strncpy(strTime,str,20);
        str=strTime;
        ptr=strchr(str,':');
    	require(ptr != NULL, exit);
    	*ptr='\0';
    	mqtt_timer_ob.head.timerTime.hour=atoi(str);		//acquire   hour  ----> 11

    	str=ptr+1;
    	ptr=strchr(str,':');
    	require(ptr != NULL, exit);
    	*ptr='\0';
    	mqtt_timer_ob.head.timerTime.minute=atoi(str);	//acquire  minute  ----> 05

    	mqtt_timer_ob.head.timerTime.second=atoi(ptr+1);	//acquire  second  ----> 00

        mqtt_timer_ob.head.timerType=TIMER_NORMAL;


        if(REPEAT_NONE==mqtt_timer_ob.head.repeatType)
        {
            mg_rtc_get_time(&curTime);
            mqtt_timer_ob.head.timerTime.year=curTime.year;
            mqtt_timer_ob.head.timerTime.month=curTime.month;
            mqtt_timer_ob.head.timerTime.day=curTime.day;
        }

    }


    //mqtt_log("timerType:%d", mqtt_timer_ob.head.timerType);
    //mqtt_log("timerTime.hour:%d", mqtt_timer_ob.head.timerTime.hour);
    //mqtt_log("timerTime.minute:%d", mqtt_timer_ob.head.timerTime.minute);
    //mqtt_log("timerTime.second:%d", mqtt_timer_ob.head.timerTime.second);


	ob = json_object_object_get(response_object, TIMER_TYPE_STATUS);		// "timerStatus" : 1
	require(ob != NULL, exit);
    mqtt_timer_ob.head.timerStatus=json_object_get_int(ob);
    //mqtt_log("timerStatus:%d", mqtt_timer_ob.head.timerStatus);


    ob = json_object_object_get(response_object, TIMER_TYPE_OFFSET);		//   "timeOffset"
	if(ob!=NULL)
	{
	    mqtt_timer_ob.head.timerOffset=json_object_get_int(ob);
	}




	rules_array = json_object_object_get(response_object, TIMER_TYPE_RULES);	//  "rules" : ["deviceType" ,"mac","targetposition"]
	require(rules_array!=NULL, exit);

    rules_num=json_object_array_length(rules_array);	//acquire  array length : 3
    //mqtt_log("rules_num:%d",rules_num);

    if(rules_num>MG_DEVICE_MAX)
        goto exit;

    for(i=0;i<rules_num;i++)
    {
	tmp_obj = json_object_array_get_idx(rules_array, i);

	ob = json_object_object_get(tmp_obj, SCENE_TYPE_CODE);		// "sceneCode"
	if(ob!=NULL)
	{

		mqtt_timer_ob.head.timerEventType=TIMER_SCENE;				//场景中的定时器类型
		 strncpy(mqtt_timer_ob.item.sceneCode,json_object_get_string(ob),MG_CODE_SIZE);
		 break;
	}
	mqtt_timer_ob.head.timerEventType=TIMER_DEVICE;				//设备定时器 类型
	ob = json_object_object_get(tmp_obj, DEVICE_ONLINE_TYPE_MAC);	// "mac"
	if(ob==NULL) continue;

	if(strncmp(dev_master_ob_p->devMac,json_object_get_string(ob),MG_DEV_MAC_MAX)==0)	//master  
	{
		ob = json_object_object_get(tmp_obj, DEVICE_ONLINE_TYPE_DEVICETYPE);	// "deviceType"
    		if(ob==NULL) continue;

		if(strncmp(dev_master_ob_p->devType,json_object_get_string(ob),MG_DEVICE_TYPE_SIZE)!=0)
                continue;

            mqtt_timer_ob.item.devItem[i].devID=MG_MASTER_ID;		//0xaa
            dev_master_ob_p->dev_ops.dev_op_dp_to_struct(dev_master_ob_p->devType,(void *)mqtt_timer_ob.item.devItem[i].devDP,(void *)tmp_obj);
        }
        else		//hub
        {

            if(dev_master_ob_p->hub_flag!=1)
                continue;

            dev_sub_ob_p=device_sub_object_search(json_object_get_string(ob));	//根据mac 查询(从机sub)设备
            if(NULL==dev_sub_ob_p)
                continue;
            ob = json_object_object_get(tmp_obj, DEVICE_ONLINE_TYPE_DEVICETYPE);
    		if(ob==NULL) continue;
            if(strncmp(dev_sub_ob_p->devType,json_object_get_string(ob),MG_DEVICE_TYPE_SIZE)!=0)
                continue;

            mqtt_timer_ob.item.devItem[i].devID=dev_sub_ob_p->devID;
            dev_master_ob_p->dev_ops.dev_op_dp_to_struct(dev_sub_ob_p->devType,(void *)mqtt_timer_ob.item.devItem[i].devDP,(void *)tmp_obj);

        }


    }


    if((timer_head=timer_object_search(mqtt_timer_ob.head.timerCode))==NULL)	//查找timer_object 是否存在该timerCode 定时器
        timer_object_add(&mqtt_timer_ob);		//如果不存在，则添加
    else										//已存在，修改即可
    {
        mqtt_timer_ob.head.timerID=timer_head->timerID;
        timer_object_mod(&mqtt_timer_ob);
    }
exit:

    if(response_object!=NULL)
 		json_object_put(response_object);

}


void  mqtt_get_timers_codes_req()
{

    uint8_t timerID=MG_ID_INVALID;
    TIMER_OBJECT *timer_ob_p;
    mqtt_packet_t *mqtt_packet_send_p=NULL;

    mqtt_log("mqtt_get_timers_codes_req");


    mqtt_packet_send_p=mqtt_packet_create(MQTT_MSGTYPE_GET_TIMERS_CODE,0,NULL);
	require(mqtt_packet_send_p != NULL, ERROR);

	mqtt_send_to_queue(mqtt_packet_send_p);


    while((timer_ob_p=timer_object_next(timerID))!=NULL)
    {
        timerID=timer_ob_p->head.timerID;
        timer_ob_p->head.flag=0xff;

    }


    return;

ERROR:
    if(mqtt_packet_send_p!=NULL)
        free(mqtt_packet_send_p);



}



int  mqtt_get_timers_codes_rsp(mqtt_packet_t *mqtt_packet_recv_p)
{

	json_object *response_object = NULL;
	json_object *timer_array = NULL;
	json_object *tmp_timer;
	json_object *ob = NULL;
    int timer_num;
	int totalPage=0;
	int currentPage=0;
    int i;
    char *timerCode;
    char *modifyTime;
    MG_TIME_DATA mgTime;
    TIMER_OBJECT *timer_ob_p;
    uint8_t timerID=MG_ID_INVALID;
    int ret=-1;

    mqtt_log("mqtt_get_timers_codes_rsp");

	response_object = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(response_object != NULL, exit);

	ob = json_object_object_get(response_object, TIMER_TYPE_TOTAL_PAGE);
	require(ob != NULL, exit);
    totalPage= json_object_get_int(ob);
    mqtt_log("totalPage:%d",totalPage);

	ob = json_object_object_get(response_object, TIMER_TYPE_TOTAL_CURRENT_PAGE);
	require(ob != NULL, exit);
    currentPage= json_object_get_int(ob);
    mqtt_log("currentPage:%d",currentPage);

	timer_array = json_object_object_get(response_object, TIMER_TYPE_ARRAY);
	require(timer_array!=NULL, exit);

	timer_num = json_object_array_length(timer_array);
    mqtt_log("timer_num:%d",timer_num);

    for(i=0;i<timer_num;i++)
    {
		tmp_timer = json_object_array_get_idx(timer_array, i);

    	ob = json_object_object_get(tmp_timer, TIMER_TYPE_CODE);
    	if(ob==NULL)
    	    continue;
        timerCode= json_object_get_string(ob);

    mqtt_log("timerCode:%s",timerCode);

        ob = json_object_object_get(tmp_timer, TIMER_TYPE_MODIFY_TIME);
    	if(ob==NULL)
    	    continue;
        modifyTime= json_object_get_string(ob);
    mqtt_log("modifyTime:%s",modifyTime);

        if((timer_ob_p=timer_object_search(timerCode))==NULL)
        {
            mqtt_get_single_timer_req(timerCode);
            continue;
        }
    mqtt_log("----------------------");


        timer_ob_p->head.flag=0;

        time_to_mgTime(modifyTime,&mgTime);
        if(timer_ob_p->head.modifyTime.year != mgTime.year ||
           timer_ob_p->head.modifyTime.month != mgTime.month ||
           timer_ob_p->head.modifyTime.day != mgTime.day ||
           timer_ob_p->head.modifyTime.hour != mgTime.hour ||
           timer_ob_p->head.modifyTime.minute != mgTime.minute ||
           timer_ob_p->head.modifyTime.second != mgTime.second )
        {
            mqtt_get_single_timer_req(timerCode);

        }



    }
    ret=0;

    if(currentPage==totalPage)
    {
        while((timer_ob_p=timer_object_next(timerID))!=NULL)
        {
            timerID=timer_ob_p->head.timerID;
            if(0xff==timer_ob_p->head.flag)
                timer_object_del(timer_ob_p->head.timerID);

        }
        ret=1;
    }


exit:

    if(response_object!=NULL)
 		json_object_put(response_object);

    return ret;
}



void mqtt_req_reget_time(mqtt_packet_t *mqtt_packet_recv_p)
{
	json_object *content_json_ob = NULL;
	json_object *ob = NULL;
	char *timerCode=NULL;

	content_json_ob = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(content_json_ob != NULL, exit);
	ob = json_object_object_get(content_json_ob, TIMER_TYPE_CODE);
	require(ob != NULL, exit);
	timerCode= json_object_get_string(ob);

    mqtt_get_single_timer_req(timerCode);

exit:

    if(content_json_ob!=NULL)
 		json_object_put(content_json_ob);


    return ;
}



void mqtt_req_timer_del(mqtt_packet_t *mqtt_packet_recv_p)
{
	json_object *content_json_ob = NULL;
	json_object *ob = NULL;
	char *timerCode=NULL;
    TIMER_HEAD *timer_head_p;

	content_json_ob = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(content_json_ob != NULL, exit);
	ob = json_object_object_get(content_json_ob, TIMER_TYPE_CODE);
	require(ob != NULL, exit);

	timerCode= json_object_get_string(ob);

    timer_head_p=timer_object_search(timerCode);
    if(timer_head_p==NULL)
        goto exit;

    timer_object_del(timer_head_p->timerID);

exit:

    if(content_json_ob!=NULL)
 		json_object_put(content_json_ob);


    return ;
}



void mqtt_clienttime_notify_req(char *timerCode)
{
    json_object *send_json_object = NULL;
    mqtt_packet_t *mqtt_packet_send_p=NULL;
    char *contentData;



    mqtt_log("mqtt_clienttime_notify_req");


	send_json_object = json_object_new_object();
	require(send_json_object != NULL, ERROR);

	json_object_object_add(send_json_object,TIMER_TYPE_CODE,json_object_new_string(timerCode) );

	contentData = json_object_to_json_string(send_json_object);

    mqtt_packet_send_p=mqtt_packet_create(MQTT_MSGTYPE_CLIENT_TIMER_NOTIFY,strlen(contentData),(uint8_t *)contentData);
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


void mqtt_get_astronomical_time_req()
{
    mqtt_packet_t *mqtt_packet_send_p=NULL;

    mqtt_log("mqtt_get_astronomical_time_req");


    mqtt_packet_send_p=mqtt_packet_create(MQTT_MSGTYPE_GET_ASTRONOMICAL_TIME,0,NULL);
	require(mqtt_packet_send_p != NULL, ERROR);

	mqtt_send_to_queue(mqtt_packet_send_p);


    return;

ERROR:
    if(mqtt_packet_send_p!=NULL)
        free(mqtt_packet_send_p);
}


void mqtt_get_astronomical_time_rsp(mqtt_packet_t *mqtt_packet_recv_p)
{

	json_object *content_json_ob = NULL;
	json_object *ob = NULL;
	json_object *days_object = NULL;
	int array_length = 0;
	int i;
	json_object *sub_obj;
	char *str;
	char *ptr;
	char strData[12];
    mqtt_log("mqtt_get_astronomical_time_rsp");

	content_json_ob = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(content_json_ob != NULL, exit);

	days_object = json_object_object_get(content_json_ob,ASTRONOMICAL_TIME_TYPE_DAYS);
	require(days_object, exit);

	array_length = json_object_array_length(days_object);

    mqtt_log("array_length:%d",array_length);



    for(i=0;i<array_length;i++)
    {
		sub_obj = json_object_array_get_idx(days_object, i);
        if(sub_obj==NULL)
            continue;

    	ob = json_object_object_get(sub_obj,ASTRONOMICAL_TIME_TYPE_DAY);
        if(ob==NULL)
            continue;
        str=json_object_get_string(ob);

        strncpy(strData,str,12);
    //mqtt_log("day:%s",strData);
        str=strData;
        ptr=strchr(str,'-');
        if(NULL==ptr)
            continue;
    	*ptr='\0';
    	astime.year=atoi(str);
    //mqtt_log("year:%d",astime.year);


    	str=ptr+1;
   // mqtt_log("str:%s",str);


        ptr=strchr(str,'-');
    	if(NULL==ptr)
            continue;
    	*ptr='\0';
    	astime.month=atoi(str);
        astime.day=atoi(ptr+1);
   // mqtt_log("month:%d",astime.month);
   // mqtt_log("day:%d",astime.day);



    	ob = json_object_object_get(sub_obj,ASTRONOMICAL_TIME_TYPE_SUNRISE);
        if(ob==NULL)
            continue;
        str=json_object_get_string(ob);
    //mqtt_log("sunrise:%s",strData);


        strncpy(strData,str,12);
        str=strData;
        ptr=strchr(str,':');
        if(NULL==ptr)
            continue;
    	*ptr='\0';
    	astime.sunrise_hr=atoi(str);
    //mqtt_log("sunrise_hr:%d",astime.sunrise_hr);

    	str=ptr+1;
    	ptr=strchr(str,':');
    	if(NULL==ptr)
            continue;
    	*ptr='\0';
    	astime.sunrise_min=atoi(str);
    //mqtt_log("sunrise_min:%d",astime.sunrise_min);


    	ob = json_object_object_get(sub_obj,ASTRONOMICAL_TIME_TYPE_SUNSET);
        if(ob==NULL)
            continue;
        str=json_object_get_string(ob);
        strncpy(strData,str,12);
     // mqtt_log("sunset:%s",strData);

        str=strData;
        ptr=strchr(str,':');
        if(NULL==ptr)
            continue;
    	*ptr='\0';
    	astime.sunset_hr=atoi(str);
    //mqtt_log("sunset_hr:%d",astime.sunset_hr);

    	str=ptr+1;
    	ptr=strchr(str,':');
    	if(NULL==ptr)
            continue;
    	*ptr='\0';
    	astime.sunset_min=atoi(str);
    //mqtt_log("sunset_min:%d",astime.sunset_min);

    	astronomical_time_set(&astime,i,array_length);

    }


exit:

    if(content_json_ob!=NULL)
 		json_object_put(content_json_ob);

}


