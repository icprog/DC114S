#ifndef __MG_MQTT_SERVICE_H__
#define __MG_MQTT_SERVICE_H__

#include "mgMQTTClient.h"
#include "tlv_box.h"
#include "mgMQTT.h"
#include "json.h"
#include "mgKernel.h"

#define mqtt_log(M, ...) custom_log("MQTT", M, ##__VA_ARGS__)


#define MQTT_CLIENT_PUB_TOPIC   "moorgen/send"



#define TAG_RETCODE	0x80	// 返回码
#define TAG_RETMSG	0x81	// 返回消息
#define TAG_MSGTYPE	0x82	// 请求类型
#define TAG_CONTENT	0x83	// 报文内容
#define TAG_TOKEN	0x84	// 登陆令牌
#define TAG_REMARKS	0x85	// 备注
#define TAG_MSGID	0x86	// 消息id
#define TAG_TIMESTAMPS	0x87	// 时间戳
#define TAG_SIGN		0x88	// 签名
#define TAG_VERSION		0x89	// 协议版本号
#define TAG_DEVICETYPE	0x90
#define TAG_MAC		0x91
#define TAG_DEVICECODE	0x92
#define TAG_PARENTCODE	0x93
#define TAG_FWVERSION	0x94	//固件版本
#define TAG_DEVICE_SIGN_CODE	0x95	// 设备特征值
#define TAG_GWMAC	0x9F01
#define TAG_COMMAND_TYPE	0x9F41
#define TAG_COMMAND_NAME	0x9F42
#define TAG_COMMAND_ID	0x9F43
#define TAG_COMMAND_VALUE	0x9F44
#define TAG_SENSOR_TYPE	0x9F22
#define TAG_SENSOR_CODE	0x9F21
#define TAG_CLIENTID	0x9F81
#define TAG_TDATA	0x9F82
#define TAG_APPID	0x9F83


#define MQTT_CLIENT_KEEPALIVE       (15)
#define MQTT_CMD_TIMEOUT            (10000)  // 5s
#define MQTT_YIELD_TIME            (100)  // 100ms


#define MAX_MQTT_TOPIC_SIZE         (512)
#define MAX_MQTT_DATA_SIZE          (3072)
#define MAX_MQTT_SEND_QUEUE_SIZE    (50)
#define MAX_MQTT_RECV_QUEUE_SIZE    (50)

#define MQTT_SUBSCRIBE_RETRY        (3)  //MQTT订阅失败重试次数
#define MAX_MQTT_RETRY_TIME	(3000)	//重新发起请求的时间间隔3s
#define MAX_MQTT_RESPONSE_TIMEOUT  (30*1000)		//获取响应的超时时间 (修改超时时间从2分钟缩短为30s)


#define MSGID_SIZE (22)
#define CONTENT_SIZE (4096)
#define MAX_CLOUD_STORAGE_BUFF  (512)	// 获取云端自存储数据最大字节数
#define MAX_SINGLE_CLOUD_STORAGE_BUFF (512)	//获取云端自存储数据单字节最大字节数

#define MQTT_RETCODE_SUCESS ("0000")
#define MQTT_RETCODE_FAILED ("1111")

/* Gateway */
#define SLAVE_MAC_SIZE (20)
#define SLAVE_TYPE_SIZE (12)
#define SLAVE_SERVER_MSG_SIZE (64)
#define SLAVE_CLIENT_MSG_SIZE (128)




#define MAX_COMMAND_ID_SIZE (8)
#define MAX_DETAILMSG_SIZE   (64)
#define MAX_DEVICE_MSG_SIZE 256

#define MAX_ERROR_CODE_SIZE (6)
#define MAX_ERROR_MSG_SIZE	(512)

#define URL_PATH	(128)
#define MD5_SIZE		(32)

//BIND USER
#define MAX_USERCODE_SIZE 36

//scene
#define MAX_SCENE_BUFF	(3072)
#define MAX_SCENE_CODE_SIZE 	(36)
#define MAX_SCENE_MODIFY_TIME_SIZE		(19)
#define MAX_SCENE_RULES_SIZE	(512)

#define MAX_SCENES_PAGE_ALIAS_SIZE	(8)
#define MAX_PAGE_SCENES_NUM	(6)
//TODO
#define MAX_SCENE_DEVICE_NUM  (30)	//场景允许最大的设备数量

//timer
#define MAX_TIMER_BUFF	(1024)
#define MAX_TIMER_CODE_SIZE	MAX_SCENE_CODE_SIZE
#define MAX_TIMER_MODIFY_TIME_SIZE	 MAX_SCENE_MODIFY_TIME_SIZE

