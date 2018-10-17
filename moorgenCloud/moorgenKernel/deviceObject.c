#include "mgKernel.h"
#include "mgMqttService.h"

DEV_MASTER_OBJEC   *dev_master_ob_p=NULL;
static DEV_SUB_OBJECT     dev_sub_ob;
static DMG_OBJECT *dev_sub_dmg=NULL;

void device_sub_printf(DEV_SUB_OBJECT *dev_sub_ob_p);


DEV_SUB_OBJECT *device_sub_object_get(uint8_t devID)
{
    DEV_SUB_OBJECT *dev_sub_ob_p;

    dev_sub_ob_p=(DEV_SUB_OBJECT *)dmgGetValue(dev_sub_dmg,devID);

    if(dev_sub_ob_p !=NULL )
    {
        if(devID == dev_sub_ob_p->devID)
            return dev_sub_ob_p;

    }

    return NULL;
}

static  __inline void device_sub_object_set(DEV_SUB_OBJECT *dev_sub_ob_p)
{
    dmgSetValue(dev_sub_dmg,dev_sub_ob_p->devID,(void *)dev_sub_ob_p);

}

DEV_SUB_OBJECT *device_sub_object_search(char *mac)
{
    uint8_t devID=MG_ID_INVALID;
    DEV_SUB_OBJECT *dev_sub_ob_p;

    while((devID=dmgGetNextID(dev_sub_dmg,devID))!=MG_ID_INVALID)
    {
        dev_sub_ob_p=device_sub_object_get(devID);
        if(NULL==dev_sub_ob_p)
            continue;

        if(strncmp(dev_sub_ob_p->devMac,mac,MG_DEV_MAC_MAX)==0)
            return dev_sub_ob_p;

    }

    return NULL;
}


int device_sub_object_inuse_get()
{
    return dev_sub_dmg->inUse;
}

int device_sub_object_add(DEV_SUB_OBJECT *dev_sub_ob_p)
{
    uint8_t devID;
    uint16_t  macID;

    if(strlen(dev_sub_ob_p->devType)==0)
        return -1;

    devID=dmgGetID(dev_sub_dmg);
    if(MG_ID_INVALID==devID)
        return -1;

    dev_sub_ob_p->devID=devID;

    macID=mg_kernel_mac_count_get();

    sprintf(dev_sub_ob_p->devMac,"%s%02x%02x",dev_master_ob_p->devMac,macID>>8,macID&0xff);
    device_sub_object_set(dev_sub_ob_p);

    //save
    mg_eeprom_write(DEVICE_EE_ADDR+devID*sizeof(DEV_SUB_OBJECT), (uint8_t *)dev_sub_ob_p, sizeof(DEV_SUB_OBJECT));

    mg_kernel_devbit_set(dev_sub_dmg->bitSet);

    device_sub_printf(dev_sub_ob_p);


    return 0;
}


int device_sub_object_mod(DEV_SUB_OBJECT *dev_sub_ob_p )
{

    device_sub_object_set(dev_sub_ob_p);

    //save
    mg_eeprom_write(DEVICE_EE_ADDR+dev_sub_ob_p->devID*sizeof(DEV_SUB_OBJECT), (uint8_t *)dev_sub_ob_p, sizeof(DEV_SUB_OBJECT));

    device_sub_printf(dev_sub_ob_p);


    return 0;
}


void device_sub_object_del(DEV_SUB_OBJECT *dev_sub_ob_p)
{

    if(dev_master_ob_p && dev_master_ob_p->hub_flag)
    {
        dev_master_ob_p->dev_ops.dev_op_del(DEVICE_SUB,(void *)dev_sub_ob_p);
    }
    scene_object_item_del(dev_sub_ob_p->devID);
    timer_object_dev_item_del(dev_sub_ob_p->devID);
    dmgDelID(dev_sub_dmg,dev_sub_ob_p->devID);

    mg_kernel_devbit_set(dev_sub_dmg->bitSet);



}

DEV_SUB_OBJECT *device_sub_object_next(uint8_t devID)
{

    devID=dmgGetNextID(dev_sub_dmg,devID);
    if(MG_ID_INVALID==devID)
        return NULL;


    return (DEV_SUB_OBJECT *)device_sub_object_get(devID);

}



void device_sub_object_all_reset()
{
    if(dev_sub_dmg->bitSet!=0)
    {
        dev_sub_dmg->bitSet=0;
        if(dev_master_ob_p->dev_ops.dev_op_del!=NULL)
            dev_master_ob_p->dev_ops.dev_op_del(DEVICE_MASTER,NULL);
    }
}

int device_hub_check()
{
   return  dev_master_ob_p->hub_flag;
}


