#include "product.h"
#include "mgKernel.h"
#include "moorgenService.h"
#include "uartProtocol.h"


extern DEVICE_STATUS    dev_status;

static DEV_MASTER_OBJEC master_ob;
/* add */
extern uint8_t RunStatus;
static MOORGEN_SYS_INFO moorgen_sys_info;
uint8_t journey_update = 0;
mico_timer_t timer_handle;
void MotorExeTimerCall(void *arg);
static mico_semaphore_t MotorExe_sem = NULL;
static uint8_t timer_flag = 0;


uint8_t Pause_Flag = 0;

void curatin_device_report()
{
    DT72_DP *dt72_dp=(DT72_DP *)master_ob.devDP;

	if(dev_status.status == 0)
		dt72_dp->operation = DP_OPS_CLOSE_CURTAIN;
	else if(dev_status.status==1)
		dt72_dp->operation= DP_OPS_OPEN_CURTAIN;
	else if(dev_status.status==2)
		dt72_dp->operation = DP_OPS_PAUSE_CURTAIN;


	dt72_dp->targetPosition= dev_status.targetPosition;


	if(dev_status.currentPosition<=100)
		dt72_dp->currentPosition= dev_status.currentPosition;
	else
		dt72_dp->currentPosition = 0;


	if(dev_status.currentPosition==0xff)
		dt72_dp->direction= DP_DIRECTION_NO_CALIBRITION;
	else 
		dt72_dp->direction= dev_status.direction;

	dt72_dp->currentState = dev_status.currentState;		//journey

	dev_log("curatin_device_report");
	dev_log("#####################");
	dev_log("operation:%d",dt72_dp->operation);
	dev_log("currentPosition:%d",dt72_dp->currentPosition);
	dev_log("targetPosition:%d",dt72_dp->targetPosition);
	dev_log("direction:%d",dt72_dp->direction);
	dev_log("journey: %d", dt72_dp->currentState);
	dev_log("#####################");

    if(master_ob.dev_ops.dev_op_dp_report!=NULL)
        master_ob.dev_ops.dev_op_dp_report(DEVICE_MASTER,(void *)&master_ob);
}