#define MAX_TIMER_TRIGGER_TIME_SIZE	(20)
#define MAX_TIMER_CRON_SIZE	(32)
#define MAX_TIMER_RULES_SIZE MAX_SCENE_RULES_SIZE
#define MAX_TIMER_DEVICE_NUM MAX_SCENE_DEVICE_NUM

#define MAX_TIMERS_PAGE_ALIAS_SIZE	(8)
#define MAX_PAGE_TIMERS_NUM (6)


/* System time */
#define MAX_SYSTEM_TIME_SIZE	19
#define MAX_SYSTEM_TIMEZONE_SIZE	64

/* user code */
#define MAX_USERCODE_SIZE 36

/* astronomical time */
#define MAX_ASTRONOMICAL_DAY_SIZE 10
#define MAX_ASTRONOMICAL_SUNRISE_SIZE 8
#define MAX_ASTRONOMICAL_SUNSET_SIZE 8

/* Register */
#define MAX_CHANNEL_CODE	36

/* Battery */
#define MAX_BATTERY_LEVEL		10000

#define MOORGEN_PAYLOAD_LEN_MAX              (2048)

enum Retained {RETAINED_NULL};

typedef enum
{
	mqtt_unconnect=0,
	mqtt_connecting,
	mqtt_connected,
	mqtt_registering,
	mqtt_registered,
	mqtt_logining,
	mqtt_logined,
	mqtt_binding,
	mqtt_bound,
	mqtt_running,
	mqtt_deleteing

} mqtt_status_t;


typedef enum
{
	MQTT_MSGTYPE_ERR = 0,
	MQTT_MSGTYPE_REGISTER = 1,
	MQTT_MSGTYPE_ACTIVITE = MQTT_MSGTYPE_REGISTER,
	MQTT_MSGTYPE_LOGIN,
	MQTT_MSGTYPE_SLAVE_REPORT,
	MQTT_MSGTYPE_STATUS_REPORT,
	MQTT_MSGTYPE_CLOUD_STORAGE,
	MQTT_MSGTYPE_DEVICE_ONLINE,
	MQTT_MSGTYPE_DEVICE_OFFLINE,
	MQTT_MSGTYPE_COMMISSION_RESULT,
	MQTT_MSGTYPE_COMMAND_RESULT,
	MQTT_MSGTYPE_ERROR_REPORT,
	MQTT_MSGTYPE_TRANSPARENT_REPORT,
	MQTT_MSGTYPE_GET_STORAGE,
	MQTT_MSGTYPE_DATA_REPORT,
	MQTT_MSGTYPE_BIND_USER,
	MQTT_MSGTYPE_GET_SCENES,
	MQTT_MSGTYPE_GET_SINGLE_SCENE,
	MQTT_MSGTYPE_GET_TIMERS,
	MQTT_MSGTYPE_GET_SINGLE_TIMER,
	MQTT_MSGTYPE_GET_SYSTEM_TIME = 19,
	MQTT_MSGTYPE_GET_SCENES_CODE = 20,
	MQTT_MSGTYPE_GET_TIMERS_CODE,
	MQTT_MSGTYPE_GET_USER_CODE = 22,
	MQTT_MSGTYPE_CLIENT_TIMER_NOTIFY,
	MQTT_MSGTYPE_GET_ASTRONOMICAL_TIME,
	MQTT_MSGTYPE_RATE_PUSH_REPORT=26,
	MQTT_MSGTYPE_REQUEST_MAX,
	MQTT_MSGTYPE_SUBSCRIBE_START = 100,
	MQTT_MSGTYPE_REBOOT = MQTT_MSGTYPE_SUBSCRIBE_START,
	MQTT_MSGTYPE_RELOGIN,
	MQTT_MSGTYPE_RESET,
	MQTT_MSGTYPE_DEVICE_REPORT_SUBSCRIBE,
	MQTT_MSGTYPE_START_COMMISSION,
	MQTT_MSGTYPE_DELETE_SLAVE,
	MQTT_MSGTYPE_CLOSE_SLAVE,
	MQTT_MSGTYPE_OPEN_SLAVE,
	MQTT_MSGTYPE_CONFIGURATION,
	MQTT_MSGTYPE_UPDATE,
	MQTT_MSGTYPE_TRANSPARENT,
	MQTT_MSGTYPE_DISCONNECT,
	MQTT_MSGTYPE_DEVICE_CONTROL,
	MQTT_MSGTYPE_UPDATE_SCENE,
	MQTT_MSGTYPE_UPDATE_TIMER,
	MQTT_MSGTYPE_SCENE_CONTROL,
	MQTT_MSGTYPE_DEL_SCENE,
	MQTT_MSGTYPE_DEL_TIMER,
	MQTT_MSGTYPE_REGET_USER_CODE,
	MQTT_MSGTYPE_ASTRONOMICAL_TIME_GET,
	MQTT_MSGTYPE_UNBIND,
	MQTT_MSGTYPE_CLEAR_SCENE,
	MQTT_MSGTYPE_CLEAR_TIMER,
	MQTT_MSGTYPE_SUBSCRIBE_MAX
} mqtt_msgtype_t;


