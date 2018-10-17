#include "mgMqttService.h"
#include "mgOS.h"
#include "moorgenService.h"
#include "mgKernel.h"
#include "Product.h"




int mqtt_slave_report_req()
{
    mqtt_packet_t *mqtt_packet_send_p=NULL;
    const char *contentData;
	json_object *device_list_object = NULL;
	json_object *device_body_object = NULL;
    uint8_t devID=MG_ID_INVALID;
    DEV_SUB_OBJECT *dev_sub_ob_p;

    if(device_hub_check()==0)
        return 0;

    mqtt_log("mqtt_slave_report_req");


	device_list_object = json_object_new_array();
	require(device_list_object != NULL, ERROR);


    while((dev_sub_ob_p=device_sub_object_next(devID))!=NULL)
    {
        devID=dev_sub_ob_p->devID;

		device_body_object = json_object_new_object();
		if(device_body_object==NULL)
		    continue;

    	json_object_object_add(device_body_object,SLAVE_REPORT_TYPE_MAC, json_object_new_string(dev_sub_ob_p->devMac));
    	json_object_object_add(device_body_object,SLAVE_REPORT_TYPE_DEVICETYPE, json_object_new_string(dev_sub_ob_p->devType));

		json_object_array_add(device_list_object, device_body_object);

    }

	contentData = json_object_to_json_string(device_list_object);

    mqtt_packet_send_p=mqtt_packet_create(MQTT_MSGTYPE_SLAVE_REPORT,strlen(contentData),(uint8_t *)contentData);
	require(mqtt_packet_send_p != NULL, ERROR);

	mqtt_send_to_queue(mqtt_packet_send_p);

	json_object_put(device_list_object);

    return 0;

ERROR:
    if(mqtt_packet_send_p!=NULL)
        free(mqtt_packet_send_p);

    if(device_list_object!=NULL)
		json_object_put(device_list_object);

    return -1;
}





