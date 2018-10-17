#include "mgOS.h"

//static int wifi_status;
extern char contrmode[40];
uint8_t RunStatus = 0;

int mg_ota_begin()
{
    mico_logic_partition_t* ota_partition = MicoFlashGetInfo( MICO_PARTITION_OTA_TEMP );

    if(MicoFlashErase(MICO_PARTITION_OTA_TEMP,0,ota_partition->partition_length)!=kNoErr)
        return -1;

    return 0;
}


int mg_ota_flash_write(uint32_t* off_set,uint8_t *filebuf,int filelength)
{

    if (MicoFlashWrite(MICO_PARTITION_OTA_TEMP,off_set, (uint8_t*)filebuf,filelength)!=kNoErr)
        return -1;

    return 0;

}

int mg_ota_end(uint32_t filelength,uint8_t* md5)
{
    uint32_t  flashaddr;
    int left,len;
    md5_context ctx;
    uint16_t crc = 0;
    CRC16_Context contex;
    uint8_t *filebuf=NULL;
    uint8_t md5_calc[16];
    int ret=0;

    filebuf=(uint8_t *)malloc(1024);	//md5 计算校验需要的内存空间
    if(NULL==filebuf)
        return -1;
    
    InitMd5( &ctx );
    CRC16_Init( &contex );

    flashaddr = 0;
    left = filelength;
    while(left > 0) 
    {
        if (left > 1024) {		//>1024, 分批计算
            len = 1024;
        } else {
            len = left;
        }
        left -= len;
        MicoFlashRead(MICO_PARTITION_OTA_TEMP, &flashaddr, (uint8_t *)filebuf, len);//从flash 的OTA 分区读出部分下载的数据
        Md5Update( &ctx, (uint8_t *)filebuf, len);				//更新md5要处理的缓冲区	数据													
        CRC16_Update( &contex, filebuf, len );				//更新CRC16 要处理的缓冲区数据

    }

    Md5Final( &ctx, md5_calc );							//md5 校验计算
    CRC16_Final( &contex, &crc );						//CRC16校验计算

    if(memcmp(md5_calc, md5, 16) != 0) {				//计算得到的md5 结果跟 http中获取的md5 值做比对
        ret=-1;;
    }
    free(filebuf);

    if(ret==0)											//对比无误
        mico_ota_switch_to_new_fw( filelength, crc );		//切换新的固件(即下载的固件)


    return ret;
}

void mg_os_reboot()
{
    MicoSystemReboot();

}

#define DATE_FORMAT "%Y-%m-%d %H:%M:%S"

void time_to_mgTime(char *mqttTime , MG_TIME_DATA *mgTime)
{
	struct tm time;
	strptime(mqttTime, DATE_FORMAT, &time);

//    mgos_log("time_to_mgTime");

 //   mgos_log("%d-%d-%d %d:%d:%d %d ",time.tm_year,time.tm_mon,time.tm_mday,time.tm_hour,time.tm_min,time.tm_sec,time.tm_wday);

	mgTime->second= time.tm_sec;
	mgTime->minute= time.tm_min;
	mgTime->hour= time.tm_hour;
	mgTime->day= time.tm_mday;
	mgTime->month = time.tm_mon+1;
	mgTime->year = time.tm_year+1900;
	if(time.tm_wday==0)
	    mgTime->week = 7;
	else
	    mgTime->week = time.tm_wday;


	

}


void mg_rtc_set_time(MG_TIME_DATA *mgTime)
{
    mico_rtc_time_t  systime;

    systime.year=mgTime->year-2000;
    systime.month=mgTime->month;
    systime.hr=mgTime->hour;
    systime.date=mgTime->day;
    systime.min=mgTime->minute;
    systime.sec=mgTime->second;

   // mgos_log("mg_rtc_set_time");
   // mgos_log("%d-%d-%d %d:%d:%d  %d",mgTime->year,mgTime->month,mgTime->day,mgTime->hour,mgTime->minute,mgTime->second,mgTime->week);

    
    if(mgTime->week==7)
        systime.weekday=0;
    else
        systime.weekday=mgTime->week;
    //mgos_log("%d-%d-%d %d:%d:%d %d ",systime.year,systime.month,systime.date,systime.hr,systime.min,systime.sec,systime.weekday);


    MicoRtcSetTime(&systime);
}


void mg_rtc_get_time(MG_TIME_DATA *mgTime)
{
    mico_rtc_time_t  systime;
    MicoRtcGetTime(&systime);
    char timeStr[32];

   // mgos_log("mg_rtc_get_time");

    //mgos_log("%d-%d-%d %d:%d:%d %d ",systime.year,systime.month,systime.date,systime.hr,systime.min,systime.sec,systime.weekday);

    sprintf(timeStr,"%d-%d-%d %d:%d:%d",systime.year+2000,systime.month,systime.date,systime.hr,systime.min,systime.sec);

    time_to_mgTime(timeStr,mgTime);

/*
    mgTime->year=systime.year+2000;
    mgTime->month=systime.month;
    mgTime->hour=systime.hr;
    mgTime->day=systime.date;
    mgTime->minute=systime.min;
    mgTime->second=systime.sec;


    if(systime.weekday==0)
        mgTime->week=7;
    else
        mgTime->week=systime.weekday;
*/
}





