#include "mgKernel.h"

static SCENE_OBJECT secene_ob;

static DMG_OBJECT *scene_dmg=NULL;

void scene_printf(SCENE_OBJECT *scene_ob_p);


static  __inline SCENE_HEAD *scene_object_get(uint8_t sceneID)
{
    SCENE_HEAD *scene_head_p;

    scene_head_p=(SCENE_HEAD *)dmgGetValue(scene_dmg,sceneID);

    if(scene_head_p!=NULL)
    {
        if(sceneID == scene_head_p->sceneID)
            return scene_head_p;

    }

    return NULL;
}

static  __inline void scene_object_set(SCENE_HEAD *scene_head_p)
{
    dmgSetValue(scene_dmg,scene_head_p->sceneID,(void *)scene_head_p);

}

SCENE_HEAD *scene_object_search(char *sceneCode)
{
    uint8_t sceneID=MG_ID_INVALID;
    SCENE_HEAD *scene_head_p;

    if(sceneCode==NULL)
        return NULL;

    while((sceneID=dmgGetNextID(scene_dmg,sceneID))!=MG_ID_INVALID)
    {
        scene_head_p=scene_object_get(sceneID);
        if(NULL==scene_head_p)
            continue;

        if(strncmp(scene_head_p->sceneCode,sceneCode,MG_CODE_SIZE)==0)
            return scene_head_p;

    }

    return NULL;
}


int scene_object_add(SCENE_OBJECT *scene_ob_p)
{
    uint8_t sceneID;

    if(strlen(scene_ob_p->head.sceneCode)==0)
        return -1;

    sceneID=dmgGetID(scene_dmg);
    if(MG_ID_INVALID==sceneID)
        return -1;

    scene_ob_p->head.sceneID=sceneID;

    scene_object_set(&scene_ob_p->head);

    //save
    mg_eeprom_write(SCENE_EE_ADDR+sceneID*sizeof(SCENE_OBJECT), (uint8_t *)scene_ob_p, sizeof(SCENE_OBJECT));

    mg_kernel_scenebit_set(scene_dmg->bitSet);

   // scene_printf(scene_ob_p);


    return 0;
}


int scene_object_mod(SCENE_OBJECT *scene_ob_p )
{
    scene_object_set(&scene_ob_p->head);
    //save
    mg_eeprom_write(SCENE_EE_ADDR+scene_ob_p->head.sceneID*sizeof(SCENE_OBJECT), (uint8_t *)scene_ob_p, sizeof(SCENE_OBJECT));

    //scene_printf(scene_ob_p);
    return 0;
}


void scene_object_del(uint8_t sceneID )
{
    dmgDelID(scene_dmg,sceneID);
    mg_kernel_scenebit_set(scene_dmg->bitSet);
}

SCENE_OBJECT *scene_object_next(uint8_t sceneID)
{
    sceneID=dmgGetNextID(scene_dmg,sceneID);
    if(MG_ID_INVALID==sceneID)
        return NULL;

    return (SCENE_OBJECT *)scene_object_get(sceneID);
}

void scene_object_all_reset()
{
    if(scene_dmg->bitSet!=0)
    {
        scene_dmg->bitSet=0;
       // mg_kernel_scenebit_set(scene_dmg->bitSet);
    }
}


void scene_object_item_del(uint8_t    devID)
{

    uint8_t sceneID=MG_ID_INVALID;
    uint8_t i;
    uint8_t saveFlag;

    while((sceneID=dmgGetNextID(scene_dmg,sceneID))!=MG_ID_INVALID)
    {
        saveFlag=0;
        mg_eeprom_read(SCENE_EE_ADDR+sceneID*sizeof(SCENE_OBJECT),(uint8_t *)&secene_ob, sizeof(SCENE_OBJECT));

        for(i=0;i<MG_DEVICE_MAX;i++)
        {
            if(MG_ID_INVALID==secene_ob.devItem[i].devID)
                continue;

          //  mg_log("i:devID:%d\n",i,secene_ob.devItem[i].devID);

            if(devID==secene_ob.devItem[i].devID)
            {
                secene_ob.devItem[i].devID=MG_ID_INVALID;
                saveFlag ++;
            }

        }

        if(saveFlag>0)
            mg_eeprom_write(SCENE_EE_ADDR+sceneID*sizeof(SCENE_OBJECT), (uint8_t *)&secene_ob, sizeof(SCENE_OBJECT));


    }


}


