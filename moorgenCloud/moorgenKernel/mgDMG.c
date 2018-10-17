#include "mgDMG.h"




DMG_OBJECT *dmgCreate(size_t n,uint16_t valueSize)
{
    DMG_OBJECT *dmgOB;
    
    if ((dmgOB =(DMG_OBJECT *) malloc(sizeof(DMG_OBJECT)+n*valueSize)) == NULL)
        return NULL;

    dmgOB->valueSize=valueSize;
    dmgOB->total=n;
    dmgOB->inUse=0;
    dmgOB->bitSet=0x00;
    memset(dmgOB->value,0xff,n*valueSize);
    return dmgOB;
    
}


void dmgRelease(DMG_OBJECT *dmgOB)
{

    free(dmgOB);

}

uint8_t dmgGetID(DMG_OBJECT *dmgOB)
{
    uint8_t  i;
    uint32_t value = 0;

    if(NULL==dmgOB)
        return MG_ID_INVALID;
        
    if(dmgOB->inUse == dmgOB->total)   
        return MG_ID_INVALID;

    for(i = 0; i < 32; i ++)
    {
        value = 0x01 << i;
        if(0 == (dmgOB->bitSet & value))
            break;
    }
    if(i >= 32)
        return MG_ID_INVALID;

    if(i>=dmgOB->total)
        return MG_ID_INVALID;

        
    dmgOB->bitSet |= value;
    dmgOB->inUse ++;

    return i;
}



uint8_t dmgInsertID(DMG_OBJECT *dmgOB, uint8_t id)
{
    uint8_t  i;

    if(NULL==dmgOB)
        return MG_ID_INVALID;

    if(id>dmgOB->total)		//total = MG_SCENE_MAX(20)
        return MG_ID_INVALID;
    i = id & 0x1F;				//总共20个，需要5位来表示，获取当前的sceneID
    if(0 == (dmgOB->bitSet&(0x01<<i)))	//如果当前的场景位 还没被使用
    {
        dmgOB->bitSet |= (0x01<<i);
        dmgOB->inUse ++;

        return i;
    }
    return MG_ID_INVALID;				//当前场景位已经被使用，则不能使用该位

}


void dmgDelID(DMG_OBJECT *dmgOB, uint8_t id)
{
    uint8_t  iii;
    uint32_t value = 0x01;

    if(id >= dmgOB->total)
        return;

    iii = id%32;

    if(0 == (dmgOB->bitSet & (0x00000001<<iii)))
        return;

    value <<= iii;
    value = ~value;
    
    dmgOB->bitSet &= value;
    if(dmgOB->inUse > 0)
       dmgOB->inUse --;
    memset(dmgOB->value+id*dmgOB->valueSize,0xff,dmgOB->valueSize);
    
    return;
}




uint8_t dmgGetNextID(DMG_OBJECT *dmgOB, uint8_t id)
{
    uint8_t j;

   
//printf("dmgGetNextID\r\n");

    if(id==MG_ID_INVALID)
    {
        j=0;
    }
    else
    {
        if(id >= dmgOB->total)
            return MG_ID_INVALID;
            
        j=id+1;

    }    
    
//printf("total:%d\r\n",dmgOB->total);

    for(; j < 32; j ++)
    {
//printf("j:%d\r\n",j);

        if(j >= dmgOB->total)
            return MG_ID_INVALID;
    
        if(dmgOB->bitSet & (0x01 << j))
        {

            return j;
        }
    }

 
    return MG_ID_INVALID;
}



void dmgSetValue(DMG_OBJECT *dmgOB, uint8_t id, void *value )
{
    if((id>=dmgOB->total)||(id==MG_ID_INVALID))
        return;

    memset(dmgOB->value+id*dmgOB->valueSize,0,dmgOB->valueSize);


    memcpy(dmgOB->value+id*dmgOB->valueSize,value,dmgOB->valueSize);

   
}

void *dmgGetValue(DMG_OBJECT *dmgOB, uint8_t id)
{

    if((id>=dmgOB->total)||(id==MG_ID_INVALID))
        return NULL;

    return dmgOB->value+id*dmgOB->valueSize;
}