typedef enum
{
	COMMAND_NORMAL = 0,
	COMMAND_ERR_TIMEROUT,
	COMMAND_ERR_DEVICE_EXITED,
	COMMAND_ERR_SYSTEM,
	COMMAND_ERR_GENERATE_MAC_FAILED,
	COMMAND_ERR_NODE_UNFIND,
	COMMAND_ERR_REMOVE_NODE_FAILED,
	COMMAND_ERR_ADD_NODE_FAILED,
	COMMAND_ERR_SAVE_NODE_FAILED,
	COMMAND_ERR_PUSH_SLAVE_DEVICE_FAILED,
	COMMAND_ERR_PUSH_DATA_POINT_FAILED,
	COMMAND_ERR_PUSH_GATEWAY_STATUS_FAILED,
	COMMAND_ERR_PUSH_ONLINE_STATUS_FAILED,
	COMMAND_ERR_PUSH_COMMISSION_RESULT_FAILED,
	/*  OTA */
	COMMAND_ERR_OTA_CHECK_FAILURE,             //检查OTA失败
	COMMAND_ERR_OTA_NO_NEW_VERSION,           //检查发现无新版本
	COMMAND_ERR_OTA_UPDATE_FAILURE,            //更新失败
	COMMAND_ERR_OTA_MD5_FAIL,				// MD5校验失败
	COMMAND_ERR_OTA_NO_MEM,				// OTA空间不足
	COMMAND_ERR_OTA_NO_FILE,				// 未找到OTA镜像
	MAX_COMMAND_ERR_TYPE
} error_code_type_t;


#define START_COMMISSION    "104"
#define DEL_COMMISSION      "105"
#define DISABLE_COMMISSION  "106"
#define ENABLE_COMMISSION   "107"
#define UPDATE_FIRMWARE     "109"

#define COMMAND_RESULT_SUCCESS	1
#define COMMAND_RESULT_FAILED		0

#define ERR_CODE_TYPE     "errCode"
#define SLAVE_MAV_TYPE    "slaveMac"
#define SLAVE_DEVICE_TYPE  "deviceType"
#define SLAVE_FIRWARE_VER    "Version"



typedef struct
{
    int  lock;
	char actionType[8];
	int actionResult;	//0:failed  1:success
	char serverMsg[68];
	char deviceMsg[100];
} mqtt_command_result_t;


typedef struct _mqtt_service_t
{
    Network mqtt_N;
    Client mqtt_C;
	mg_queue_t mqtt_msg_send_queue;
	mg_queue_t mqtt_msg_recv_queue;
	mqtt_status_t  mqtt_status;
	int   diconnect_time;
	int   timeOut;
	int   keepaliveTime;
	int   keepaliveTimeout;

	mqtt_command_result_t mqtt_command_result;
} mqtt_service_t;


typedef struct
{
	int msgType;
	int content_size;
	unsigned char content_data[0];
} mqtt_packet_t, *p_mqtt_packet_t_t;




#define MQTT_TYPE_RETCODE           "retCode"
#define MQTT_TYPE_RETMSG            "retMsg"

#define REGISTER_TYPE_MAC               "mac"
#define REGISTER_TYPE_DEVICETYPE        "deviceType"
#define REGISTER_TYPE_CHANNEL_CODE      "channelCode"
#define REGISTER_TYPE_DEVICECODE        "deviceCode"



#define LOGIN_TYPE_FWVERSION            "fwVersion"
#define LOGIN_TYPE_DEVICE_SIGN_CODE     "deviceSignCode"
#define LOGIN_TYPE_BATTERY              "battery"
#define LOGIN_TYPE_SERIALNO             "serialNo"
#define LOGIN_TYPE_INTRANET             "intranet"
#define LOGIN_TYPE_SSID                 "ssid"
#define LOGIN_TYPE_ACCESSTOKEN          "accessToken"
#define LOGIN_TYPE_REFRESHTOKEN         "refreshToken"