void mg_device_init()
{
    uint32_t    bitSet;
    int i;

    if(dev_master_ob_p==NULL)
        return;

    if(dev_master_ob_p->hub_flag==0)
        return;

    dev_sub_dmg=dmgCreate(MG_DEVICE_MAX,sizeof(DEV_SUB_OBJECT));

    bitSet=mg_kernel_devbit_get();

    for(i=0; i < MG_DEVICE_MAX; i ++)
    {

        if(bitSet & (0x01 << i))
        {


            memset(&dev_sub_ob,0,sizeof(DEV_SUB_OBJECT));
            mg_eeprom_read(DEVICE_EE_ADDR+i*sizeof(DEV_SUB_OBJECT), (uint8_t *)&dev_sub_ob, sizeof(DEV_SUB_OBJECT));
            if(dev_sub_ob.devID != i)
                continue;

            if(dmgInsertID(dev_sub_dmg, dev_sub_ob.devID)==MG_ID_INVALID)
                continue;

            device_sub_object_set(&dev_sub_ob);

        }
    }

}

void  devive_dp_report(DEVICE_TYPE type ,void *arg )
{
	json_object *content_json_ob = NULL;
    DEV_SUB_OBJECT *dev_sub_ob_p;
    const char *contentData;

    if(NULL==dev_master_ob_p)
        return;



    mqtt_log("devive_dp_report");



	content_json_ob = json_object_new_object();
	require(content_json_ob != NULL, ERROR);



    if(DEVICE_MASTER==type)
    {
    	json_object_object_add(content_json_ob,STATUS_REPORT_TYPE_MAC, json_object_new_string(dev_master_ob_p->devMac));
    	json_object_object_add(content_json_ob,STATUS_REPORT_TYPE_DEVICETYPE, json_object_new_string(dev_master_ob_p->devType));

        dev_master_ob_p->dev_ops.dev_op_dp_to_json(dev_master_ob_p->devType,(void *)dev_master_ob_p->devDP,(void *)content_json_ob);
    }
    else
    {
        dev_sub_ob_p=(DEV_SUB_OBJECT *)arg;
    	json_object_object_add(content_json_ob,STATUS_REPORT_TYPE_MAC, json_object_new_string(dev_sub_ob_p->devMac));
    	json_object_object_add(content_json_ob,STATUS_REPORT_TYPE_DEVICETYPE, json_object_new_string(dev_sub_ob_p->devType));

        dev_master_ob_p->dev_ops.dev_op_dp_to_json(dev_sub_ob_p->devType,(void *)dev_sub_ob_p->devDP,(void *)content_json_ob);
    }



	contentData = json_object_to_json_string(content_json_ob);


    mqtt_device_data_report_req(contentData);


ERROR:
    if(content_json_ob!=NULL)
		json_object_put(content_json_ob);

}





void device_commissionResult(int result,DEV_SUB_OBJECT *dev_sub_ob_p )
{
    dev_master_ob_p->pair_flag=0;

    if(COMMAND_NORMAL==result)
    {
        device_sub_object_add(dev_sub_ob_p);


        mqtt_slave_report_req();
        mqtt_device_all_online_req();

        mqtt_command_result_error_msg(COMMAND_NORMAL,dev_sub_ob_p->devMac,dev_sub_ob_p->devType,NULL);
        mqtt_command_result_req(1);

    }
    else if(COMMAND_ERR_TIMEROUT==result)
    {
        mqtt_command_result_error_msg(COMMAND_ERR_TIMEROUT," "," ",NULL);
        mqtt_command_result_req(0);

    }
    else if(COMMAND_ERR_DEVICE_EXITED==result)
    {
        mqtt_command_result_error_msg(COMMAND_ERR_DEVICE_EXITED,dev_sub_ob_p->devMac,dev_sub_ob_p->devType,NULL);
        mqtt_command_result_req(1);
        mqtt_slave_report_req();

        mqtt_device_all_online_req();

    }

}


void device_master_init(DEV_MASTER_OBJEC  *dev_master_p)
{
    dev_master_ob_p=dev_master_p;
    dev_master_ob_p->dev_ops.dev_op_pair_result=device_commissionResult;
    dev_master_ob_p->dev_ops.dev_op_dp_report=devive_dp_report;

    mg_device_init();

}


void device_master_mac_set(char *mac)
{
    if(dev_master_ob_p)
    {
        strncpy(dev_master_ob_p->devMac,mac,MG_DEV_MAC_MAX);
    }
}

void device_sub_printf(DEV_SUB_OBJECT *dev_sub_ob_p)
{

    mg_log("device_sub_printf");

    mg_log("ID:%d",dev_sub_ob_p->devID);
    mg_log("mac:%s",dev_sub_ob_p->devMac);
    mg_log("type:%s",dev_sub_ob_p->devType);
    mg_log("res:%d",dev_sub_ob_p->res);


}



