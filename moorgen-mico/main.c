/**
 ******************************************************************************
 * @file    main.c
 * @author  
 * @version V1.0.0
 * @date    
 * @brief   
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ******************************************************************************
 */ 

#include "mico.h"
#include "moorgenService.h"
#include "mgOS.h"
#include "product.h"
#include "factoryTest.h"

#define app_log(M, ...)             custom_log("APP", M, ##__VA_ARGS__)
#define app_log_trace()             custom_log_trace("APP")

static MOORGEN_SYS_INFO moorgen_sys_info;
extern system_context_t* sys_context;

/* KeyScan thread define */
#define KeyScan_STK_SIZE 	1024
mico_thread_t* KeyScanHandle;
void KeyScan_thread( mico_thread_arg_t  arg );
/* Scan the motor to the top or bottom */
#define MotorScan_STK_SIZE	1024
mico_thread_t* MotorScanHandle;
void MotorScan_thread(mico_thread_arg_t arg);
	
//static  system_context_t  *inContext=NULL;
extern uint8_t RunStatus;



OSStatus mg_mico_system_init( mico_Context_t* in_context )
{
  OSStatus err = kNoErr;

  require_action( in_context, exit, err = kNotPreparedErr );


  /* Initialize mico notify system */
  err = system_notification_init( sys_context );
  require_noerr( err, exit ); 

    err = system_network_daemen_start( sys_context );
    require_noerr( err, exit ); 

 
#ifdef MICO_SYSTEM_MONITOR_ENABLE
  /* MiCO system monitor */
  err = mico_system_monitor_daemen_start( );
  require_noerr( err, exit ); 
#endif

#ifdef MICO_CLI_ENABLE
  /* MiCO command line interface */
  cli_init();
#endif

exit:
  return err;
}


int application_start( void )
{
	// Add your code here
    app_log("CONFIG_PRODUCT:%s",CONFIG_PRODUCT);
    app_log("CONFIG_FACTORY:%s",CONFIG_FACTORY);
    app_log("CONFIG_VERSION:%s",CONFIG_VERSION);
    app_log("CONFIG_DEVICE_TYPE:%s",CONFIG_DEVICE_TYPE);

    #ifdef CONFIG_MQTT_CA_EU
    app_log("CONFIG_MQTT_CA_EU");
    #endif

    #ifdef CONFIG_MQTT_CA_CN
    app_log("CONFIG_MQTT_CA_CN");
    #endif

    mg_mico_system_init(mico_system_context_init(9216));	//storage  certificate 
    mg_os_init();

/* Whether to conduct factory test */
    FactoryTest();

    mico_rtos_create_thread( KeyScanHandle, MICO_APPLICATION_PRIORITY, "Key_Scan", KeyScan_thread, KeyScan_STK_SIZE, 0 );
    mico_rtos_create_thread( MotorScanHandle, MICO_APPLICATION_PRIORITY, "Motor_Scan", MotorScan_thread, MotorScan_STK_SIZE, 0 );
/*	
    mg_eeprom_read(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));	
    app_log("currentPostion = %d, targetPostion =%d",moorgen_sys_info.moorgen_des.currentPostion, moorgen_sys_info.moorgen_des.targetPostion);
    app_log("journey set %d", moorgen_sys_info.moorgen_des.currentState);
*/ 
    moorgen_service_init(CONFIG_FACTORY,CONFIG_PRODUCT,CONFIG_DEVICE_TYPE,CONFIG_VERSION);
    product_init();

    mg_wifi_start();

    moorgen_service_start();

//    while(1);

exit:
    return 0;
}