#define SLAVE_REPORT_TYPE_MAC               "mac"
#define SLAVE_REPORT_TYPE_DEVICETYPE        "deviceType"
#define SLAVE_REPORT_TYPE_FWVERSION         "fwVersion"
#define SLAVE_REPORT_TYPE_DEVICESIGNCODE    "deviceSignCode"
#define SLAVE_REPORT_TYPE_BATTERY           "battery"
#define SLAVE_REPORT_TYPE_SERIALNO          "serialNo"


#define STATUS_REPORT_TYPE_MAC               "mac"
#define STATUS_REPORT_TYPE_DEVICETYPE        "deviceType"
#define STATUS_REPORT_TYPE_FWVERSION         "fwVersion"
#define STATUS_REPORT_TYPE_DEVICESIGNCODE    "deviceSignCode"
#define STATUS_REPORT_TYPE_BATTERY           "battery"
#define STATUS_REPORT_TYPE_SERIALNO          "serialNo"


#define CLOUD_STORAGE_TYPE_ATTRNAME     "attrName"
#define CLOUD_STORAGE_TYPE_ATTRVALUE    "attrValue"


#define DEVICE_ONLINE_TYPE_MAC              "mac"
#define DEVICE_ONLINE_TYPE_DEVICETYPE       "deviceType"
#define DEVICE_ONLINE_TYPE_FWVERSION        "fwVersion"
#define DEVICE_ONLINE_TYPE_DEVICESIGNCODE   "deviceSignCode"
#define DEVICE_ONLINE_TYPE_BATTERY          "battery"
#define DEVICE_ONLINE_TYPE_SERIALNO         "serialNo"


#define DEVICE_OFFLINE_TYPE_MAC              "mac"
#define DEVICE_OFFLINE_TYPE_DEVICETYPE       "deviceType"


#define COMMISSION_RESULT_TYPE_MAC              "mac"
#define COMMISSION_RESULT_TYPE_DEVICETYPE       "deviceType"
#define COMMISSION_RESULT_TYPE_FWVERSION        "fwVersion"
#define COMMISSION_RESULT_TYPE_BATTERY          "battery"
#define COMMISSION_RESULT_TYPE_SERIALNO         "serialNo"
#define COMMISSION_RESULT_TYPE_RESULT           "commissionResult"
#define COMMISSION_RESULT_TYPE_DEVICEMSG        "deviceMsg"



#define COMMAND_RESULT_TYPE_ACTION              "actionType"
#define COMMAND_RESULT_TYPE_RESULT              "actionResult"
#define COMMAND_RESULT_TYPE_SERVERMSG           "serverMsg"
#define COMMAND_RESULT_TYPE_DEVICEMSG           "deviceMsg"



#define ERROR_REPORT_TYPE_ERRCODE              "errCode"
#define ERROR_REPORT_TYPE_ERRMSG               "errMsg"



#define BIND_USER_TYPE_USER_ACCESSTOKEN       "userAccessToken"
#define BIND_USER_TYPE_TIMEZONE               "timeZone"
#define BIND_USER_KEYT                        "userCode"



#define SCENE_TYPE_CODE                     "sceneCode"
#define SCENE_TYPE_MODIFY_TIME              "modifyTime"
#define SCENE_TYPE_RULES                    "rules"
#define SCENE_TYPE_ARRAY                    "scenes"
#define SCENE_TYPE_PAGEALIAS                "pageAlias"
#define SCENE_TYPE_TOTAL_PAGE               "totalPage"
#define SCENE_TYPE_CURRENT_PAGE             "currentPage"


#define TIMER_TYPE_CODE                     "timerCode"
#define TIMER_TYPE_MODIFY_TIME              "modifyTime"
#define TIMER_TYPE_TRRIGGER_TIME            "triggerTime"
#define TIMER_TYPE_SCENECODE                "sceneCode"
#define TIMER_TYPE_REPEAT                   "repeat"
#define TIMER_TYPE_OFFSET                   "timeOffset"
#define TIMER_TYPE_DAYS                     "days"
#define TIMER_TYPE_TIME                     "time"
#define TIMER_TYPE_STATUS                   "timerStatus"
#define TIMER_TYPE_RULES                    "rules"
#define TIMER_TYPE_ARRAY                    "timers"
#define TIMER_TYPE_PAGEALIAS                "pageAlias"
#define TIMER_TYPE_TOTAL_PAGE               "totalPage"
#define TIMER_TYPE_TOTAL_CURRENT_PAGE       "currentPage"

