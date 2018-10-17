//=============================================================================
//  移植步骤:
//	1.在Test_AddItems(&items_json_object): 增加测试项目名称和内容
//  2.在void result_Report(struct json_object *send_json_object， int result): 设置用户测试结果上报内容
//  3.用户编写自己的测试函数：格式: int xxx(json_object *ob)
//  4.在factoryTest.h中设置每个项目在ItemBits 变量对应位
//  5.调用void FactoryTest(void)进行测试: 并调用usrFunction_register()函数注册测试函数和更新
//										      项目总数Item_total
//	PS:  最终只要调用void FactoryTest(void)即可
//=============================================================================
#include "factoryTest.h"
#include "moorgenService.h"

#define factest_log(M, ...) custom_log("factest_log", M, ##__VA_ARGS__)

//=============================================================================
//  用户配置区
//=============================================================================
#define FacTestLED_Tri_INTERVAL		300
//AP ssid , psswd
const char *factory_SSID = "TEST-AP1";					//TEST-AP1		
const char *factory_Password = "9876543210";				//9876543210

//=============================================================================
//  系统默认设置(无需配置，更改)
//=============================================================================
_usrFunc_list_t* usrFunc_list;
	
static mg_semaphore_t factory_stationUp_sem = NULL;
mico_semaphore_t endFactoryTest_sem = NULL;

static int udp_fd = -1;
uint8_t Wifitry_cnt = 0;
uint8_t IsReport = 0;
uint8_t notifi_endfactoryTest = 0;   //通知FactoryTest_thread 厂测进程是否结束
uint32_t ItemBits = 0;		//bit0: 预留 bit1 ~bit31 : 标志对应项目是否已经测试	
uint8_t Item_total = 0;	//记录总共测试项目数目, 目前最大测试31个项目
/* factory test config */
static system_context_t  *factoryinContext;

static LinkStatusTypeDef wifiLinkStatus;

static mico_timer_t _Led_EL_timer;
static bool _Led_EL_timer_initialized = false;

/* Factory Acceptance Test */
#define FactorTest_STK_SIZE		2048
mico_thread_t* FactoryTestHandle;
static void FactoryTest_thread(mico_thread_arg_t arg);


/* Factory Key Test thread */
#ifdef defaultKey_test
	#define FacKeyTest_STK_SIZE 	1024
	mico_thread_t* FacKeyTestHandle;
	static void FacKeyTest_thread( mico_thread_arg_t  arg );
#endif

/* Monitor whether all tests are completed */
#define MonitorAllCompleted_STK_SIZE  512
mico_thread_t* MonitorAllCompleteHandle;
static void MonitorAllComplete_thread(mico_thread_arg_t arg);

/* After factory test is completed, turn on the timer and exit after 20 seconds.*/
mico_timer_t endTimerHandler ;
void endFuncCallBack(void *arg);

static int Do_TestItem(char *recvbuf);
static void FactoryWifi_Start(const char *ssid, const char *password);

int usrFunction_register(void *functionAddress, void *arg);
void Test_AddItems(struct json_object *items_json_object);
void result_Report(struct json_object *send_json_object, int result);

void FacTest_LedFlash(void);
void FacTest_LedOff(void);