void curtainTargetPositionSet(UINT8 targetPosition)
{
	OSStatus err =kNoErr;
	uint8_t positionDiffer = 0;
	uint32_t B2T_t = 0, T2B_t = 0;
	static uint32_t adjust_cnt  = 0;
	uint32_t tmp1,tmp2, offset = 0;

	mico_rtos_init_semaphore( &MotorExe_sem, 1 );
/*
    if(dev_status.currentPosition==0xff)
    {
        curatin_device_report();
        return;
    }
 */
    	dev_status.targetPosition=targetPosition;
	dev_log("curtainTargetPositionSet");
//    devUartCtrl(POSITION_ADDR,&targetPosition,1);
	dev_status.app=1;

	mg_eeprom_read(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));
	B2T_t = moorgen_sys_info.moorgen_des.journey_up;
	T2B_t = moorgen_sys_info.moorgen_des.journey_down;
	dev_status.direction = moorgen_sys_info.moorgen_des.direction;
	dev_status.currentState = moorgen_sys_info.moorgen_des.currentState;
	
	dev_log("B2T_t : %ld", B2T_t);
	dev_log("T2B_t : %ld", T2B_t);

	if(moorgen_sys_info.moorgen_des.currentState == 0)
	{
		dev_log("Unset journey");
		return ;
	}
	
	if((adjust_cnt == 0)&&(!journey_update))
	{
		dev_log("not update journey");
		if(dev_status.targetPosition<=50)						// priority to up
		{	
			MotorExe(dev_status.direction, isreversal);			//to     position   0
			dev_status.status = 1;
			while(RunStatus != 0)
			{
				mico_rtos_delay_milliseconds(50);
			}
			dev_status.status = 2;
			dev_log("is reach top");
			positionDiffer =  dev_status.targetPosition;
			timer_flag = 0;
			err = mico_init_timer(&timer_handle,  (uint32_t)((float)positionDiffer/100*T2B_t), MotorExeTimerCall, NULL);
			require_noerr(err,exit);
		
			tmp1 = mico_rtos_get_time();
			dev_log("right away forward");
			MotorExe(dev_status.direction, isforward);
			dev_status.status = 0;
			err = mico_start_timer(&timer_handle);
			require_noerr(err,exit);
			while(( RunStatus != 0) && (timer_flag == 0))		//wait  for stop
			{
				mico_rtos_delay_milliseconds(50);
			}
			dev_status.status = 2;
			tmp2 = mico_rtos_get_time();
			if(timer_flag != 1)			
			{
				offset = tmp2 - tmp1;
				T2B_t = offset;
				moorgen_sys_info.moorgen_des.journey_down = T2B_t;
				if((uint32_t)((float)(100 - dev_status.targetPosition)/100*B2T_t) >50)
				{
					mico_rtos_get_semaphore(&MotorExe_sem,MICO_WAIT_FOREVER);		//consum
			//		timer_flag = 0;
					mico_init_timer(&timer_handle,  (uint32_t)((float)(100 - dev_status.targetPosition)/100*B2T_t), MotorExeTimerCall, NULL);
					MotorExe(dev_status.direction, isreversal);
					dev_status.status = 1;
					mico_start_timer(&timer_handle);
					mico_rtos_get_semaphore( &MotorExe_sem, MICO_WAIT_FOREVER );
					timer_flag = 0;
					dev_status.status = 2;
				}
			}
			else if(dev_status.targetPosition == 100)		
			{
				tmp1 = mico_rtos_get_time();
				MotorExe(dev_status.direction, isforward);
				dev_status.status = 0;
				while(RunStatus != 0)
				{
					mico_rtos_delay_milliseconds(50);
				}
				dev_status.status = 2;
				tmp2 = mico_rtos_get_time();
				offset = tmp2 -tmp1;
				T2B_t +=offset;
				moorgen_sys_info.moorgen_des.journey_down = T2B_t;
			}
		
		}
		else			//targetPosition > 50
		{
			MotorExe(dev_status.direction, isforward);			//to     position  100
			dev_status.status = 0;
			while(RunStatus != 0)
			{
				mico_rtos_delay_milliseconds(50);
			}
			dev_status.status = 2;
			dev_log("is reach bottom");
			positionDiffer =  100 - dev_status.targetPosition;
			timer_flag = 0;
			err = mico_init_timer(&timer_handle,  (uint32_t)((float)positionDiffer/100*B2T_t), MotorExeTimerCall, NULL);
			require_noerr(err,exit);
		
			tmp1 = mico_rtos_get_time();
			dev_log("right away reverse");
			MotorExe(dev_status.direction, isreversal);
			dev_status.status = 1;
			err = mico_start_timer(&timer_handle);
			require_noerr(err,exit);
			while(( RunStatus != 0) && (timer_flag == 0))		//wait  for stop
			{
				mico_rtos_delay_milliseconds(50);
			}
			dev_status.status = 2;
			tmp2 = mico_rtos_get_time();
			if(timer_flag != 1)			
			{
				offset = tmp2 - tmp1;
				B2T_t = offset;
				moorgen_sys_info.moorgen_des.journey_up= B2T_t;
				if((uint32_t)((float)(dev_status.targetPosition)/100*T2B_t) >50)
				{
					mico_rtos_get_semaphore(&MotorExe_sem,MICO_WAIT_FOREVER);		//consum
			//		timer_flag = 0;
					mico_init_timer(&timer_handle,  (uint32_t)((float)(dev_status.targetPosition)/100*T2B_t), MotorExeTimerCall, NULL);
					MotorExe(dev_status.direction, isforward);
					dev_status.status = 0;
					mico_start_timer(&timer_handle);
					mico_rtos_get_semaphore( &MotorExe_sem, MICO_WAIT_FOREVER );
					timer_flag = 0;
					dev_status.status = 2;
				}
			}
			else if(dev_status.targetPosition == 0)		
			{
				tmp1 = mico_rtos_get_time();
				MotorExe(dev_status.direction, isreversal);
				dev_status.status = 1;
				while(RunStatus != 0)
				{
					mico_rtos_delay_milliseconds(50);
				}
				dev_status.status = 2;
				tmp2 = mico_rtos_get_time();
				offset = tmp2 -tmp1;
				B2T_t +=offset;
				moorgen_sys_info.moorgen_des.journey_up = B2T_t;
			}
			
		}
		dev_status.currentPosition = dev_status.targetPosition;
		mico_deinit_timer(&timer_handle);
		adjust_cnt++;
	}
	else
	{
		if(dev_status.currentPosition < dev_status.targetPosition)
		{
			positionDiffer =  dev_status.targetPosition - dev_status.currentPosition;
			timer_flag = 0;
			err = mico_init_timer(&timer_handle,  (uint32_t)((float)positionDiffer/100*T2B_t), MotorExeTimerCall, NULL);
			require_noerr(err,exit);
			tmp1 = mico_rtos_get_time();
			MotorExe(dev_status.direction, isforward);
			dev_status.status = 0;
			err = mico_start_timer(&timer_handle);
			require_noerr(err,exit);
			while(( RunStatus != 0) && (timer_flag == 0))		//wait  for stop
			{
				mico_rtos_delay_milliseconds(50);
			}
			dev_status.status = 2;
			tmp2 = mico_rtos_get_time();
			if(timer_flag != 1)			
			{
				dev_log("bottom  stop");
				offset = tmp2 - tmp1;
				T2B_t = (uint32_t)((float)dev_status.currentPosition/100*T2B_t) + offset;
				moorgen_sys_info.moorgen_des.journey_down = T2B_t;
				if((uint32_t)((float)(100 - dev_status.targetPosition)/100*B2T_t) >50)
				{
					mico_rtos_get_semaphore(&MotorExe_sem,MICO_WAIT_FOREVER);		//consume 
			//		timer_flag = 0;
					mico_init_timer(&timer_handle,  (uint32_t)((float)(100 - dev_status.targetPosition)/100*B2T_t), MotorExeTimerCall, NULL);
					MotorExe(dev_status.direction, isreversal);
					dev_status.status = 1;
					mico_start_timer(&timer_handle);
					mico_rtos_get_semaphore( &MotorExe_sem, MICO_WAIT_FOREVER );
					timer_flag = 0;
					dev_status.status = 2;
				}
			}
			else if(dev_status.targetPosition == 100)		
			{
				tmp1 = mico_rtos_get_time();
				MotorExe(dev_status.direction, isforward);
				dev_status.status = 0;
				while(RunStatus != 0)
				{
					mico_rtos_delay_milliseconds(50);
				}
				dev_status.status = 2;
				tmp2 = mico_rtos_get_time();
				offset = tmp2 -tmp1;
				T2B_t +=offset;
				moorgen_sys_info.moorgen_des.journey_down = T2B_t;
			}
		}
		else
		{
			positionDiffer =  dev_status.currentPosition - dev_status.targetPosition; 
			timer_flag = 0;
			err = mico_init_timer(&timer_handle,  (uint32_t)((float)positionDiffer/100*B2T_t), MotorExeTimerCall, NULL);
			require_noerr(err,exit);
			tmp1 = mico_rtos_get_time();
			MotorExe(dev_status.direction, isreversal);
			dev_status.status = 1;
			err = mico_start_timer(&timer_handle);
			require_noerr(err,exit);
			while(( RunStatus != 0) && (timer_flag == 0))		//wait  for stop
			{
				mico_rtos_delay_milliseconds(50);
			}
			dev_status.status = 2;
			tmp2 = mico_rtos_get_time();
			if(timer_flag != 1)			
			{
				dev_log("top  stop");
				offset = tmp2 - tmp1;
				B2T_t = (uint32_t)((float)(100 - dev_status.currentPosition)/100*B2T_t )+ offset;		//dev_status.currentPosition  != 0
				moorgen_sys_info.moorgen_des.journey_up = B2T_t;
				if((uint32_t)((float)( dev_status.targetPosition)/100*T2B_t) >50)
				{
					mico_rtos_get_semaphore(&MotorExe_sem,MICO_WAIT_FOREVER);		//consume 
				//	timer_flag = 0;
					mico_init_timer(&timer_handle,  (uint32_t)((float)( dev_status.targetPosition )/100*T2B_t), MotorExeTimerCall, NULL);
					MotorExe(dev_status.direction, isforward);
					dev_status.status = 0;
					mico_start_timer(&timer_handle);
					mico_rtos_get_semaphore( &MotorExe_sem, MICO_WAIT_FOREVER );
					timer_flag = 0;
					dev_status.status = 2;
				}
			}
			else if(dev_status.targetPosition == 0)		
			{
				tmp1 = mico_rtos_get_time();
				MotorExe(dev_status.direction, isreversal);
				dev_status.status = 1;
				while(RunStatus != 0)
				{
					mico_rtos_delay_milliseconds(50);
				}
				dev_status.status = 2;
				tmp2 = mico_rtos_get_time();
				offset = tmp2 -tmp1;
				B2T_t +=offset;
				moorgen_sys_info.moorgen_des.journey_up = B2T_t;
			}
		}
		dev_status.currentPosition = dev_status.targetPosition;
	}
	mico_deinit_timer(&timer_handle);
	moorgen_sys_info.moorgen_des.currentPostion = dev_status.targetPosition;
	moorgen_sys_info.moorgen_des.targetPostion = dev_status.targetPosition;
	mg_eeprom_write(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));

  exit:
	curatin_device_report();
  	mico_rtos_deinit_semaphore(&MotorExe_sem);
  	dev_log("timer  err %d",err);
	return;
}

