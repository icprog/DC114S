#include "mgKernel.h"
#include "mgMqttService.h"

static TIMER_OBJECT timer_ob;
static mg_timer_t  timer_task;
static DMG_OBJECT *timer_dmg=NULL;

static ASTRONOMICAL_TIME  cur_astronomical;
static ASTRONOMICAL_TIME  astronomical_time[MG_ASTRONOMICAL_TIME_MAX];

static uint8_t timer_mqtt_exe=0;

void timer_printf(TIMER_OBJECT *timer_ob_p);
void astime_printf();


static  __inline TIMER_HEAD *timer_object_get(uint8_t timerID)
{
    TIMER_HEAD *timer_head_p;

    timer_head_p=(TIMER_HEAD *)dmgGetValue(timer_dmg,timerID);

    if(timer_head_p!=NULL)
    {
        if(timerID == timer_head_p->timerID)
            return timer_head_p;

    }

    return NULL;
}

static  __inline void timer_object_set(TIMER_HEAD *timer_head_p)
{
    dmgSetValue(timer_dmg,timer_head_p->timerID,(void *)timer_head_p);

}

TIMER_HEAD *timer_object_search(char *timerCode)
{
    uint8_t timerID=MG_ID_INVALID;
    TIMER_HEAD *timer_head_p;
    if(NULL==timerCode)
        return NULL;
  //  mg_log("timer_object_search");
   // mg_log("timerCode:%s",timerCode);

    while((timerID=dmgGetNextID(timer_dmg,timerID))!=MG_ID_INVALID)
    {
   // mg_log("timerID:%d",timerID);


        timer_head_p=timer_object_get(timerID);
        if(NULL==timer_head_p)
            continue;


        if(strncmp(timer_head_p->timerCode,timerCode,MG_CODE_SIZE)==0)
        {
         //    mg_log("timer_head_p->timerCode:%s",timer_head_p->timerCode);

            return timer_head_p;
        }
    }

    return NULL;
}


int timer_object_add( TIMER_OBJECT *timer_ob_p )
{
    uint8_t timerID;

    mg_log("timer_object_add");

    if(strlen(timer_ob_p->head.timerCode)==0)
        return -1;

    timerID=dmgGetID(timer_dmg);
    if(MG_ID_INVALID==timerID)
        return -1;

    timer_ob_p->head.timerID=timerID;

    timer_object_set(&timer_ob_p->head);

    //save
    mg_eeprom_write(TIMER_EE_ADDR+timer_ob_p->head.timerID*sizeof(TIMER_OBJECT), (uint8_t *)timer_ob_p, sizeof(TIMER_OBJECT));


    mg_kernel_timerbit_set(timer_dmg->bitSet);

   // timer_printf(timer_ob_p);
    return 0;
}


int timer_object_mod( TIMER_OBJECT *timer_ob_p )
{
     mg_log("timer_object_mod");

    timer_object_set(&timer_ob_p->head);

    //save
    mg_eeprom_write(TIMER_EE_ADDR+timer_ob_p->head.timerID*sizeof(TIMER_OBJECT), (uint8_t *)timer_ob_p, sizeof(TIMER_OBJECT));

  //  timer_printf(timer_ob_p);

    return 0;
}



void timer_object_del(uint8_t timerID )
{
     mg_log("timer_object_del");

    dmgDelID(timer_dmg,timerID);

    mg_kernel_timerbit_set(timer_dmg->bitSet);

}




TIMER_OBJECT *timer_object_next(uint8_t timerID)
{
    timerID=dmgGetNextID(timer_dmg,timerID);
    if(MG_ID_INVALID==timerID)
        return NULL;


    return (TIMER_OBJECT *)timer_object_get(timerID);

}



void timer_object_dev_item_del(uint8_t devID)
{

    uint8_t timerID=MG_ID_INVALID;
    uint8_t i;
    uint8_t saveFlag;
    TIMER_HEAD *timer_head_p;

    while((timerID=dmgGetNextID(timer_dmg,timerID))!=MG_ID_INVALID)
    {
        saveFlag=0;

        timer_head_p=timer_object_get(timerID);
        if(timer_head_p==NULL)
            continue;
        if(TIMER_DEVICE!=timer_head_p->timerEventType)
            continue;
        mg_eeprom_read(TIMER_EE_ADDR+timerID*sizeof(TIMER_OBJECT), (uint8_t *)&timer_ob, sizeof(TIMER_OBJECT));

        for(i=0;i<MG_DEVICE_MAX;i++)
        {

            if(MG_ID_INVALID==timer_ob.item.devItem[i].devID)
                continue;

            if(timer_ob.item.devItem[i].devID==devID)
            {
                timer_ob.item.devItem[i].devID=MG_ID_INVALID;
                saveFlag++;
            }

            if(saveFlag>0)
                mg_eeprom_write(TIMER_EE_ADDR+timerID*sizeof(TIMER_OBJECT), (uint8_t *)&timer_ob, sizeof(TIMER_OBJECT));

        }


    }

}