//=============================================================================
//  1.用户设置 “测试项目名称”
//=============================================================================
void Test_AddItems(struct json_object *items_json_object)
{
	/* 根据用户需求，增加测试项目名称和内容 */
	json_object_object_add(items_json_object, "T-Wifi", json_object_new_string("rssi"));
	json_object_object_add(items_json_object, "T-Motor", json_object_new_string("rotation"));
	json_object_object_add(items_json_object, "T-Eeprom", json_object_new_string("read-write"));
	json_object_object_add(items_json_object, "T-Key", json_object_new_string("2"));
}
//=============================================================================
// 2.测试结果上报
//=============================================================================
void result_Report(struct json_object *send_json_object,  int result)
{
	/* 项目测试结果上报: */
	switch(result)
	{
		case MotorNoErr:	
				factest_log("motor OK");
				IsReport = 1;
				json_object_object_add(send_json_object, "Motor", json_object_new_string("Success"));
				break;
		case EepromNoErr:
				factest_log("eeprom OK");
				IsReport = 1;
				json_object_object_add(send_json_object, "Eeprom", json_object_new_string("Success"));
				break;
		case WifiRssiNoErr:  	
				factest_log("wifi OK");
				IsReport = 1;
				json_object_object_add(send_json_object, "Wifi", json_object_new_int(wifiLinkStatus.rssi));
				break;
		case KeyPressNoErr:
				IsReport = 0;		
				break;
		case MotorErr:	
				factest_log("motor err");
				IsReport = 1;
				json_object_object_add(send_json_object, "Motor", json_object_new_string("Error"));
				break;
		case EepromErr:
				factest_log("eeprom err");
				IsReport = 1;
				json_object_object_add(send_json_object, "Eeprom", json_object_new_string("Error"));
				break;
		case WifiRssiErr:	
				factest_log("wifi err");
				IsReport = 1;
				json_object_object_add(send_json_object, "Wifi", json_object_new_int(0));
				break;
		default : 
				factest_log("test Invalid");
				IsReport = 1;
				json_object_object_add(send_json_object, FactoryTest_CODE, json_object_new_int(Invalid));
				break;
	}
}
//=============================================================================
//  3.用户编写测试函数  格式:  uint8_t xxx_test(json_object *ob)
//=============================================================================
int Motor_test(json_object *ob)
{
	if(strcmp(json_object_get_string(ob), "T-Motor") == 0)	
	{
		//motor test
		factest_log("motor test");
		ItemBits |= (1<<MotorBit);
		MotorExe(0, isforward);		//isforward
		mico_rtos_delay_milliseconds(3000);
		MotorExe(0,isreversal);
		mico_rtos_delay_milliseconds(3000);
		MotorExe(0,isstop);
		return MotorNoErr;
	}
	return Invalid;
}

int WifiRssi_test(json_object *ob)
{
	if(strcmp(json_object_get_string(ob), "T-Wifi") == 0)
	{
		//wifi rssi test
		factest_log("wifi test");
		ItemBits |= (1<<WifiRssiBit);
		memset(&wifiLinkStatus,0x0,sizeof(wifiLinkStatus));
		if(micoWlanGetLinkStatus(&wifiLinkStatus) == kNoErr)
		{
			return WifiRssiNoErr;
		 }
		return WifiRssiErr;
	}
	return Invalid;
}

int Eeprom_test(json_object *ob)
{
	OSStatus err;
	mico_i2c_device_t at24c_i2c_test = {
  		AT24C_I2C_DEVICE, AT24C_I2C_ADDR, I2C_ADDRESS_WIDTH_7BIT, I2C_STANDARD_SPEED_MODE
	};
	if(strcmp(json_object_get_string(ob), "T-Eeprom") == 0)
	{
		factest_log("eeprom test");
		ItemBits |= (1<<EepromBit);
		err = MicoI2cProbeDevice(&at24c_i2c_test, 5);
		if(err)	
			return EepromNoErr;
		else
			return EepromErr;
	}
	return Invalid;
}

int Key_test(json_object *ob)
{
	if(strcmp(json_object_get_string(ob), "T-Key") == 0)
	{
		//不做任何处理
		factest_log("key test");
		return KeyPressNoErr;
	}
	return Invalid;
}
//=============================================================================
//  4.开始测试
//=============================================================================
void FactoryTest(void)
{
	if(MicoGpioInputGet(SYS_KEY1) == false && (MicoGpioInputGet(EasyLink_BUTTON) == false))
	{
		/**************用户测试函数注册和项目总数设置***************/
		usrFunction_register((void *)Motor_test, NULL);
		usrFunction_register((void *)WifiRssi_test, NULL);
		usrFunction_register((void *)Eeprom_test, NULL);
		usrFunction_register((void *)Key_test, NULL);

		Item_total = 5; //测试项目总数: 这里为5 的原因: 此环境中有2个按键
		/*********************end******************************/
		mico_rtos_init_semaphore(&endFactoryTest_sem,1);
		factest_log("*****Factory test*****");
		FacTest_LedFlash();	//led  indicate
		FactoryWifi_Start(factory_SSID,factory_Password);
		mico_rtos_get_semaphore( &endFactoryTest_sem, MICO_WAIT_FOREVER );
		mico_rtos_deinit_semaphore(&endFactoryTest_sem);		//deinitialization  semaphore
		FacTest_LedOff();	//close led
	}
}
//================================分割符(测试配置完成)==========================//