void curtainOpenSet()
{
   	dev_status.targetPosition=0;
//	dev_status.app=1;

	dev_status.status = 1;

	mg_eeprom_read(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));
	dev_status.direction = moorgen_sys_info.moorgen_des.direction;
	dev_status.currentState = moorgen_sys_info.moorgen_des.currentState;
	
	MotorExe(dev_status.direction, isreversal);

	Pause_Flag = 0;
	while((RunStatus != 0) && (Pause_Flag ==0 ))
	{
		mico_rtos_delay_milliseconds(50);
	}
	if(Pause_Flag == 1) 
	{
		dev_log("Open is paused");
		Pause_Flag = 0;
		return ;
	}
	moorgen_sys_info.moorgen_des.currentPostion = 0;
	moorgen_sys_info.moorgen_des.targetPostion = 0;
	moorgen_sys_info.moorgen_des.status = 2;

	mg_eeprom_write(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));
	 
	dev_status.status = 2;
	dev_status.currentPosition = 0;
//	dev_status.currentState = 0;

	curatin_device_report();
}


void curtainCloseSet()
{	
	dev_status.targetPosition=100;
//   	 dev_status.app=1;

	dev_status.status = 0;		//0

	mg_eeprom_read(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));
	dev_status.direction = moorgen_sys_info.moorgen_des.direction;
	dev_status.currentState = moorgen_sys_info.moorgen_des.currentState;
	
	MotorExe(dev_status.direction, isforward);

	Pause_Flag = 0;
	while((RunStatus != 0) && (Pause_Flag ==0 ))
	{
		mico_rtos_delay_milliseconds(50);
	}
	if(Pause_Flag == 1) 
	{
		dev_log("Close is paused");
		Pause_Flag = 0;
		return ;
	}
	moorgen_sys_info.moorgen_des.currentPostion = 100;
	moorgen_sys_info.moorgen_des.targetPostion = 100;
	moorgen_sys_info.moorgen_des.status = 2;

	mg_eeprom_write(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));

	 dev_status.status = 2;
	dev_status.currentPosition = 100;
