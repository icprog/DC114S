/*
******************************************************************************
* @file    factoryTest.h  
* @author  huang    2018-9-12
* @version V1.2.2      
* @date    
* @brief   Factory Test.  The user just calls function that "void FactoryTest(void)".
*		   and config :
*
*		const char *factory_SSID = "test3";				
*		const char *factory_Password = "continue"; 
*		#define FactoryTest_RemoIP		"192.168.137.1"
*		#define FactoryTest_RemoPORT 	8000
*		#define FactoryTest_LocalPORT	20000
*
*		#define FactoryTest_CODE		"Test"
*
*		#define FinishNoErr				0
*		#define MotorNoErr	1			1
*		#define EepromNoErr				2
*		#define WifiRssiNoErr			3
*		#define MotorErr				-1
*		#define EepromErr				-2
*		#define WifiRssiErr				-3
*		#define Invalid					0x7f
*
*note:   The above configuration is included in "mgOS.h" . If the file is not included in the user project,
*	     The above parameters need to be individual configured.
*
*版本说明:  在V1.2.0基础上:
*				1.修改进入厂测模式(EasLink && KEY1 开机前同时按下)
*				2.尝试连接AP，失败5次后，退出厂测模式
*				3.增加LED显示，300ms间隔闪烁
*				4.增加IsReport变量，标识测试项目是否要上报(主要针对按键)
*
*				在V1.2.1基础上:
*				1.增加了所有测试项目完成后，在ExitTestDelay(ms)时间后自动退出厂测模式
******************************************************************************
*
*  Copyright (c) 2018 DOOYA Inc.
*
******************************************************************************
*/
#ifndef _FACTEST_H__
#define _FACTEST_H__

#include "mico.h"
#include "mgOS.h"


//是否缺省开启按键扫描测试
#define defaultKey_test

//配置服务器和本地port
#define FactoryTest_RemoIP		"192.168.4.1"
#define FactoryTest_RemoPORT 	8000
#define FactoryTest_LocalPORT		20000

//AP端发送的测试指令固定对象， 如: {"Test":"T-Wifi"}
#define FactoryTest_CODE		"Test"
#define testFinish				"finish"

#define ExitTestDelay			30000

//注册用户测试结果码
#define FinishNoErr				0
#define MotorNoErr				1
#define EepromNoErr				2
#define WifiRssiNoErr				3
#define KeyPressNoErr			4
#define MotorErr					-1
#define EepromErr				-2
#define WifiRssiErr				-3
#define Invalid					0x7f
//设置测试项目bit 位(1~31)
#define MotorBit		MotorNoErr
#define EepromBit	EepromNoErr
#define WifiRssiBit	WifiRssiNoErr
#define Key1Bit		KeyPressNoErr
#define Key2Bit		(KeyPressNoErr+1)

typedef int (*usrRegister_function)	( json_object *ob );

typedef struct _usrFunc_list{
  void  *function;
  void  *arg;
  struct _usrFunc_list *next;
} _usrFunc_list_t;


void FactoryTest(void);

int usrFunction_register(void *functionAddress, void *arg);
//用户注册的函数申明
int Motor_test(json_object *ob);
int WifiRssi_test(json_object *ob);
int Eeprom_test(json_object *ob);
	
#endif