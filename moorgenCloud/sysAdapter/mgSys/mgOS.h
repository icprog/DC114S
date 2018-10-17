#ifndef __MG_OS_H__
#define __MG_OS_H__

#include "mico.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mgEeprom.h"

#define mgos_log(M, ...) custom_log("mgos_log", M, ##__VA_ARGS__)


#define MG_APPLICATION_PRIORITY         (7)
#define MG_WAIT_FOREVER    MICO_WAIT_FOREVER

#define SYS_KEY1	MICO_GPIO_9
#define SYS_KEY2	MICO_GPIO_10


#define IsTopStaus		SYS_KEY1
#define IsBottomStaus	SYS_KEY2

#define SwitchMotorDelay	500

#define GainKey1Status()	MicoGpioInputGet(MICO_GPIO_9)
#define GainKey2Status()	MicoGpioInputGet(MICO_GPIO_10)


typedef enum 
{
	isforward,
	isreversal,
	isstop,	
}MotorDoStatus_t;

typedef struct Certificate_config{
	uint32_t datasize;
	char certificate[0];
}Certificate_config_t;

 
typedef struct tagMG_TIME_DATA{
    uint16_t  year;
    uint8_t   month;
    uint8_t   week;  /*1:monday ... 7:sunday*/
    uint8_t   day;
    uint8_t   hour;
    uint8_t   minute;
    uint8_t   second;
}MG_TIME_DATA;



typedef enum
{
	MG_WIFI_READY=0,
	MG_WIFI_STA_START,
	MG_WIFI_STA_STOP,
	MG_WIFI_STA_CONNECTED,
	MG_WIFI_STA_DISCONNECTED,
	MG_WIFI_AP_START,
	MG_WIFI_AP_STOP,
	MG_WIFI_AP_STACONNECTED,
	MG_WIFI_AP_STADISCONNECTED,
	MG_WIFI_MAX
	
} mg_wifi_status_t;



typedef  mico_thread_arg_t mg_thread_arg_t;
typedef  mico_thread_t     mg_thread_t;
typedef  mico_queue_t      mg_queue_t;
typedef  mico_timer_t      mg_timer_t;
typedef  mico_semaphore_t  mg_semaphore_t;
typedef  mico_mutex_t      mg_mutex_t;


#define  mg_rtos_create_thread      mico_rtos_create_thread
#define  mg_rtos_thread_sleep       mico_rtos_thread_sleep
#define  mg_rtos_thread_msleep      mico_rtos_thread_msleep
#define  mg_rtos_delete_thread      mico_rtos_delete_thread

#define  mg_rtos_init_queue         mico_rtos_init_queue
#define  mg_rtos_is_queue_full      mico_rtos_is_queue_full
#define  mg_rtos_is_queue_empty     mico_rtos_is_queue_empty
#define  mg_rtos_pop_from_queue     mico_rtos_pop_from_queue
#define  mg_rtos_push_to_queue      mico_rtos_push_to_queue


#define  mg_rtos_init_semaphore     mico_rtos_init_semaphore
#define  mg_rtos_deinit_semaphore   mico_rtos_deinit_semaphore
#define  mg_rtos_get_semaphore      mico_rtos_get_semaphore
#define  mg_rtos_set_semaphore      mico_rtos_set_semaphore

#define  mg_rtos_init_mutex         mico_rtos_init_mutex
#define  mg_rtos_lock_mutex         mico_rtos_lock_mutex
#define  mg_rtos_unlock_mutex       mico_rtos_unlock_mutex

#define  mg_init_timer  mico_init_timer
#define  mg_start_timer mico_start_timer
#define  mg_stop_timer  mico_stop_timer

int mg_ota_begin();
int mg_ota_flash_write(uint32_t* off_set,uint8_t *filebuf,int filelength);
int mg_ota_end(uint32_t filelength,uint8_t* md5);


void time_to_mgTime(char *mqttTime , MG_TIME_DATA *mgTime);
void mg_rtc_get_time(MG_TIME_DATA *mgTime);
void mg_rtc_set_time(MG_TIME_DATA *mgTime);
void mg_os_reboot();


void mg_wifi_config_suc(char *ssid, char *password);
void mg_wifi_mac_get(char *macStr);
void mg_wifi_info_get(char *ssid, char *ip , char *bssid);
int mg_wifi_status_get();
int mg_freemem_get();



void mgAesEncrypt(uint8_t *in,uint8_t *out,uint8_t *key,int length);
void  mgAesDecrypt(uint8_t *in,uint8_t*out,uint8_t *key, int length);

/*add component*/
void MotorExe(uint8_t DoDirection , MotorDoStatus_t  Doctrl);
uint8_t GainControlMode(void);
void Key_GPIO_Init(void);

Certificate_config_t * Certificate_ReadFlash(void);
void Certificate_WriteFlash(char *data);

int mg_os_init();
void  mg_wifi_start();

#endif
