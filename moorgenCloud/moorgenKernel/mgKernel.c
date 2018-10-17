#include "mgKernel.h"

#define MAGIC_CODE   "moorgen"

static MG_KERNEL_DATA mg_kernel_data;


uint16_t mg_kernel_mac_count_get()
{
    mg_kernel_data.sub_mac_count++;

    return mg_kernel_data.sub_mac_count;

}

void mg_kernel_scenebit_set(uint32_t scenebit)
{
    if(mg_kernel_data.sceneBit != scenebit)
    {
        mg_kernel_data.sceneBit=scenebit;

        //save
        mg_eeprom_write(KERNEL_EE_ADDR, (uint8_t *)&mg_kernel_data, sizeof(MG_KERNEL_DATA));
    }
}

uint32_t mg_kernel_scenebit_get()
{

    return mg_kernel_data.sceneBit;

}


void mg_kernel_devbit_set(uint32_t devbit)
{
    if(mg_kernel_data.devBit != devbit)
    {

        mg_kernel_data.devBit=devbit;

        //save
        mg_eeprom_write(KERNEL_EE_ADDR, (uint8_t *)&mg_kernel_data, sizeof(MG_KERNEL_DATA));
    }

}

uint32_t mg_kernel_devbit_get()
{

    return mg_kernel_data.devBit;

}


void mg_kernel_timerbit_set(uint32_t timerbit)
{

    if(mg_kernel_data.timerBit != timerbit)
    {

        mg_kernel_data.timerBit=timerbit;

        //save
        mg_eeprom_write(KERNEL_EE_ADDR, (uint8_t *)&mg_kernel_data, sizeof(MG_KERNEL_DATA));
    }

}

uint32_t mg_kernel_timerbit_get()
{

    return mg_kernel_data.timerBit;

}

void mg_kernel_all_reset()
{
    device_sub_object_all_reset();
    scene_object_all_reset();
    timer_object_all_reset();
    mg_kernel_data.sceneBit=0;
    mg_kernel_data.devBit=0;
    mg_kernel_data.timerBit=0;
    mg_eeprom_write(KERNEL_EE_ADDR, (uint8_t *)&mg_kernel_data, sizeof(MG_KERNEL_DATA));


}



void mg_kernel_init()
{
    mg_log("mg_kernel_init");
    mg_eeprom_read(KERNEL_EE_ADDR, (uint8_t *)&mg_kernel_data, sizeof(MG_KERNEL_DATA));
    if(memcmp(mg_kernel_data.magicCode,MAGIC_CODE,strlen(MAGIC_CODE))!=0)		//对比魔数
    {

        mg_log("mg_kernel first");


        memset(&mg_kernel_data,0,sizeof(MG_KERNEL_DATA));
        strcpy(mg_kernel_data.magicCode,MAGIC_CODE);
        mg_eeprom_write(KERNEL_EE_ADDR,(uint8_t *)&mg_kernel_data, sizeof(MG_KERNEL_DATA));//写入默认魔数
        
    }

    mg_log("sub_mac_count:%d",mg_kernel_data.sub_mac_count);
    mg_log("devBit:%ld",mg_kernel_data.devBit);
    mg_log("sceneBit:%ld",mg_kernel_data.sceneBit);
    mg_log("timerBit:%ld",mg_kernel_data.timerBit);

    mg_scene_init();//从EEPROM中读取scene相关数据，并更新到scene_dmg
    mg_timer_init();


}