int mqtt_device_online_req(DEV_SUB_OBJECT *dev_sub_ob_p)
{
    json_object *send_json_object = NULL;
    mqtt_packet_t *mqtt_packet_send_p=NULL;
    const char *contentData;

    mqtt_log("mqtt_device_online_req");

	send_json_object = json_object_new_object();
	require(send_json_object != NULL, ERROR);

	json_object_object_add(send_json_object,DEVICE_ONLINE_TYPE_MAC, json_object_new_string(dev_sub_ob_p->devMac));
	json_object_object_add(send_json_object,DEVICE_ONLINE_TYPE_DEVICETYPE, json_object_new_string(dev_sub_ob_p->devType));

	contentData = json_object_to_json_string(send_json_object);

    mqtt_packet_send_p=mqtt_packet_create(MQTT_MSGTYPE_DEVICE_ONLINE,strlen(contentData),(uint8_t *)contentData);
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


void mqtt_device_all_online_req( )
{
    uint8_t devID=MG_ID_INVALID;
    DEV_SUB_OBJECT *dev_sub_ob_p;

    if(device_hub_check()==0)
        return;
    mqtt_log("mqtt_device_all_online_req");


    while((dev_sub_ob_p=device_sub_object_next(devID))!=NULL)
    {
        devID=dev_sub_ob_p->devID;

        mqtt_device_online_req(dev_sub_ob_p);

    }

}



void mqtt_device_data_report_req(const char *contentData)
{
    mqtt_packet_t *mqtt_packet_send_p=NULL;

    if(mqtt_status_get()!=mqtt_running)
        return;

    mqtt_log("mqtt_device_data_report_req");


    mqtt_packet_send_p=mqtt_packet_create(MQTT_MSGTYPE_DATA_REPORT,strlen(contentData),(uint8_t *)contentData);
	require(mqtt_packet_send_p != NULL, ERROR);


	mqtt_send_to_queue(mqtt_packet_send_p);

    return;

ERROR:
    if(mqtt_packet_send_p!=NULL)
        free(mqtt_packet_send_p);

    return ;
}


void mqtt_req_start_commission(mqtt_packet_t *mqtt_packet_recv_p)
{
	json_object *content_json_ob = NULL;
	json_object *ob = NULL;
    char *slaveMac=NULL;
    char *slaveType=NULL;
    char *clientMsg=NULL;
    char *serverMsg=NULL;

    mqtt_log("mqtt_req_start_commission");
    if(dev_master_ob_p->pair_flag==1)
        return;


	content_json_ob = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(content_json_ob != NULL, exit);

	ob = json_object_object_get(content_json_ob, SUB_SLAVE_DEVICE_MAC);
	if(ob!=NULL)
    	slaveMac= json_object_get_string(ob);
	ob = json_object_object_get(content_json_ob, SUB_SLAVE_DEVICE_TYPE);
	if(ob!=NULL)
    	slaveType= json_object_get_string(ob);

    ob = json_object_object_get(content_json_ob, SUB_SLAVE_DEVICE_SERVER_MSG);
	if(ob!=NULL)
    	serverMsg= json_object_get_string(ob);

    ob = json_object_object_get(content_json_ob, SUB_SLAVE_DEVICE_CLIENT_MSG);
	if(ob!=NULL)
    	clientMsg= json_object_get_string(ob);

    if(mqtt_command_result(START_COMMISSION,serverMsg)<0)
        return;

    if(dev_master_ob_p!=NULL  &&  dev_master_ob_p->hub_flag)
    {
        dev_master_ob_p->pair_flag=1;
        dev_master_ob_p->dev_ops.dev_op_start_pair(slaveMac,slaveType,clientMsg);
    }
exit:

    if(content_json_ob!=NULL)
 		json_object_put(content_json_ob);


    return ;
}



void mqtt_req_device_control(mqtt_packet_t *mqtt_packet_recv_p)
{
	json_object *content_json_ob = NULL;
	json_object *ob = NULL;

    char *devMac=NULL;
    char *devType=NULL;
    DEV_SUB_OBJECT *dev_sub_ob_p;
    uint8_t  devDP[MG_DEV_DP_MAX];



    mqtt_log("mqtt_req_device_control");
    if(dev_master_ob_p->pair_flag)
        return;

	content_json_ob = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(content_json_ob != NULL, exit);
	ob = json_object_object_get(content_json_ob, SLAVE_REPORT_TYPE_MAC);		//require mac 
	if(ob!=NULL)
    	devMac= json_object_get_string(ob);		//require  device  mac
	ob = json_object_object_get(content_json_ob, SLAVE_REPORT_TYPE_DEVICETYPE);	//require deviceType
	if(ob!=NULL)
    	devType= json_object_get_string(ob);										//device type   Json ->  string

    if(NULL==devMac || NULL==devType)
        goto exit;

    if(strncmp(dev_master_ob_p->devMac,devMac,MG_DEV_MAC_MAX)==0)
    {

        dev_master_ob_p->dev_ops.dev_op_dp_to_struct(dev_master_ob_p->devType,(void *)devDP,(void *)content_json_ob);
        dev_master_ob_p->dev_ops.dev_op_dp_ctrl(DEVICE_MASTER,(void *)dev_master_ob_p,(void *)devDP);
    }
    else
    {
        if(dev_master_ob_p->hub_flag!=1)
            goto exit;

        dev_sub_ob_p=device_sub_object_search(devMac);
        if(dev_sub_ob_p!=NULL)
        {
            dev_master_ob_p->dev_ops.dev_op_dp_to_struct(dev_sub_ob_p->devType,(void *)devDP,(void *)content_json_ob);
            dev_master_ob_p->dev_ops.dev_op_dp_ctrl(DEVICE_SUB,(void *)dev_sub_ob_p,(void *)devDP);

        }

    }




exit:

    if(content_json_ob!=NULL)
 		json_object_put(content_json_ob);


    return ;
}






void mqtt_req_delete_slave(mqtt_packet_t *mqtt_packet_recv_p)
{
	json_object *content_json_ob = NULL;
	json_object *ob = NULL;

    char *devMac=NULL;
    char *devType=NULL;
    DEV_SUB_OBJECT *dev_sub_ob_p=NULL;
    char *serverMsg=NULL;

    mqtt_log("mqtt_req_delete_slave");





	content_json_ob = json_tokener_parse((char *)mqtt_packet_recv_p->content_data);
	require(content_json_ob != NULL, exit);
	ob = json_object_object_get(content_json_ob, SUB_SLAVE_DEVICE_MAC);
	if(ob!=NULL)
    	devMac= json_object_get_string(ob);
	ob = json_object_object_get(content_json_ob, SUB_SLAVE_DEVICE_TYPE);
	if(ob!=NULL)
    	devType= json_object_get_string(ob);

	ob = json_object_object_get(content_json_ob, SUB_SLAVE_DEVICE_SERVER_MSG);
	if(ob!=NULL)
    	serverMsg= json_object_get_string(ob);

    if(mqtt_command_result(DEL_COMMISSION,serverMsg)<0)
        goto exit;;

    if(devMac==NULL || devType==NULL)
    {
        mqtt_command_result_error_msg(COMMAND_ERR_NODE_UNFIND,devMac,devType,NULL);
        mqtt_command_result_req(0);
        goto exit;;
    }

    if(dev_master_ob_p->pair_flag)
    {
        mqtt_command_result_error_msg(COMMAND_ERR_REMOVE_NODE_FAILED,devMac,devType,NULL);
        mqtt_command_result_req(0);

    }
    else
    {
        dev_sub_ob_p=device_sub_object_search(devMac);
        if(dev_sub_ob_p!=NULL)
        {

            device_sub_object_del(dev_sub_ob_p);

            mqtt_command_result_error_msg(COMMAND_NORMAL,devMac,devType,NULL);
            mqtt_command_result_req(1);

            mqtt_slave_report_req();
            mqtt_device_all_online_req();
        }
        else
        {
            mqtt_command_result_error_msg(COMMAND_ERR_NODE_UNFIND,devMac,devType,NULL);
            mqtt_command_result_req(0);
        }
    }

exit:

    if(content_json_ob!=NULL)
 		json_object_put(content_json_ob);


    return ;

}


void mqtt_device_alldp_report()
{

    uint8_t devID=MG_ID_INVALID;
    DEV_SUB_OBJECT *dev_sub_ob_p;

    curtainMsgReport();
//    devive_dp_report(DEVICE_MASTER,dev_master_ob_p);

    if(dev_master_ob_p->hub_flag!=1)
        return;



    while((dev_sub_ob_p=device_sub_object_next(devID))!=NULL)
    {
        devID=dev_sub_ob_p->devID;

        devive_dp_report(DEVICE_SUB,dev_sub_ob_p);

    }

}