#define SYSTEM_TIME_TYPE_TYPE_TIME           "time"
#define SYSTEM_TIME_TYPE_TIMEZONE           "timeZone"




#define ASTRONOMICAL_TIME_TYPE_DAY           "day"
#define ASTRONOMICAL_TIME_TYPE_SUNRISE       "sunrise"
#define ASTRONOMICAL_TIME_TYPE_SUNSET        "sunset"
#define ASTRONOMICAL_TIME_TYPE_SYSTIME       "sysTime"
#define ASTRONOMICAL_TIME_TYPE_DAYS          "days"
#define ASTRONOMICAL_TIME_TYPE_ASTIMES       "asTimes"



#define SUB_SLAVE_DEVICE_MAC                "slaveMac"
#define SUB_SLAVE_DEVICE_TYPE               "slaveType"
#define SUB_SLAVE_DEVICE_SERVER_MSG         "serverMsg"
#define SUB_SLAVE_DEVICE_CLIENT_MSG         "clientMsg"
#define SUB_SLAVE_DEVICE_CLIENT_MSG_TYPE    "type"



#define OTA_UPDATE_TYPE_URL                "url"
#define OTA_UPDATE_TYPE_MD5                "md5"
#define OTA_UPDATE_TYPE_MAC                "mac"
#define OTA_UPDATE_TYPE_DEVICETYPE         "deviceType"
#define OTA_UPDATE_TYPE_SERVERMSG          "serverMsg"
#define OTA_UPDATE_TYPE_RATE                "rateType"
#define OTA_UPDATE_TYPE_Value               "rateValue"



mqtt_packet_t *mqtt_packet_create(int msgType,int content_size,uint8_t  *content_data);
void mqtt_packet_release(mqtt_packet_t *mqtt_packet_p);
void mqtt_send_to_queue( mqtt_packet_t *mqtt_packet_send_p);
void mqtt_command_result_error_msg(const error_code_type_t errorCode, const char *slave_mac, const char *slave_type,const char *fw_version);
int  mqtt_command_result(char *actionType,	char *serverMsg);
void mqtt_command_result_req(int result);
int mqtt_status_get();
void mqtt_rate_push_report(char *mac, char *deviceType ,uint8_t value );


int mqtt_slave_report_req();
int mqtt_device_online_req(DEV_SUB_OBJECT *dev_sub_ob_p);
void mqtt_device_all_online_req( );
void mqtt_device_data_report_req(const char *contentData);
void mqtt_req_device_control(mqtt_packet_t *mqtt_packet_recv_p);
void mqtt_req_delete_slave(mqtt_packet_t *mqtt_packet_recv_p);
void mqtt_req_delete_slave(mqtt_packet_t *mqtt_packet_recv_p);
void mqtt_device_alldp_report();
void mqtt_req_start_commission(mqtt_packet_t *mqtt_packet_recv_p);


void mqtt_get_single_scene_req(char *secneCode);
void mqtt_get_single_scene_rsp(mqtt_packet_t *mqtt_packet_recv_p);
void mqtt_get_scene_codes_req();
int mqtt_get_scene_codes_rsp(mqtt_packet_t *mqtt_packet_recv_p);
void mqtt_req_reget_scene(mqtt_packet_t *mqtt_packet_recv_p);
void mqtt_req_scene_control(mqtt_packet_t *mqtt_packet_recv_p);
void mqtt_req_scene_del(mqtt_packet_t *mqtt_packet_recv_p);



void mqtt_get_single_timer_req(char *timerCode);
void mqtt_get_single_timer_rsp(mqtt_packet_t *mqtt_packet_recv_p);
void  mqtt_get_timers_codes_req();
int  mqtt_get_timers_codes_rsp(mqtt_packet_t *mqtt_packet_recv_p);
void mqtt_req_reget_time(mqtt_packet_t *mqtt_packet_recv_p);
void mqtt_req_timer_del(mqtt_packet_t *mqtt_packet_recv_p);
void mqtt_clienttime_notify_req(char *timerCode);
void mqtt_get_astronomical_time_req();
void mqtt_get_astronomical_time_rsp(mqtt_packet_t *mqtt_packet_recv_p);


void mqtt_ota_update_rsp(int result,char *version);
int mg_mqtt_service_init();


#endif