void KeyScan_thread( mico_thread_arg_t  arg )
{
	uint8_t controlMode = 0xff;
	static uint8_t key_up = 1; 	//按键松开标志
	static uint8_t run_flag = 0; 

	Key_GPIO_Init();
	mgos_log("KeyScan_thread in \r\n");
	while(1)
	{
		controlMode = GainControlMode() ;
	//	mgos_log("controlMode: %d",controlMode);
		if(controlMode != 0xff)
		{
			 if(controlMode ==0)			//Double-button
			 {
				if(key_up && ((MicoGpioInputGet(SYS_KEY1) == false) ||( MicoGpioInputGet(SYS_KEY2) == false)))
				{
					mico_rtos_delay_milliseconds(10);
					key_up = 0;
					if((MicoGpioInputGet(SYS_KEY1) == false) && ( MicoGpioInputGet(SYS_KEY2) == true))
					{
						if(run_flag ==1)
						{
							MotorExe(0,isstop);
							run_flag = 0;
						}
						else		// 0  : stop     2: reversal
						{
							MotorExe(0,isforward);
							run_flag = 1;
						}
					}
					else if((MicoGpioInputGet(SYS_KEY1) == true) && ( MicoGpioInputGet(SYS_KEY2) == false) )
					{
						if(run_flag ==2)
						{
							MotorExe(0,isstop);
							run_flag = 0;
						}
						else
						{
							MotorExe(0,isreversal);
							run_flag = 2;
						}
					}
				}
				else if((MicoGpioInputGet(SYS_KEY1)== true) &&( MicoGpioInputGet(SYS_KEY2) == true))
				{
					key_up = 1;
				}
			 }
			 else if(controlMode ==1)		//Double-button Reboundable
			 {
				key_up = 1;
				if(key_up && ((MicoGpioInputGet(SYS_KEY1)== false) ||( MicoGpioInputGet(SYS_KEY2)== false)))
				{
					mico_rtos_delay_milliseconds(10);
					key_up = 0;
					if((MicoGpioInputGet(SYS_KEY1) == false) && (MicoGpioInputGet(SYS_KEY2)== true) )
					{
						MotorExe(0,isforward);
						run_flag = 1;
					}
					else if((MicoGpioInputGet(SYS_KEY1) == true) && (MicoGpioInputGet(SYS_KEY2)== false))
					{
						MotorExe(0,isreversal);
						run_flag = 2;
					}
				}
				else if((MicoGpioInputGet(SYS_KEY1)== true) && ( MicoGpioInputGet(SYS_KEY2) == true))
				{
					MotorExe(0,isstop);
					key_up = 1;
					run_flag = 0;
				}
			 }
			 else if(controlMode == 2)		//246
			 {
				if(key_up && ((MicoGpioInputGet(SYS_KEY1) == false) ||( MicoGpioInputGet(SYS_KEY2) == false)))
				{
					mico_rtos_delay_milliseconds(10);
					key_up =0;
					if((MicoGpioInputGet(SYS_KEY1) == false) && (MicoGpioInputGet(SYS_KEY2) == true))
					{
						MotorExe(0,isforward);
						run_flag = 1;
					}
					else if((MicoGpioInputGet(SYS_KEY1) == true) && (MicoGpioInputGet(SYS_KEY2)== false))
					{
						MotorExe(0,isreversal);
						run_flag = 2;
					}
					else //both pressed 
					{
						MotorExe(0,isstop);
						run_flag = 0;
					}
				}
				else if((MicoGpioInputGet(SYS_KEY1) == true) &&( MicoGpioInputGet(SYS_KEY2) == true))
				{
					key_up = 1;
				}
				else if(!key_up && ((MicoGpioInputGet(SYS_KEY1) == false) && ( MicoGpioInputGet(SYS_KEY2) == false)))
				{
					MotorExe(0,isstop);
					run_flag = 0;
				}
			 }
		}
		mico_rtos_delay_milliseconds(50);
	}
}

void MotorScan_thread(mico_thread_arg_t arg)
{
	static uint8_t lowLevel_cnt = 0;

	MicoGpioInitialize(MICO_GPIO_12, INPUT_PULL_DOWN);//INPUT_PULL_DOWN
	while(1)
	{
		if(RunStatus != 0)
		{
			if(MicoGpioInputGet(MICO_GPIO_12) == false)	//true
			{
				lowLevel_cnt++;
				if(lowLevel_cnt >= 99)
				{
					lowLevel_cnt = 0;
					MotorExe(0,isstop);
				}
			}
			else 
			{
				lowLevel_cnt = 0;
			}
		}
		mico_rtos_delay_milliseconds(5);
	}
}
