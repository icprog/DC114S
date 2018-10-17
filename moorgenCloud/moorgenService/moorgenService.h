#ifndef __MOORGEN_SERVICE_H__
#define __MOORGEN_SERVICE_H__


#include <stdint.h>


#define MAX_USER_TOKEN_SIZE	    (80)
#define MAX_TIMEZONE_SIZE       68
#define MAX_DEVICE_TYPE_SIZE    12
#define MAX_MODULE_FACTORY_SIZE   20

#define MAX_MAC_SIZE  (20)
#define MAX_FW_SIZE (16)
#define MAX_DEVICE_SIGN_CODE_SIZE (128)
#define MAX_SERIALNO_SIZE (16)
#define MAX_DEVICECODE_SIZE (40)
#define MAX_DOMAIN_NAME_SZIE 200

typedef struct
{	
    char module_type[32];		//模块型号
	char deviceType[MAX_DEVICE_TYPE_SIZE];	//设备类型
	char device_code[MAX_DEVICECODE_SIZE];		// 设备标识码
	uint8_t userKey[16];
	char    serialNo[16];
	char    domainName[MAX_DOMAIN_NAME_SZIE];
	/*new add: store  journey*/
	uint32_t journey_up;
	uint32_t journey_down;

	uint8_t currentState;			//0:  no set    3: set
	uint8_t currentPostion;
	uint8_t targetPostion;		
	uint8_t status;
	uint8_t direction;
	
}MOORGEN_DES_S;


typedef struct
{
	uint8_t is_bind_user;		// 是否绑定设备
	uint8_t is_activate;
	uint8_t is_local_server;
	uint8_t is_ota;	
	char module_factory[MAX_MODULE_FACTORY_SIZE];
	char timezone[MAX_TIMEZONE_SIZE ];		//时区信息
	char device_mac[MAX_MAC_SIZE];		//MAC地址
	char firmware[MAX_FW_SIZE];		// 软件版本
	char user_access_tokern[MAX_USER_TOKEN_SIZE];	//用户访问令牌
	char access_token[37];		// 登陆令牌
	char refresh_token[37];		// 刷新令牌
	char deviceSignCode[MAX_DEVICE_SIGN_CODE_SIZE];
	char localIp[20];
	char ssid[40];
    char bssid[6];
    MOORGEN_DES_S  moorgen_des;
	
}MOORGEN_SYS_INFO;


typedef  void  (*moorgen_led_func)(int led_status);



typedef enum
{
	LED_POWER_ON=0,
	LED_WIF_CONFIG,
	LED_WIFI_CONNECTING,
	LED_WIFI_CONNECTED,
	LED_MQTT_CONNECTED,
	LED_MQTT_DISCONNECTED,
	LED_HUB_PAIRING,
	LED_HUB_UPDATE,
	LED_HUB_RESET,
	LED_LAST,
	LED_MAX
	
} mg_led_status_t;


void moorgen_domain_name_set(char  *domainName);
char *moorgen_domain_name_get();
char *moorgen_deviceCode_get();
void moorgen_deviceCode_set(char  *deviceCode);
char  *moorgen_dev_mac_get();
char  *moorgen_dev_type_get();
char  *moorgen_module_factory_get();
char  *moorgen_module_type_get();
void moorgen_userkey_set(char  *userkey);
uint8_t  *moorgen_userkey_get( );
void moorgen_is_local_server_set(uint8_t set_flag);
uint8_t moorgen_is_local_server_get();
void moorgen_is_ota_set(uint8_t set_flag);
uint8_t moorgen_is_ota_get();
void moorgen_is_bind_reset();
void  moorgen_service_recover();

void moorgen_user_data_write(uint8_t *data, int len);
void moorgen_user_data_read(uint8_t *data, int len);


MOORGEN_SYS_INFO *moorgen_sys_info_get();
void moorgen_sys_info_update();

int moorgen_service_init(char *config_factory,char *config_product,char *config_device_type,char *config_version);


void moorgen_wifi_config_rsp(int result);
void moorgen_wifi_config_start( void );
void moorgen_wifi_config_stop( void );

void moorgen_service_start();

void moorgen_service_led_register(moorgen_led_func func);
void moorgen_service_led_set(int led_status);


#endif
