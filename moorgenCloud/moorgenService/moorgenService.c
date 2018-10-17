#include "moorgenService.h"
#include "mgOS.h"
#include "moorgenLocalServer.h"
#include "mgMqttService.h"
#include "mgKernel.h"

static MOORGEN_SYS_INFO moorgen_sys_info;
static moorgen_led_func  led_set_func=NULL;

char  *moorgen_dev_mac_get()
{
    return moorgen_sys_info.device_mac;
}

char  *moorgen_dev_type_get()
{
    return moorgen_sys_info.moorgen_des.deviceType;
}


char  *moorgen_module_factory_get()
{
    return moorgen_sys_info.module_factory;
}

char  *moorgen_module_type_get()
{
    return moorgen_sys_info.moorgen_des.module_type;
}

void moorgen_deviceCode_set(char  *deviceCode)
{
    mg_log("moorgen_deviceCode_set");
    mg_log("deviceCode:%s",deviceCode);

    if(strncmp(moorgen_sys_info.moorgen_des.device_code,deviceCode,MAX_DEVICECODE_SIZE)!=0)
    {
        strncpy(moorgen_sys_info.moorgen_des.device_code,deviceCode,MAX_DEVICECODE_SIZE);
        mg_eeprom_write(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));

    }

    moorgen_sys_info.is_activate=1;

}

char *moorgen_deviceCode_get()
{

    return moorgen_sys_info.moorgen_des.device_code;
}



void moorgen_domain_name_set(char  *domainName)
{
    mg_log("moorgen_domain_name_set");
    mg_log("domainName:%s",domainName);

    if(strncmp(moorgen_sys_info.moorgen_des.domainName,domainName,MAX_DOMAIN_NAME_SZIE)!=0)
    {
        strncpy(moorgen_sys_info.moorgen_des.domainName,domainName,MAX_DOMAIN_NAME_SZIE);
        mg_eeprom_write(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));

    }

}

char *moorgen_domain_name_get()
{

    return moorgen_sys_info.moorgen_des.domainName;
}


void moorgen_userkey_set(char  *userkey)
{



//userkey:f96edcd9-c1ce-46f1-8113-75c19af6a370



    if(memcmp(moorgen_sys_info.moorgen_des.userKey,userkey,16)==0)
        return;

    memcpy(moorgen_sys_info.moorgen_des.userKey,userkey,16);
    mg_eeprom_write(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));

}


uint8_t  *moorgen_userkey_get( )
{

    return moorgen_sys_info.moorgen_des.userKey;

}


MOORGEN_SYS_INFO *moorgen_sys_info_get()
{
    return &moorgen_sys_info;
}

void moorgen_sys_info_update()
{

    mg_wifi_info_get(moorgen_sys_info.ssid,moorgen_sys_info.localIp,moorgen_sys_info.bssid);	//读取并设置当前连接的wifi  ssid和password
																						//到moorgen_sys_info 中
    mg_wifi_mac_get(moorgen_sys_info.device_mac);											//读取mac

    device_master_mac_set(moorgen_sys_info.device_mac);
 //    mgos_log("ssid:%s",moorgen_sys_info.ssid);
   //  mgos_log("localIp:%s",moorgen_sys_info.localIp);
  //mgos_log("bssid:%s",moorgen_sys_info.bssid);

}


void moorgen_is_local_server_set(uint8_t set_flag)
{
     moorgen_sys_info.is_local_server=set_flag;
}


uint8_t moorgen_is_local_server_get()
{
    return moorgen_sys_info.is_local_server;
}


void moorgen_is_ota_set(uint8_t set_flag)
{
     moorgen_sys_info.is_ota=set_flag;
}


uint8_t moorgen_is_ota_get()
{
    return moorgen_sys_info.is_ota;
}


void moorgen_is_bind_reset()
{
    moorgen_sys_info.is_bind_user=0;
}



void moorgen_user_data_write(uint8_t *data, int len)
{
    
    mg_eeprom_write(USER_EE_ADDR,(uint8_t *)data,len);

}

void moorgen_user_data_read(uint8_t *data, int len)
{
    
    mg_eeprom_read(USER_EE_ADDR,(uint8_t *)data,len);

}




void  moorgen_service_recover()
{
    memset(&moorgen_sys_info, 0, sizeof(MOORGEN_SYS_INFO));
    mg_eeprom_write(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));

}


int moorgen_service_init(char *config_factory,char *config_product,char *config_device_type,char *config_version)
{
    MG_TIME_DATA mgTime;

    memset(&moorgen_sys_info, 0, sizeof(MOORGEN_SYS_INFO));   //全局变量清空
    moorgen_sys_info.is_activate=0;			//设备是否激活(注册)
    moorgen_sys_info.is_ota=0;				//OTA 功能是否开启
    moorgen_sys_info.is_bind_user=0;		//是否已经绑定用户(配网)
    moorgen_sys_info.is_local_server=0;		//标识本地(局域网) 服务器是否开启，默认为0，使用mqtt对接云服务器

    
    mg_log("moorgen_service_init");

	//读取Eeprom中SYSTEM_EE_ADDR分区中的相关信息
    mg_eeprom_read(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));

   // mg_log("moorgen_service_init");

	//对比读出来的模块类型和设备类型是否与设定值一致
    if((strncmp(moorgen_sys_info.moorgen_des.module_type,config_product,32)==0) &&
     (strncmp(moorgen_sys_info.moorgen_des.deviceType,config_device_type,MAX_DEVICE_TYPE_SIZE)==0))
    {

        if(strlen(moorgen_sys_info.moorgen_des.device_code)!=0)		//设备代码不为0
        {
      		mg_log("*************device REGISTER 1****************** ");
		mg_log("device_code = %s",moorgen_sys_info.moorgen_des.device_code);	
          	moorgen_sys_info.is_activate=1;							//标志激活位
        }
	else
		mg_log("*************device  No REGISTER 0****************** ");

    }
    else			//如果不一致，则设置成设定值
    {
        memset(&moorgen_sys_info.moorgen_des,0,sizeof(MOORGEN_DES_S));
        strncpy(moorgen_sys_info.moorgen_des.deviceType,config_device_type,MAX_DEVICE_TYPE_SIZE);
        strncpy(moorgen_sys_info.moorgen_des.module_type,config_product,32);
    }
    mg_log("module_type:%s",moorgen_sys_info.moorgen_des.module_type);
    mg_log("deviceType:%s",moorgen_sys_info.moorgen_des.deviceType);
    mg_log("device_code :%s",moorgen_sys_info.moorgen_des.device_code);

	//设置版本和出厂("connector")
    strncpy(moorgen_sys_info.firmware,config_version,MAX_FW_SIZE);
    strncpy(moorgen_sys_info.module_factory,config_factory,MAX_MODULE_FACTORY_SIZE);

	//获取设备mac地址
    mg_wifi_mac_get(moorgen_sys_info.device_mac);

	//设置系统时间
    memset(&mgTime,0,sizeof(mgTime));
    mgTime.year=2000;
    mg_rtc_set_time(&mgTime);


	//读出EEPROM中场景和定时相关信息，并对场景和定时初始化
    mg_kernel_init();

    return 0;
    
}





void moorgen_service_start()
{

    
    moorgen_udp_server_start();		
    
    mg_mqtt_service_init();

    

}



void moorgen_service_led_register(moorgen_led_func func)
{
    led_set_func=func;
}

void moorgen_service_led_set(int led_status)
{
    if(led_set_func!=NULL)
        led_set_func(led_status);
}