//	dev_status.currentState = 0;			

	curatin_device_report();
}

void curtainDirectionSet()
{
	if(dev_status.direction == 0)
		dev_status.direction  = 1;
	else if(dev_status.direction == 1 )
		dev_status.direction = 0;

	moorgen_sys_info.moorgen_des.direction = dev_status.direction;
	
	 mg_eeprom_write(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));
}

/* add stop handler */
void curtainPauseSet()
{
	dev_status.app=1;		//
	
	dev_status.status = 0;		//pause  2

	MotorExe(dev_status.direction, isstop);
	Pause_Flag  = 1;

	moorgen_sys_info.moorgen_des.status = 2;
	mg_eeprom_write(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));

	curatin_device_report();
}
void curtainJourneySet()
{
	uint32_t tmp1= 0, tmp2 = 0;
	uint32_t BtoT_t , TtoB_t;  

	mg_eeprom_read(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));
	dev_status.direction = moorgen_sys_info.moorgen_des.direction;
	MotorExe(dev_status.direction, isforward);		//isforward
	dev_status.status = 0;
	while(RunStatus != 0)						//Wait until the motor reaches the bottom
	{
		mico_rtos_delay_milliseconds(50);
	}
	dev_status.status = 2;
	tmp1 = mico_rtos_get_time();					//require  system time(uint: ms)
	MotorExe(dev_status.direction, isreversal);		//isreversal
	dev_status.status = 1;
	while(RunStatus != 0)						//Wait until the motor reaches the top
	{
		mico_rtos_delay_milliseconds(5);
	}
	dev_status.status = 2;
	tmp2 = mico_rtos_get_time();
	BtoT_t = tmp2 -tmp1;
	dev_status.status = 2;
	dev_log("BtoT_t = %ld\r\n",BtoT_t);

	MotorExe(dev_status.direction, isforward);		//isforward
	dev_status.status = 0;
	while(RunStatus != 0)
	{
		mico_rtos_delay_milliseconds(5);
	}
	dev_status.status = 2;
	tmp1 = mico_rtos_get_time();

	TtoB_t = tmp1 - tmp2;
	dev_log("TtoB_t = %ld\r\n",TtoB_t);
	moorgen_sys_info.moorgen_des.journey_up = BtoT_t;
	moorgen_sys_info.moorgen_des.journey_down = TtoB_t;
	moorgen_sys_info.moorgen_des.currentState = 3;
	moorgen_sys_info.moorgen_des.currentPostion = 100;
	moorgen_sys_info.moorgen_des.targetPostion = 100;
	
	
	 mg_eeprom_write(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));	

	 dev_status.currentPosition = 100;
	dev_status.targetPosition = 100;
	 journey_update = 1;

	 dev_status.currentState = 3;

	 curatin_device_report();
	 
}