//========================================================================
//  wifi状态改变通知
//========================================================================
static void factoryNotify_WifiStatusHandler( WiFiEvent status, void* const inContext )
{
    switch ( status )
    {
        case NOTIFY_STATION_UP:
            mico_rtos_set_semaphore( &factory_stationUp_sem );
            break;
        case NOTIFY_STATION_DOWN:
        case NOTIFY_AP_UP:
        case NOTIFY_AP_DOWN:
            break;
    }
}
//========================================================================
//  连接测试AP， 如 ssid : "test3"   password : "continue"
//========================================================================
static void FactoryWifi_Start(const char *ssid, const char *password)
{
	
	factoryinContext =system_context();

	mg_rtos_init_semaphore(&factory_stationUp_sem,1);
	
	factest_log("Factory_wifi_start");

	mico_system_notify_register( mico_notify_WIFI_STATUS_CHANGED, (void *)factoryNotify_WifiStatusHandler, (void *) factoryinContext );
	mico_rtos_lock_mutex( &factoryinContext->flashContentInRam_mutex );
	factoryinContext->flashContentInRam.micoSystemConfig.easyLinkByPass = EASYLINK_BYPASS_NO;
	//ssid
	strncpy(factoryinContext->flashContentInRam.micoSystemConfig.ssid, ssid, 32);
	factoryinContext->flashContentInRam.micoSystemConfig.channel = 0;
	memset( factoryinContext->flashContentInRam.micoSystemConfig.bssid, 0x0, 6 );
	factoryinContext->flashContentInRam.micoSystemConfig.security = SECURITY_TYPE_AUTO;
	//password
	strncpy( factoryinContext->flashContentInRam.micoSystemConfig.key,password, 64);
	strncpy( factoryinContext->flashContentInRam.micoSystemConfig.user_key, password, 64 );
	factoryinContext->flashContentInRam.micoSystemConfig.keyLength = strlen( factoryinContext->flashContentInRam.micoSystemConfig.key );
	factoryinContext->flashContentInRam.micoSystemConfig.user_keyLength = strlen( factoryinContext->flashContentInRam.micoSystemConfig.key );
	factoryinContext->flashContentInRam.micoSystemConfig.dhcpEnable = true;
	mico_rtos_unlock_mutex( &factoryinContext->flashContentInRam_mutex );
	system_connect_wifi_fast(factoryinContext );	//wifi  connecting
	while(mico_rtos_get_semaphore( &factory_stationUp_sem, 1000 ) && (Wifitry_cnt <5));
//	mico_rtos_get_semaphore( &factory_stationUp_sem, MG_WAIT_FOREVER ); 
	if(Wifitry_cnt >=5)
	{
//		factoryinContext->flashContentInRam.micoSystemConfig.configured = unConfigured;
		mico_rtos_set_semaphore(&endFactoryTest_sem);
		return;
	}
	if(factory_stationUp_sem != NULL )
   	{
		mico_rtos_deinit_semaphore( &factory_stationUp_sem );
		factory_stationUp_sem=NULL;
	} 
	factest_log("Factorytest_udp_start");
	/* Start local server listener thread*/
	mico_rtos_create_thread(FactoryTestHandle, MICO_APPLICATION_PRIORITY, "factoryTest_thread", FactoryTest_thread, FactorTest_STK_SIZE,  (mico_thread_arg_t)0 );
}
//========================================================================
//  UDP接收测试指令并进行相关处理
//========================================================================
static void FactoryTest_thread(mico_thread_arg_t arg)
{
	OSStatus err;
	struct sockaddr_in addr, remoteaddr;
	fd_set readfds;
	socklen_t addrLen = sizeof(addr);
	int  len;	//udp_fd = -1,
	char *buf = NULL;
	char *sendbuf =NULL;
	 const char *sendData;
	char MACmsg[20] = {0};
	int errcode = Invalid;
	struct json_object *send_json_object = NULL;
	struct json_object *start_json_object = NULL;
	struct json_object *items_json_object = NULL;

	mg_wifi_mac_get(MACmsg); 
				
	buf = malloc(100);
	sendbuf = malloc(100);
	require_action(buf||sendbuf,exit, err = kNoMemoryErr);
	
	/*Establish a UDP port to receive any data sent to this port*/
	udp_fd = socket(AF_INET,SOCK_DGRAM, IPPROTO_UDP);
	require_action(IsValidSocket(udp_fd), exit, err = kNoResourcesErr);

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;			
	addr.sin_port = htons(FactoryTest_LocalPORT);	
	err = bind(udp_fd, (struct sockaddr *)&addr, sizeof(addr));
	require_noerr(err, exit);
	
	factest_log("Open local UDP port %d", FactoryTest_LocalPORT);

	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr =inet_addr(FactoryTest_RemoIP);
	remoteaddr.sin_port = htons(FactoryTest_RemoPORT);		

	send_json_object = json_object_new_object();
	start_json_object = json_object_new_object();
	items_json_object = json_object_new_object();

	/* 1. send deviceType and mac etc */
	json_object_object_add(start_json_object,"DeviceType",json_object_new_string(CONFIG_DEVICE_TYPE));
	json_object_object_add(start_json_object,"Mac",json_object_new_string(MACmsg));
	json_object_object_add(start_json_object,"Factory",json_object_new_string(CONFIG_FACTORY));
	json_object_object_add(start_json_object,"Version",json_object_new_string(CONFIG_VERSION));
	sendData= json_object_to_json_string(start_json_object);
	memcpy(sendbuf,sendData,strlen(sendData));
	sendto( udp_fd, sendbuf, strlen(sendData), 0, (struct sockaddr *)&remoteaddr, sizeof(struct sockaddr_in) );
	json_object_put(start_json_object);
	mico_rtos_delay_milliseconds(3000);
	/* 2. set test items */
	Test_AddItems(items_json_object);
	/* 3. package send buffer */
	sendData = json_object_to_json_string(items_json_object);
	memcpy(sendbuf,sendData,strlen(sendData));
	/* 4. send to AP  and clear sendbuf*/
	sendto( udp_fd, sendbuf, strlen(sendData), 0, (struct sockaddr *)&remoteaddr, sizeof(struct sockaddr_in) );
	memset(sendbuf,0x0,strlen(sendData));
	json_object_put(items_json_object);

	mico_rtos_create_thread(MonitorAllCompleteHandle, MICO_APPLICATION_PRIORITY, "monitor all complete", MonitorAllComplete_thread, MonitorAllCompleted_STK_SIZE, 0);
#ifdef defaultKey_test
	mico_rtos_create_thread(FacKeyTestHandle, MICO_APPLICATION_PRIORITY, "Factory_key_test", FacKeyTest_thread, FacKeyTest_STK_SIZE, (uint32_t)0);
#endif
	
	while(1)
	{
		if(notifi_endfactoryTest ==1)
			goto exit;
		
		FD_ZERO(&readfds);
		FD_SET(udp_fd, &readfds);

		memset(buf,0x0,100);
		memset(sendbuf,0x0,100);

		require_action( select(udp_fd + 1, &readfds, NULL, NULL, NULL) >= 0, exit,err = kConnectionErr );

		 /*Read data from udp and deal with */
		if(FD_ISSET(udp_fd, &readfds))
		{
			len = recvfrom(udp_fd, buf,100, 0, (struct sockaddr *)&addr, &addrLen);
			require_action( len >= 0, exit, err = kConnectionErr );

			if((errcode =Do_TestItem(buf)) != FinishNoErr)
			{
				send_json_object = json_object_new_object();
				result_Report(send_json_object, errcode);
				if(IsReport)
				{
					sendData= json_object_to_json_string(send_json_object);
					memcpy(sendbuf,sendData,strlen(sendData));
					sendto( udp_fd, sendbuf, strlen(sendData), 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_in) );
					json_object_put(send_json_object);
				}
			}
			else 
			{
				mico_rtos_set_semaphore(&endFactoryTest_sem);
				break;		//end while(1)	
			}
		}
	}

