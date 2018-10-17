#ifndef _MG_KERNEL_H_
#define _MG_KERNEL_H_

#include <stdint.h>
#include "mgOS.h"
#include "mgDMG.h"


#define MG_DEVICE_MAX   30
#define MG_SCENE_MAX    20
#define MG_TIMER_MAX    20

#define MG_CODE_SIZE  40
#define MG_DEV_CHARACT_MAX 4
#define MG_DEVICE_TYPE_SIZE 12
#define MG_ASTRONOMICAL_TIME_MAX 30
#define MG_DEV_MAC_MAX 24
#define MG_DEV_DP_MAX  28
#define MG_DEV_SUB_FW_SIZE 16

#define MG_MASTER_ID   0xaa


#define mg_log(M, ...) custom_log("mg_kernel", M, ##__VA_ARGS__)




typedef struct tagMG_KERNEL_DATA{

    char     magicCode[8];
	uint16_t sub_mac_count;	// 子设备MAC累加数
    uint32_t  devBit;
    uint32_t  sceneBit;
    uint32_t  timerBit;

}MG_KERNEL_DATA;

typedef enum
{
    DEVICE_MASTER=0,
    DEVICE_SUB=1,

}DEVICE_TYPE;


typedef struct tagDEV_SUB_OBJECT{
    uint8_t  devID;
    uint8_t  fID;
    uint8_t  rfAddr[4];
    uint8_t  key;
    uint8_t  res;
    char     firmware[MG_DEV_SUB_FW_SIZE];
    char     devMac[MG_DEV_MAC_MAX];
    char     devType[MG_DEVICE_TYPE_SIZE];
    uint8_t  devDP[MG_DEV_DP_MAX];

}DEV_SUB_OBJECT;


typedef struct tagDEV_OPS{

    void  (*dev_op_dp_ctrl)(DEVICE_TYPE type ,void *arg,void *dp);
    void  (*dev_op_dp_report)(DEVICE_TYPE type ,void *arg);
    void  (*dev_op_del)(DEVICE_TYPE type ,void *arg);
    void  (*dev_op_start_pair)(char *slaveMac,char *slaveType,char *clientMsg);
    void  (*dev_op_pair_result)(int result,DEV_SUB_OBJECT *dev_sub_ob_p );
    void  (*dev_op_dp_to_json)(char  *devType,void *dp,void *json);
    void  (*dev_op_dp_to_struct)(char  *devType ,void *dp,void *json);

}DEV_OPS;


typedef struct tagDEV_MASTER_OBJEC{


    char     devMac[MG_DEV_MAC_MAX];
    char     devType[MG_DEVICE_TYPE_SIZE];
    uint8_t  hub_flag;
    uint8_t  pair_flag;
    uint16_t res;
    DEV_OPS  dev_ops;
    uint8_t  devDP[MG_DEV_DP_MAX];

}DEV_MASTER_OBJEC;



typedef struct tagSCENE_EVENT_ITEM{
    uint8_t    devID;
    uint8_t    devDP[MG_DEV_DP_MAX+3];

}DEV_ITEM;

typedef struct tagSCENE_HEAD{
    uint8_t  sceneID;
    uint8_t  flag;
    uint16_t res2;
    char  sceneCode[MG_CODE_SIZE];
    MG_TIME_DATA modifyTime;
}SCENE_HEAD;

typedef struct tagSCENE_OBJECT{
    SCENE_HEAD  head;
    DEV_ITEM    devItem[MG_DEVICE_MAX];
}SCENE_OBJECT;


typedef enum
{
    TIMER_SUNRISE=0,
    TIMER_SUNSET =1,
    TIMER_NORMAL=2,

}TIMER_TYPE;

typedef enum
{
    REPEAT_MONTH=0,
    REPEAT_WEEK=1,
    REPEAT_NONE =2

}REPEAT_TYPE;

typedef enum
{
    TIMER_DEVICE=0,
    TIMER_SCENE=1,
}TIMER_EVENT_TYPE;

typedef struct tagTIMER_HEAD{
    uint8_t  timerID;
    uint8_t  repeatType;
    uint16_t repeatDays;
    uint8_t  timerType;
    uint8_t  timerStatus;
    uint8_t  timerEventType;
    uint8_t  flag;
    int      timerOffset;
    MG_TIME_DATA timerTime;
    char  timerCode[MG_CODE_SIZE];
    MG_TIME_DATA modifyTime;
}TIMER_HEAD;

typedef struct tagTIMER_OBJECT{
    TIMER_HEAD  head;
    union{
        DEV_ITEM    devItem[MG_DEVICE_MAX];
        char        sceneCode[MG_CODE_SIZE];
    } item;
}TIMER_OBJECT;

typedef struct SUN_TIME{
    uint8_t date;
    uint8_t month;

}SUN_TIME;

typedef struct tagASTRONOMICAL_TIME{
    uint16_t year;
    uint8_t day;
    uint8_t month;
    uint8_t sunrise_hr;
    uint8_t sunrise_min;
    uint8_t sunset_hr;
    uint8_t sunset_min;

}ASTRONOMICAL_TIME;


extern DEV_MASTER_OBJEC   *dev_master_ob_p;

void mg_kernel_scenebit_set(uint32_t scenebit);
uint32_t mg_kernel_scenebit_get();
void mg_kernel_devbit_set(uint32_t devbit);
uint32_t mg_kernel_devbit_get();
void mg_kernel_timerbit_set(uint32_t timerbit);
uint32_t mg_kernel_timerbit_get();
uint16_t mg_kernel_mac_count_get();
void mg_kernel_all_reset();



SCENE_HEAD *scene_object_search(char *sceneCode);
int scene_object_add(SCENE_OBJECT *scene_ob_p );
int scene_object_mod(SCENE_OBJECT *scene_ob_p );
void scene_object_del(uint8_t sceneID );
void scene_object_exe(char  *sceneCode);
void scene_object_item_del(uint8_t    devID);
void scene_object_all_reset();
SCENE_OBJECT *scene_object_next(uint8_t sceneID);
void mg_scene_init();


TIMER_HEAD *timer_object_search(char *timerCode);
int timer_object_add( TIMER_OBJECT *timer_ob_p  );
int timer_object_mod( TIMER_OBJECT *timer_ob_p  );
void timer_object_del(uint8_t timerID );
TIMER_OBJECT *timer_object_next(uint8_t timerID);
void timer_object_dev_item_del(uint8_t devID);
void timer_object_all_reset();
void timer_mqtt_exe_set( );
void timer_mqtt_exe_reset( );
void astronomical_time_set(ASTRONOMICAL_TIME *astime_p,int index,int MaxIndex);
void astronomical_today_flush(MG_TIME_DATA *curTime);

void mg_timer_init();



DEV_SUB_OBJECT *device_sub_object_get(uint8_t devID);
DEV_SUB_OBJECT *device_sub_object_search(char *mac);
int device_sub_object_inuse_get();
int device_sub_object_add(DEV_SUB_OBJECT *dev_sub_ob_p);
int device_sub_object_mod(DEV_SUB_OBJECT *dev_sub_ob_p );
void device_sub_object_del(DEV_SUB_OBJECT *dev_sub_ob_p);
void device_sub_object_all_reset();
DEV_SUB_OBJECT *device_sub_object_next(uint8_t devID);
void mg_device_init();
void  devive_dp_report(DEVICE_TYPE type ,void *arg );
int device_hub_check();
void device_master_mac_set(char *mac);
void device_master_init(DEV_MASTER_OBJEC  *dev_master_p);


void mg_kernel_init();

#endif