void timer_object_all_reset()
{
    if(timer_dmg->bitSet!=0)
    {
        timer_dmg->bitSet=0;
        //mg_kernel_timerbit_set(timer_dmg->bitSet);
    }
}


void timer_mqtt_exe_set( )
{

   timer_mqtt_exe=1;
}



void timer_mqtt_exe_reset( )
{

   timer_mqtt_exe=0;
}


void astronomical_today_flush(MG_TIME_DATA *curTime)
{
    int i;

 //   mg_log("astronomical_today_flush");


    for(i=0;i<MG_ASTRONOMICAL_TIME_MAX;i++)		//查找30内所有的日出，日落时间表，
    {												//找到，赋值给cur_astronomical
        if(astronomical_time[i].year==curTime->year &&
           astronomical_time[i].month==curTime->month &&
           astronomical_time[i].day==curTime->day)
        {
            cur_astronomical=astronomical_time[i];

            return;
        }
    }

    for(i=0;i<MG_ASTRONOMICAL_TIME_MAX;i++)
    {
        if(astronomical_time[i].year!=0)
            cur_astronomical=astronomical_time[i];
        else
            break;

    }


}



void astronomical_time_set(ASTRONOMICAL_TIME *astime_p,int index,int MaxIndex)
{
    MG_TIME_DATA  systime;


    if(index >= MG_ASTRONOMICAL_TIME_MAX)
        return;

    if(0==index)
    {
        memset(astronomical_time,0,sizeof(astronomical_time));
    }

    memcpy(&astronomical_time[index],astime_p,sizeof(ASTRONOMICAL_TIME));



    if(index==(MaxIndex-1) || index==(MG_ASTRONOMICAL_TIME_MAX-1) )
    {
        //save;

        mg_eeprom_write(ASTIME_EE_ADDR,(uint8_t *)astronomical_time,sizeof(astronomical_time));

        mg_rtc_get_time(&systime);

        astronomical_today_flush(&systime);

        astime_printf();
    }
}

static inline void timer_sunrise_offset(uint8_t *hr,uint8_t *min, int offset)
{
    int astime=cur_astronomical.sunrise_hr*60+cur_astronomical.sunrise_min;

    astime +=offset;

    if(astime<0)
        astime=0;
    else if(astime>1439)
        astime=1439;


    *hr=astime/60;
    *min=astime%60;

}


static inline void timer_sunset_offset(uint8_t *hr,uint8_t *min, int offset)
{
    int astime=cur_astronomical.sunset_hr*60+cur_astronomical.sunset_min;

    astime +=offset;

    if(astime<0)
        astime=0;
    else if(astime>1439)
        astime=1439;


    *hr=astime/60;
    *min=astime%60;




}