exit:
    if ( err != kNoErr )
        factest_log("UDP thread exit with err: %d", err);
    if ( buf != NULL ) free( buf );
    if ( sendbuf != NULL ) free(sendbuf);
    mico_rtos_delete_thread( NULL );
#ifdef defaultKey_test
    mico_rtos_delete_thread( FacKeyTestHandle );
#endif
}

//=============================================================================
//  执行用户注册的测试函数
//=============================================================================
static int Do_TestItem(char *recvbuf)
{
	json_object *item_obj = NULL;
	json_object *ob = NULL;
	int result = Invalid;
	_usrFunc_list_t* index = usrFunc_list;

	item_obj = json_tokener_parse(recvbuf);	
	if(NULL==item_obj) return Invalid;				

	ob = json_object_object_get(item_obj, FactoryTest_CODE);	//"Test"
	if(ob!=NULL)
	{
		if(usrFunc_list == NULL)
			return Invalid;			//The user does not register the function
		else
		{
			do
			{
				result = ((usrRegister_function)(index->function))((json_object *)ob );  //usrFunc_list->arg
				if(result != Invalid)
					return result;
				index = index->next;
			}while(index != NULL);
		}
		if(strcmp(json_object_get_string(ob), testFinish) ==0)
			return	FinishNoErr;
		return Invalid;
   	 }
	return Invalid;
}
//=============================================================================
// 注册测试函数api
//=============================================================================
int usrFunction_register(void *functionAddress, void *arg)
{
	_usrFunc_list_t *temp = usrFunc_list;
	_usrFunc_list_t *regis = (_usrFunc_list_t *)malloc(sizeof(_usrFunc_list_t));
	
	if(regis == NULL)
		return -1;
	
	regis->function = functionAddress;
	regis->arg = arg;
	regis->next = NULL;
	
	if(usrFunc_list == NULL)
	{
		usrFunc_list = regis;
		regis->next = NULL;
	}
	else
	{
		if(temp->function == functionAddress)	// register already exist
			return -2;
		while(temp->next != NULL)
		{
			temp = temp->next;
			if(temp->function == functionAddress)	// register already exist
				return -2;
		}
		temp->next = regis;
	}
	return 0;
}
//=============================================================================
//  扫描所有厂测项目是否完成进程
//=============================================================================
static void MonitorAllComplete_thread(mico_thread_arg_t arg)
{
	uint32_t i=1;
	static uint8_t OK_cnt = 0;
	
	while(1)
	{
		for(i = 1; i <= Item_total;  i++)
		{
			if(ItemBits & (1<<i))
				OK_cnt++;
			if(OK_cnt == Item_total)
			{
				factest_log("Exit test mode after %d second",ExitTestDelay/1000);
				mico_init_timer( &endTimerHandler, ExitTestDelay, endFuncCallBack, NULL);	//退出厂测
				mico_start_timer(&endTimerHandler);
				mico_rtos_get_semaphore( &endFactoryTest_sem, MICO_WAIT_FOREVER );
				mico_deinit_timer(&endTimerHandler);
				mico_rtos_set_semaphore(&endFactoryTest_sem);
				notifi_endfactoryTest = 1;
				goto exit ;		//end while(1)	
			}
		}
		OK_cnt = 0;
		mico_rtos_delay_milliseconds(500);
	}
exit:
    mico_rtos_delete_thread( NULL );	
}
//=============================================================================
//  按键扫描任务
//=============================================================================
static void FacKeyTest_thread( mico_thread_arg_t  arg )
{
	OSStatus err = -1;
	uint8_t key_up = 1;	
	struct json_object *send_json_object = NULL;
	uint8_t *sendbuf = NULL;
	const char *sendData;

	struct sockaddr_in  remoteaddr;

	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr =inet_addr(FactoryTest_RemoIP);	
	remoteaddr.sin_port = htons(FactoryTest_RemoPORT);		

	sendbuf = malloc(20);
	require_action(sendbuf,exit, err = kNoMemoryErr);
		
	while(1)
	{
		if(key_up && ((MicoGpioInputGet(SYS_KEY1) == false) || ( MicoGpioInputGet(SYS_KEY2) == false)))
		{
			key_up = 0;

			if(MicoGpioInputGet(SYS_KEY1) == false)
			{
				mico_rtos_delay_milliseconds(100);
				if((MicoGpioInputGet(SYS_KEY1) == false) && (MicoGpioInputGet(SYS_KEY2) == true))
				{
					//Key1  press
					send_json_object = json_object_new_object();
					json_object_object_add(send_json_object, "Key", json_object_new_int(1));
					sendData = json_object_to_json_string(send_json_object);
					memcpy(sendbuf,sendData,strlen(sendData));
					sendto( udp_fd, sendbuf, strlen(sendData), 0, (struct sockaddr *)&remoteaddr, sizeof(struct sockaddr_in) );
					memset(sendbuf,0x0,strlen(sendData));
					json_object_put(send_json_object);

					ItemBits |= (1<<Key1Bit);
				}
			}
			else if((MicoGpioInputGet(SYS_KEY1) == true) && (MicoGpioInputGet(SYS_KEY2) == false))
			{
				mico_rtos_delay_milliseconds(100);
				if(MicoGpioInputGet(SYS_KEY2) == false)
				{
					//Key2   press
					send_json_object = json_object_new_object();
					json_object_object_add(send_json_object, "Key", json_object_new_int(2));
					sendData = json_object_to_json_string(send_json_object);
					memcpy(sendbuf,sendData,strlen(sendData));
					sendto( udp_fd, sendbuf, strlen(sendData), 0, (struct sockaddr *)&remoteaddr, sizeof(struct sockaddr_in) );
					memset(sendbuf,0x0,strlen(sendData));
					json_object_put(send_json_object);

					ItemBits |= (1<<Key2Bit);
				}
			}
		}
		else if((MicoGpioInputGet(SYS_KEY1)== true) &&( MicoGpioInputGet(SYS_KEY2) == true))
		{
			key_up = 1;
		}
		
		mico_rtos_delay_milliseconds(500);
	}
exit:
    if ( err != kNoErr )
        factest_log("FactoryKeyTest   thread exit with err: %d", err);
    if ( sendbuf != NULL ) free(sendbuf);
    mico_rtos_delete_thread( NULL );
}