void curtainMsgReport()
{
	DEVICE_STATUS    report_dev_status;
	
	mg_eeprom_read(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));
	report_dev_status.currentPosition = moorgen_sys_info.moorgen_des.currentPostion;
	report_dev_status.targetPosition = moorgen_sys_info.moorgen_des.targetPostion;
	report_dev_status.direction = moorgen_sys_info.moorgen_des.direction;
	report_dev_status.currentState =moorgen_sys_info.moorgen_des.currentState;
//	dev_status.status = moorgen_sys_info.moorgen_des.status;
	report_dev_status.status = dev_status.status;

//REPORT:
    	DT72_DP *dt72_dp=(DT72_DP *)master_ob.devDP;

	dev_log("***********msg report****************");
	if(report_dev_status.status==1)
		dt72_dp->operation= DP_OPS_OPEN_CURTAIN;
	else if(report_dev_status.status==2)
		dt72_dp->operation = DP_OPS_PAUSE_CURTAIN;
	else
		dt72_dp->operation = DP_OPS_CLOSE_CURTAIN;

	dt72_dp->targetPosition= report_dev_status.targetPosition;


	if(report_dev_status.currentPosition<=100)
		dt72_dp->currentPosition= report_dev_status.currentPosition;
	else
		dt72_dp->currentPosition = 0;


	if(report_dev_status.currentPosition==0xff)
		dt72_dp->direction= DP_DIRECTION_NO_CALIBRITION;
	else 
		dt72_dp->direction= report_dev_status.direction;

	dt72_dp->currentState = report_dev_status.currentState;		//journey


    if(master_ob.dev_ops.dev_op_dp_report!=NULL)
        master_ob.dev_ops.dev_op_dp_report(DEVICE_MASTER,(void *)&master_ob);
}

void curtainPointMoveDn()
{
	mg_eeprom_read(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));
	dev_status.direction = moorgen_sys_info.moorgen_des.direction;

	MotorExe(dev_status.direction, isforward);
	dev_status.status = 0;

	mico_rtos_delay_milliseconds(500);

	MotorExe(dev_status.direction, isstop);
	dev_status.status = 2;
}