void scene_object_exe(char  *sceneCode)
{
    SCENE_HEAD *scene_head_p;
    int i;
    DEV_SUB_OBJECT *dev_sub_ob_p;

    if(NULL==sceneCode)
        return;
    scene_head_p=scene_object_search(sceneCode);
    if(scene_head_p==NULL)
        return;
mg_log("scene_object_exe");

    if(NULL==dev_master_ob_p)
        return;
    mg_eeprom_read(SCENE_EE_ADDR+scene_head_p->sceneID*sizeof(SCENE_OBJECT),(uint8_t *)&secene_ob, sizeof(SCENE_OBJECT));



    for(i=0;i<MG_DEVICE_MAX;i++)
    {
        if(MG_ID_INVALID==secene_ob.devItem[i].devID)
            continue;

      //  mg_log("i:devID:%d\n",i,secene_ob.devItem[i].devID);

        if(MG_MASTER_ID==secene_ob.devItem[i].devID)
        {
            dev_master_ob_p->dev_ops.dev_op_dp_ctrl(DEVICE_MASTER,(void *)dev_master_ob_p,(void *)secene_ob.devItem[i].devDP);

        }
        else
        {
            dev_sub_ob_p=device_sub_object_get(secene_ob.devItem[i].devID);
            if(NULL==dev_sub_ob_p)
                continue;
      //  mg_log("dev_sub_ob_p ok");


            dev_master_ob_p->dev_ops.dev_op_dp_ctrl(DEVICE_SUB,(void *)dev_sub_ob_p,(void *)secene_ob.devItem[i].devDP);

        }
    }

}



void mg_scene_init()
{
    uint32_t    bitSet;
    int i;
    scene_dmg=dmgCreate(MG_SCENE_MAX,sizeof(SCENE_HEAD));//申请 DMG_OBJECT   +  MG_SCENE_MAX *SCENE_HEAD  内存空间

    bitSet=mg_kernel_scenebit_get();		//return mg_kernel_data.sceneBit;

    mg_log("scene_init");


    for(i=0; i < MG_SCENE_MAX; i ++)	
    {

        if(bitSet & (0x01 << i))
        {

            memset(&secene_ob,0,sizeof(SCENE_OBJECT));

            mg_eeprom_read(SCENE_EE_ADDR+i*sizeof(SCENE_OBJECT), (uint8_t *)&secene_ob, sizeof(SCENE_OBJECT));

            if(secene_ob.head.sceneID!=i)	//读出的sceneID 跟当前处理的 不一致
                continue;

            if(dmgInsertID(scene_dmg,secene_ob.head.sceneID)==MG_ID_INVALID)	//根据secene_ob.head.sceneID  设置scene_dmg
                continue;														///相应bitSet 位，并inUse++


            scene_object_set(&secene_ob.head);	//将scene_ob.head 写入相应scene_dmg.value 中
											//其中head 头部包含:  sceneID ,  flag,  res2,  scenecode[MG_CODE_SIZE]
											//modeifyTime
        }
    }


}


void scene_printf(SCENE_OBJECT *scene_ob_p)
{
    char timeStr[20];

    mg_log("scene_printf");

    mg_log("ID:%d",scene_ob_p->head.sceneID);
    mg_log("sceneCode:%s",scene_ob_p->head.sceneCode);

    sprintf(timeStr,"%d-%d-%d %d:%d:%d",scene_ob_p->head.modifyTime.year,scene_ob_p->head.modifyTime.month,scene_ob_p->head.modifyTime.day, \
                                scene_ob_p->head.modifyTime.hour,scene_ob_p->head.modifyTime.minute,scene_ob_p->head.modifyTime.second);

    mg_log("modifyTime:%s",timeStr);



}