static int timer_condition_check(TIMER_HEAD   *timer_head_p,MG_TIME_DATA *curTime)
{
    uint8_t     curWeekday;
    uint8_t     hour;
    uint8_t     minute;

mg_log("timer_condition_check");
mg_log("timerID:%d",timer_head_p->timerID);
mg_log("timerCode:%s",timer_head_p->timerCode);
mg_log("repeatType:%d",timer_head_p->repeatType);
mg_log("repeatDays:%d",timer_head_p->repeatDays);
mg_log("timerType:%d",timer_head_p->timerType);
mg_log("timerOffset:%d",timer_head_p->timerOffset);




    if(REPEAT_WEEK == timer_head_p->repeatType)	//REPEAT_WEEK  1 
    {

        curWeekday = (0x01<<curTime->week);		

        if(timer_head_p->repeatDays & curWeekday )		//查看当天是否有定时器任务
        {

            if(TIMER_NORMAL==timer_head_p->timerType)	//定时器: 直接设置具体时间类型
            {

                if((timer_head_p->timerTime.hour==  curTime->hour)
                && (timer_head_p->timerTime.minute== curTime->minute))
                {
                    return 0;
                }
            }
            else if(TIMER_SUNRISE==timer_head_p->timerType)
            {
                timer_sunrise_offset(&hour,&minute,timer_head_p->timerOffset);

                mg_log("TIMER_SUNRISE %d:%d",hour,minute);

                if((minute ==curTime->minute)
                   && (hour==curTime->hour))
                {

                    return 0;
                }

/*

                if((cur_astronomical.sunrise_min ==curTime->minute)
                   && (cur_astronomical.sunrise_hr==curTime->hour))
                {

                    return 0;
                }*/

            }
            else if(TIMER_SUNSET==timer_head_p->timerType)
            {

                timer_sunset_offset(&hour,&minute,timer_head_p->timerOffset);

                mg_log("TIMER_SUNSET %d:%d",hour,minute);

                if((minute ==curTime->minute)
                   && (hour==curTime->hour))
                {

                    return 0;
                }

/*

                if((cur_astronomical.sunset_min==curTime->minute)
                   && (cur_astronomical.sunset_hr==curTime->hour))
                {

                    return 0;
                }*/
            }
        }

    }
    else if(REPEAT_NONE == timer_head_p->repeatType)
    {
        if(TIMER_NORMAL==timer_head_p->timerType)
        {

            if((timer_head_p->timerTime.hour==  curTime->hour)
            && (timer_head_p->timerTime.minute== curTime->minute)
            && (timer_head_p->timerTime.day== curTime->day)
            && (timer_head_p->timerTime.month== curTime->month)
            && (timer_head_p->timerTime.year== curTime->year))
            {


                return 0;
            }
        }
        else if(TIMER_SUNRISE==timer_head_p->timerType)
        {
            if((timer_head_p->timerTime.day== curTime->day)
            && (timer_head_p->timerTime.month== curTime->month)
            && (timer_head_p->timerTime.year== curTime->year))
            {


                timer_sunrise_offset(&hour,&minute,timer_head_p->timerOffset);

                mg_log("TIMER_SUNRISE %d:%d",hour,minute);

                if((minute ==curTime->minute)
                   && (hour==curTime->hour))
                {

                    return 0;
                }
            }
        }
        else if(TIMER_SUNSET==timer_head_p->timerType)
        {
            if((timer_head_p->timerTime.day== curTime->day)
            && (timer_head_p->timerTime.month== curTime->month)
            && (timer_head_p->timerTime.year== curTime->year))
            {

                timer_sunset_offset(&hour,&minute,timer_head_p->timerOffset);

                mg_log("TIMER_SUNSET %d:%d",hour,minute);

                if((minute ==curTime->minute)
                   && (hour==curTime->hour))
                {

                    return 0;
                }
            }
        }
    }

    return -1;

}


static  __inline  void timer_item_exe(TIMER_HEAD   *timer_head_p)
{
    int i;
    DEV_SUB_OBJECT *dev_sub_ob_p;

//    mg_log("timer_item_exe");

    if(timer_mqtt_exe==1)
    {

        if(TIMER_SUNRISE==timer_head_p->timerType || TIMER_SUNSET==timer_head_p->timerType)
        {

           mqtt_clienttime_notify_req(timer_head_p->timerCode) ;
        }
        return ;
    }

    mg_log("timer_local_exe");

    //get data
    mg_eeprom_read(TIMER_EE_ADDR+timer_head_p->timerID*sizeof(TIMER_OBJECT), (uint8_t *)&timer_ob, sizeof(TIMER_OBJECT));

    if(TIMER_DEVICE==timer_ob.head.timerEventType)
    {
         for(i=0;i<MG_DEVICE_MAX;i++)
        {

            if(MG_ID_INVALID==timer_ob.item.devItem[i].devID)
                continue;

            if(MG_MASTER_ID==timer_ob.item.devItem[i].devID)
            {
                dev_master_ob_p->dev_ops.dev_op_dp_ctrl(DEVICE_MASTER,(void *)dev_master_ob_p,(void *)timer_ob.item.devItem[i].devDP);

            }
            else
            {
                dev_sub_ob_p=device_sub_object_get(timer_ob.item.devItem[i].devID);
                if(NULL==dev_sub_ob_p)
                    return;
                dev_master_ob_p->dev_ops.dev_op_dp_ctrl(DEVICE_SUB,(void *)dev_sub_ob_p,(void *)timer_ob.item.devItem[i].devDP);

            }
        }
    }
    else if(TIMER_SCENE==timer_ob.head.timerEventType)
    {
        scene_object_exe(timer_ob.item.sceneCode);
    }



}