int mg_freemem_get()
{
    return MicoGetMemoryInfo()->free_memory;
}



void mg_wifi_mac_get(char *macStr)
{
    unsigned char mac[8] = {0};
    mgos_log("mg_wifi_mac_get");

    wlan_get_mac_address(mac);
    sprintf(macStr, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    mgos_log("macStr:%s",macStr);

}
void Certificate_WriteFlash(char *data)
{
	mico_Context_t *context = NULL;
	Certificate_config_t *certificate_msg = NULL;

	context = mico_system_context_get();
	certificate_msg = mico_system_context_get_user_data(context);
	strncpy(certificate_msg->certificate, data, strlen(data));
	certificate_msg->datasize = sizeof(data);
	mico_system_context_update(context);
}

Certificate_config_t * Certificate_ReadFlash(void)
{
	mico_Context_t *context = NULL;
	Certificate_config_t *certificate_msg = NULL;

	context = mico_system_context_get();
	certificate_msg = mico_system_context_get_user_data(context);

	return certificate_msg;
}
void Key_GPIO_Init(void)
{
	MicoGpioInitialize(SYS_KEY1,INPUT_PULL_UP);
	MicoGpioInitialize(SYS_KEY2, INPUT_PULL_UP);	
}
static void DriverMotor_GPIO_Init(void)
{
	MicoGpioInitialize(MICO_GPIO_13, OUTPUT_PUSH_PULL);
	MicoGpioInitialize(MICO_GPIO_14, OUTPUT_PUSH_PULL);

	MicoGpioOutputLow(MICO_GPIO_13);
	MicoGpioOutputLow(MICO_GPIO_14);
}

void MotorExe(uint8_t DoDirection , MotorDoStatus_t  Doctrl)
{
	if(DoDirection == 0)
	{
		if(Doctrl == isforward)
		{
			MicoGpioOutputLow(MICO_GPIO_13);
			mico_rtos_delay_milliseconds(SwitchMotorDelay);
			MicoGpioOutputHigh(MICO_GPIO_14);
			RunStatus = 1;
		}
		else if(Doctrl == isreversal)
		{
			MicoGpioOutputLow(MICO_GPIO_14);
			mico_rtos_delay_milliseconds(SwitchMotorDelay);
			MicoGpioOutputHigh(MICO_GPIO_13);
			RunStatus = 2;
		}
		else		//isStop
		{
			MicoGpioOutputLow(MICO_GPIO_13);
			MicoGpioOutputLow(MICO_GPIO_14);
			RunStatus = 0;
		}
	}
	else			//change direction	
	{
		if(Doctrl == isforward)
		{
			MicoGpioOutputLow(MICO_GPIO_14);
			mico_rtos_delay_milliseconds(SwitchMotorDelay);
			MicoGpioOutputHigh(MICO_GPIO_13);
			RunStatus = 1;
		}
		else if(Doctrl == isreversal)
		{
			MicoGpioOutputLow(MICO_GPIO_13);
			mico_rtos_delay_milliseconds(SwitchMotorDelay);
			MicoGpioOutputHigh(MICO_GPIO_14);
			RunStatus =2;
		}
		else		//isStop
		{
			MicoGpioOutputLow(MICO_GPIO_13);
			MicoGpioOutputLow(MICO_GPIO_14);
			RunStatus =0;
		}
	}
	
}

uint8_t GainControlMode(void)
{
	uint8_t controlMode  = 0;
	
	if(strstr(contrmode,"controlMode\":0") != 0)
	{
		//mgos_log("controlMode  0\r\n");
		controlMode = 0;
	}
	else if(strstr(contrmode,"controlMode\":1") != 0)
	{
		//mgos_log("controlMode  1\r\n");
		controlMode = 1;
	}
	else if(strstr(contrmode,"controlMode\":2") != 0)
	{
		//mgos_log("controlMode  2\r\n");
		controlMode = 2;
	}
	else if(strstr(contrmode,"controlMode\":") != 0)
	{
		controlMode = 0xff;			//invalid
	}
	else			
	{
		//mgos_log("find err\r\n");
		controlMode = 0;				//default   set
	}
	
	return controlMode;
}
int mg_os_init()
{
    MicoRtcInitialize();
    Key_GPIO_Init();
    DriverMotor_GPIO_Init();
    if(mg_eeprom_init()!=kNoErr)
    {
        mgos_log("mg_eeprom_init error");
        goto ERROR;
    }
    return 0;
	
ERROR:
    return -1;
}