void curtainPointMoveUp()
{
	mg_eeprom_read(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));
	dev_status.direction = moorgen_sys_info.moorgen_des.direction;

	MotorExe(dev_status.direction, isreversal);
	dev_status.status = 1;

	mico_rtos_delay_milliseconds(500);

	MotorExe(dev_status.direction, isstop);
	dev_status.status = 2;
}
void dt72_dp_ctrl(DEVICE_TYPE type ,void *arg,void *dp)
{

    DT72_DP *dt72_dp=(DT72_DP *)dp;

    if(type!=DEVICE_MASTER)
        return;

	dev_log("dt72_dp_ctrl");
	dev_log("#####################");
	dev_log("operation:%d",dt72_dp->operation);
	dev_log("currentPosition:%d",dt72_dp->currentPosition);
	dev_log("targetPosition:%d",dt72_dp->targetPosition);
	dev_log("direction:%d",dt72_dp->direction);
	dev_log("#####################");


    if(dt72_dp->operation!=0xff )
    {
        switch(dt72_dp->operation)
        {
        	case DP_OPS_CLOSE_CURTAIN:
			dev_log("DP_OPS_CLOSE_CURTAIN\r\n");	
   			curtainCloseSet();
  			break;
        	case DP_OPS_OPEN_CURTAIN:
			dev_log("DP_OPS_OPEN_CURTAIN\r\n");		
            		curtainOpenSet();
            		break;
       	 	case DP_OPS_REVERSE:
			dev_log("DP_OPS_REVERSE\r\n");	
           		curtainDirectionSet();
			break;
		 case DP_OPS_JOURNEY:
		 	dev_log("DP_OPS_JOURNEY\r\n");	
			curtainJourneySet();
			break;
		case DP_OPS_REPORT:
			dev_log("DP_OPS_REPORT\r\n");	           
			curtainMsgReport();
			break;
		case DP_OPS_PointMoveUp:
			dev_log("DP_OPS_PointMoveUp\r\n");	 
			curtainPointMoveUp();
			break;
		case DP_OPS_PointMoveDn:
			dev_log("DP_OPS_PointMoveDn\r\n");	 
			curtainPointMoveDn();
			break;
        	default:	
			dev_log("default operation\r\n");
			break;
        }
    }
   else if(dt72_dp->direction != dev_status.direction)
   {
	if(dt72_dp->direction != 0xff)
	{
		dev_status.direction = dt72_dp->direction;
		moorgen_sys_info.moorgen_des.direction = dev_status.direction;
		mg_eeprom_write(SYSTEM_EE_ADDR,(uint8_t *)&moorgen_sys_info.moorgen_des,sizeof(MOORGEN_DES_S));
		curatin_device_report();
	}
   }
   if((dt72_dp->operation==0xff) && (dt72_dp->targetPosition>=0 && dt72_dp->targetPosition<=100))
    {
        curtainTargetPositionSet(dt72_dp->targetPosition);
    }
    
    
}

void dt72_dp_del(DEVICE_TYPE type ,void *arg)
{
    ;   
}


void dt72_dp_to_json(char  *devType,void *dp,void *json)
{
	json_object *dp_json_ob =(json_object *) json;
    DT72_DP *dt72_dp=(DT72_DP *)dp;
	LinkStatusTypeDef wifiLinkStatus;
	int ret =5;
	
    if(strcmp(devType,CONFIG_DEVICE_TYPE)!=0)
        return;

    if(dt72_dp->operation!=0xff)
    	json_object_object_add(dp_json_ob,DP_OPERATION,   json_object_new_int(dt72_dp->operation));
    if(dt72_dp->currentPosition!=0xff)
    	json_object_object_add(dp_json_ob,DP_CURRENT_POS, json_object_new_int(dt72_dp->currentPosition));
    if(dt72_dp->targetPosition!=0xff)
    	json_object_object_add(dp_json_ob,DP_TARGET_POS, json_object_new_int(dt72_dp->targetPosition));
    if(dt72_dp->direction!=0xff)
    	json_object_object_add(dp_json_ob,DP_DIRECTION, json_object_new_int(dt72_dp->direction));
    if(dt72_dp->currentState != 0xff)
	json_object_object_add(dp_json_ob,DP_CURRENT_STATE, json_object_new_int(dt72_dp->currentState));	

	while(ret > 0)
	{
		if(micoWlanGetLinkStatus(&wifiLinkStatus) == kNoErr)
			break;
		ret--;
	}
	if(ret>0)
		json_object_object_add(dp_json_ob, curWifiRssi, json_object_new_int(wifiLinkStatus.rssi));

  	json_object_object_add(dp_json_ob, wirelessMode, json_object_new_int(Product_wirelessMode));
}