//=============================================================================
// 指示灯触发
//=============================================================================
static void led_EL_Timeout_handler( void* arg )
{
  (void)(arg);
  MicoGpioOutputTrigger((mico_gpio_t)MICO_SYS_LED);
}

void FacTest_LedFlash(void)
{
      /*Led trigger*/
    if(_Led_EL_timer_initialized == true)
    {
        mico_stop_timer(&_Led_EL_timer);
        mico_deinit_timer( &_Led_EL_timer );
        _Led_EL_timer_initialized = false;
    }

    mico_init_timer(&_Led_EL_timer, FacTestLED_Tri_INTERVAL, led_EL_Timeout_handler, NULL);
    mico_start_timer(&_Led_EL_timer);
    _Led_EL_timer_initialized = true;

    return;
}
void FacTest_LedOff(void)
{
    if(_Led_EL_timer_initialized == true)
    {
        mico_stop_timer(&_Led_EL_timer);
        mico_deinit_timer( &_Led_EL_timer );
        _Led_EL_timer_initialized = false;
    }

    MicoSysLed(false);
}
//=============================================================================
// 完成测试所有测试项目后，
//=============================================================================
void endFuncCallBack(void *arg)
{
	mico_rtos_set_semaphore(&endFactoryTest_sem);
	mico_stop_timer(&endTimerHandler);
}