static void timerMinuteRunTask(void *arg)
{
    uint8_t     timerID = MG_ID_INVALID;
    MG_TIME_DATA  systime;
    TIMER_HEAD   *timer_head_p=NULL;
    static MG_TIME_DATA  curtime={
        .minute=100
        }; //初始化一个无效数

    mg_rtc_get_time(&systime);

    if(systime.year<2018)	//当前年份不是 设置的年份，直接退出， 不处理
        return;

    if(systime.minute==curtime.minute)
        return;

    curtime=systime;		//记录系统当前时间

    if(systime.hour==0 && systime.minute==0)	//0小时0分代表新的一天开始，就要更新天文时间(当天)
    {
        astronomical_today_flush(&systime);
    }

    mg_log("%d-%d-%d %d:%d:%d  %d",curtime.year,curtime.month,curtime.day,curtime.hour,curtime.minute,curtime.second,curtime.week);
    mg_log("cur_astronomical sunrise:%d:%d sunset:%d:%d ",cur_astronomical.sunrise_hr, \
        cur_astronomical.sunrise_min,cur_astronomical.sunset_hr,cur_astronomical.sunset_min);

    if(dmgGetInuse(timer_dmg) == 0)	//如果没有开启一个定时器
        return;

    while((timerID=dmgGetNextID(timer_dmg,timerID))!=MG_ID_INVALID)
    {

        timer_head_p=timer_object_get(timerID);	//acquire 	timer_dmg.value
        if(timer_head_p==NULL)
            continue;


        if(0==timer_head_p->timerStatus)			//定时器不处于开启状态
        {
            continue;
        }
        if(timer_condition_check(timer_head_p,&curtime)<0)
            continue;

        timer_item_exe(timer_head_p);

    }

}


void mg_timer_init()
{

    uint32_t    bitSet;
    int i;

    timer_dmg=dmgCreate(MG_TIMER_MAX,sizeof(TIMER_HEAD));
	mg_init_timer(&timer_task,  40000, timerMinuteRunTask, NULL);
	mg_start_timer(&timer_task);
	
    mg_eeprom_read(ASTIME_EE_ADDR,(uint8_t *)astronomical_time,sizeof(astronomical_time));

    astime_printf();


    mg_log("timer_init");

    bitSet=mg_kernel_timerbit_get();	//读取(前20位)定时器标志位

    for(i=0; i < MG_TIMER_MAX; i++)
    {

        if(bitSet & (0x01 << i))
        {


            memset(&timer_ob,0,sizeof(TIMER_OBJECT));
            mg_eeprom_read(TIMER_EE_ADDR+i*sizeof(TIMER_OBJECT), (uint8_t *)&timer_ob, sizeof(TIMER_OBJECT));

            if(timer_ob.head.timerID!=i)
                continue;

            if(dmgInsertID(timer_dmg,timer_ob.head.timerID)==MG_ID_INVALID)
                continue;
            timer_object_set(&timer_ob.head);		//将TIMER_HEAD 头部信息写入到timer_dmg


        }
    }


}

void astime_printf()
{
    int i;

    mg_log("astime_printf");

    for(i=0;i<MG_ASTRONOMICAL_TIME_MAX;i++)
    {
        mg_log("%d-%d-%d sunrise:%d:%d sunset:%d:%d ",astronomical_time[i].year, \
        astronomical_time[i].month,astronomical_time[i].day,astronomical_time[i].sunrise_hr,astronomical_time[i].sunrise_min,\
        astronomical_time[i].sunset_hr,astronomical_time[i].sunset_min);
    }

}



void timer_printf(TIMER_OBJECT *timer_ob_p)
{
    char timeStr[20];

    mg_log("timer_printf");
    mg_log("ID:%d",timer_ob_p->head.timerID);
    mg_log("timerCode:%s",timer_ob_p->head.timerCode);

    sprintf(timeStr,"%d-%d-%d %d:%d:%d",timer_ob_p->head.modifyTime.year,timer_ob_p->head.modifyTime.month,timer_ob_p->head.modifyTime.day, \
                                timer_ob_p->head.modifyTime.hour,timer_ob_p->head.modifyTime.minute,timer_ob_p->head.modifyTime.second);

    mg_log("modifyTime:%s",timeStr);

    mg_log("repeatType:%d",timer_ob_p->head.repeatType);

    mg_log("repeatDays:%d",timer_ob_p->head.repeatDays);

    mg_log("timerType:%d",timer_ob_p->head.timerType);
    mg_log("timerStatus:%d",timer_ob_p->head.timerStatus);

    mg_log("timerEventType:%d",timer_ob_p->head.timerEventType);

    mg_log("timerOffset:%d",timer_ob_p->head.timerOffset);

    sprintf(timeStr,"%d:%d:%d",timer_ob_p->head.timerTime.hour,timer_ob_p->head.timerTime.minute,timer_ob_p->head.timerTime.second);
    mg_log("timerTime:%s",timeStr);

}