void dt72_dp_to_struct(char  *devType ,void *dp,void *json)
{
	json_object *dp_json_ob =(json_object *) json;
	json_object *ob = NULL;
    DT72_DP *dt72_dp=(DT72_DP *)dp;

    if(strcmp(devType,CONFIG_DEVICE_TYPE)!=0)
        return;

    memset(dt72_dp,0xff,sizeof(DT72_DP));

    ob = json_object_object_get(dp_json_ob, DP_OPERATION);
	if(ob!=NULL)
    	dt72_dp->operation= json_object_get_int(ob);
    	
	ob = json_object_object_get(dp_json_ob, DP_CURRENT_POS);
	if(ob!=NULL)
    	dt72_dp->currentPosition= json_object_get_int(ob);  

	ob = json_object_object_get(dp_json_ob, DP_TARGET_POS);
	if(ob!=NULL)
    	dt72_dp->targetPosition= json_object_get_int(ob);  

   	ob = json_object_object_get(dp_json_ob, DP_DIRECTION);
	if(ob!=NULL)
    	dt72_dp->direction= json_object_get_int(ob);  


    	

}


void product_init()
{
    strncpy(master_ob.devMac,moorgen_dev_mac_get(),MG_DEV_MAC_MAX);
    strncpy(master_ob.devType,CONFIG_DEVICE_TYPE,MG_DEVICE_TYPE_SIZE);
    master_ob.hub_flag=0;
    master_ob.pair_flag=0;

    master_ob.dev_ops.dev_op_dp_ctrl=dt72_dp_ctrl;
    master_ob.dev_ops.dev_op_del=dt72_dp_del;
    master_ob.dev_ops.dev_op_dp_to_json=dt72_dp_to_json;
    master_ob.dev_ops.dev_op_dp_to_struct=dt72_dp_to_struct;

    memset(master_ob.devDP,0xff,sizeof(master_ob.devDP));
    device_master_init(&master_ob);

    devUartInit();
    moorgen_service_led_register(product_led_set);

    return;
}




static void wifiNotify_ApListCallback(ScanResult *pApList)
{
	int i=0;
	dev_log("got %d AP ",pApList->ApNum);
	for(i=0;i<pApList->ApNum;i++)
	{
		dev_log("ap%d: name = %s  |  rssi= %d",
				i,pApList->ApList[i].ssid,pApList->ApList[i].rssi);

		//if(strcmp(pApList->ApList[i].ssid,"moduletest")==0)
	    if(strcmp(pApList->ApList[i].ssid,"moduletest")==0)
		{
			devUartWifiTestSuc();
			return;
		}
	}
}


void product_fac_test()
{
    char data[12]={0};
    
    strcpy(data,"EEPROM_TEST");
    
    moorgen_user_data_write((uint8_t *)data,12);
    memset(data,0,12);
    moorgen_user_data_read((uint8_t *)data,12);
    if(strcmp(data,"EEPROM_TEST")!=0)
    {
        goto exit;
    }
    micoWlanSuspend( );
	mico_system_notify_register(mico_notify_WIFI_SCAN_COMPLETED, (void *)wifiNotify_ApListCallback,NULL);

	micoWlanStartScan();

    mico_rtos_delay_milliseconds(5000);    //sleep(5);
exit:
    MicoSystemReboot();

    
}


void MotorExeTimerCall(void *arg)
{
	MotorExe(dev_status.direction,isstop);
	mico_stop_timer(&timer_handle);
	 mico_rtos_set_semaphore( &MotorExe_sem );
	timer_flag = 1;
}
