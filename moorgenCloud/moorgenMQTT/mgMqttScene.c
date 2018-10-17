#include "mgMqttService.h"
#include "mgOS.h"
#include "moorgenService.h"
#include "mgKernel.h"


static SCENE_OBJECT mqtt_secene_ob;


void mqtt_get_single_scene_req(char *secneCode)
{
    json_object *send_json_object = NULL;
    mqtt_packet_t *mqtt_packet_send_p=NULL;
    char *contentData;

    if(NULL==secneCode)
        return;

    mqtt_log("mqtt_get_single_scene_req");


	send_json_object = json_object_new_object();
	require(send_json_object != NULL, ERROR);

	json_object_object_add(send_json_object,SCENE_TYPE_CODE,json_object_new_string(secneCode) );

	contentData = json_object_to_json_string(send_json_object);

    mqtt_packet_send_p=mqtt_packet_create(MQTT_MSGTYPE_GET_SINGLE_SCENE,strlen(contentData),(uint8_t *)contentData);
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


void mqtt_get_single_scene_rsp(mqtt_packet_t *mqtt_packet_recv_p)
{
	json_object *response_object = NULL;
	json_object *rules_array = NULL;
	json_object *ob = NULL;
    char *sceneCode;
    char *modifyTime;
    int rules_num;
    int i;
    DEV_SUB_OBJECT *dev_sub_ob_p;
    SCENE_HEAD * scene_head=NULL;

	response_object = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(response_object != NULL, exit);

	ob = json_object_object_get(response_object, SCENE_TYPE_CODE);
	require(ob != NULL, exit);
    sceneCode= json_object_get_string(ob);

    memset(&mqtt_secene_ob,0,sizeof(SCENE_OBJECT));
    for(i=0;i<MG_DEVICE_MAX;i++)
        mqtt_secene_ob.devItem[i].devID=MG_ID_INVALID;

    strncpy(mqtt_secene_ob.head.sceneCode,sceneCode,MG_CODE_SIZE);

	ob = json_object_object_get(response_object, SCENE_TYPE_MODIFY_TIME);
	require(ob != NULL, exit);
    modifyTime= json_object_get_string(ob);
    time_to_mgTime(modifyTime,&mqtt_secene_ob.head.modifyTime);

	rules_array = json_object_object_get(response_object, SCENE_TYPE_RULES);
	require(rules_array!=NULL, exit);

    rules_num=json_object_array_length(rules_array);

    if(rules_num>MG_DEVICE_MAX)
        goto exit;

    for(i=0;i<rules_num;i++)
    {
		json_object *tmp_scene = json_object_array_get_idx(rules_array, i);
		ob = json_object_object_get(tmp_scene, DEVICE_ONLINE_TYPE_MAC);
		if(ob==NULL) continue;

        if(strncmp(dev_master_ob_p->devMac,json_object_get_string(ob),MG_DEV_MAC_MAX)==0)
        {
            ob = json_object_object_get(tmp_scene, DEVICE_ONLINE_TYPE_DEVICETYPE);
    		if(ob==NULL) continue;
            if(strncmp(dev_master_ob_p->devType,json_object_get_string(ob),MG_DEVICE_TYPE_SIZE)!=0)
                continue;

            mqtt_secene_ob.devItem[i].devID=MG_MASTER_ID;
            dev_master_ob_p->dev_ops.dev_op_dp_to_struct(dev_master_ob_p->devType,(void *)mqtt_secene_ob.devItem[i].devDP,(void *)tmp_scene);



        }
        else
        {
            if(dev_master_ob_p->hub_flag!=1)
                continue;

            dev_sub_ob_p=device_sub_object_search(json_object_get_string(ob));
            if(NULL==dev_sub_ob_p)
                continue;
            ob = json_object_object_get(tmp_scene, DEVICE_ONLINE_TYPE_DEVICETYPE);
    		if(ob==NULL) continue;
            if(strncmp(dev_sub_ob_p->devType,json_object_get_string(ob),MG_DEVICE_TYPE_SIZE)!=0)
                continue;

            mqtt_secene_ob.devItem[i].devID=dev_sub_ob_p->devID;


            dev_master_ob_p->dev_ops.dev_op_dp_to_struct(dev_sub_ob_p->devType,(void *)mqtt_secene_ob.devItem[i].devDP,(void *)tmp_scene);

        }


    }


    if((scene_head=scene_object_search(mqtt_secene_ob.head.sceneCode))==NULL)
        scene_object_add(&mqtt_secene_ob);
    else
    {
        mqtt_secene_ob.head.sceneID=scene_head->sceneID;
        scene_object_mod(&mqtt_secene_ob);
    }
exit:

    if(response_object!=NULL)
 		json_object_put(response_object);
}

void mqtt_get_scene_codes_req()
{

    uint8_t sceneID=MG_ID_INVALID;
    SCENE_OBJECT *scene_ob_p;
    mqtt_packet_t *mqtt_packet_send_p=NULL;

    mqtt_log("mqtt_get_scene_codes_req");


    mqtt_packet_send_p=mqtt_packet_create(MQTT_MSGTYPE_GET_SCENES_CODE,0,NULL);
	require(mqtt_packet_send_p != NULL, ERROR);

	mqtt_send_to_queue(mqtt_packet_send_p);


    while((scene_ob_p=scene_object_next(sceneID))!=NULL)
    {
        sceneID=scene_ob_p->head.sceneID;
        scene_ob_p->head.flag=0xff;

    }


    return;

ERROR:
    if(mqtt_packet_send_p!=NULL)
        free(mqtt_packet_send_p);

}

int mqtt_get_scene_codes_rsp(mqtt_packet_t *mqtt_packet_recv_p)
{
	json_object *response_object = NULL;
	json_object *scene_array = NULL;
	json_object *tmp_scene;
	json_object *ob = NULL;
    int scene_num;
	int totalPage=0;
	int currentPage=0;
    int i;
    char *sceneCode;
    char *modifyTime;
    MG_TIME_DATA mgTime;
    SCENE_OBJECT *scene_ob_p;
    uint8_t sceneID=MG_ID_INVALID;
    int ret=-1;

	response_object = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(response_object != NULL, exit);

	ob = json_object_object_get(response_object, SCENE_TYPE_TOTAL_PAGE);
	require(ob != NULL, exit);
    totalPage= json_object_get_int(ob);

	ob = json_object_object_get(response_object, SCENE_TYPE_CURRENT_PAGE);
	require(ob != NULL, exit);
    currentPage= json_object_get_int(ob);

	scene_array = json_object_object_get(response_object, SCENE_TYPE_ARRAY);
	require(scene_array!=NULL, exit);

	scene_num = json_object_array_length(scene_array);

    for(i=0;i<scene_num;i++)
    {
		tmp_scene = json_object_array_get_idx(scene_array, i);

    	ob = json_object_object_get(tmp_scene, SCENE_TYPE_CODE);
    	if(ob==NULL)
    	    continue;
        sceneCode= json_object_get_string(ob);

        ob = json_object_object_get(tmp_scene, SCENE_TYPE_MODIFY_TIME);
    	if(ob==NULL)
    	    continue;
        modifyTime= json_object_get_string(ob);

        if((scene_ob_p=scene_object_search(sceneCode))==NULL)
        {
            mqtt_get_single_scene_req(sceneCode);
            continue;
        }
        scene_ob_p->head.flag=0;

        time_to_mgTime(modifyTime,&mgTime);
        if(scene_ob_p->head.modifyTime.year != mgTime.year ||
           scene_ob_p->head.modifyTime.month != mgTime.month ||
           scene_ob_p->head.modifyTime.day != mgTime.day ||
           scene_ob_p->head.modifyTime.hour != mgTime.hour ||
           scene_ob_p->head.modifyTime.minute != mgTime.minute ||
           scene_ob_p->head.modifyTime.second != mgTime.second )
        {
            mqtt_get_single_scene_req(sceneCode);

        }



    }

    ret=0;


    if(currentPage==totalPage)
    {
        while((scene_ob_p=scene_object_next(sceneID))!=NULL)
        {
            sceneID=scene_ob_p->head.sceneID;
            if(0xff==scene_ob_p->head.flag)
                scene_object_del(scene_ob_p->head.sceneID);

        }

        ret=1;
    }


exit:

    if(response_object!=NULL)
 		json_object_put(response_object);

    return ret;
}


void mqtt_req_reget_scene(mqtt_packet_t *mqtt_packet_recv_p)
{
	json_object *content_json_ob = NULL;
	json_object *ob = NULL;
	char *sceneCode=NULL;

	content_json_ob = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(content_json_ob != NULL, exit);
	ob = json_object_object_get(content_json_ob, SCENE_TYPE_CODE);
	require(ob != NULL, exit);

	sceneCode=json_object_get_string(ob);

    mqtt_get_single_scene_req(sceneCode);


exit:
    if(content_json_ob!=NULL)
 		json_object_put(content_json_ob);


    return ;
}


void mqtt_req_scene_control(mqtt_packet_t *mqtt_packet_recv_p)
{
	json_object *content_json_ob = NULL;
	json_object *ob = NULL;
	char *sceneCode=NULL;

	content_json_ob = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(content_json_ob != NULL, exit);
	ob = json_object_object_get(content_json_ob, SCENE_TYPE_CODE);
	require(ob != NULL, exit);
	sceneCode= json_object_get_string(ob);

    scene_object_exe(sceneCode);

exit:

    if(content_json_ob!=NULL)
 		json_object_put(content_json_ob);


    return ;
}

void mqtt_req_scene_del(mqtt_packet_t *mqtt_packet_recv_p)
{
	json_object *content_json_ob = NULL;
	json_object *ob = NULL;
	char *sceneCode=NULL;
    SCENE_HEAD *scene_head_p=NULL;

	content_json_ob = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(content_json_ob != NULL, exit);
	ob = json_object_object_get(content_json_ob, SCENE_TYPE_CODE);
	require(ob != NULL, exit);
	sceneCode= json_object_get_string(ob);

    scene_head_p=scene_object_search(sceneCode);
    if(scene_head_p==NULL)
        goto exit;

    scene_object_del(scene_head_p->sceneID);

exit:

    if(content_json_ob!=NULL)
 		json_object_put(content_json_ob);


    return ;
}


