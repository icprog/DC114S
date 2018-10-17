#ifndef _MG_DMG_H_
#define _MG_DMG_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


#define  MG_ID_INVALID  0xff
#define  MG_BIT_FULL    0xffffffff


typedef struct tagDMG_OBJECT{
    uint8_t     total;
    uint8_t     inUse;
    uint16_t    valueSize;
    uint32_t    bitSet;
    uint8_t     value[0];
}DMG_OBJECT;


#define dmgGetInuse(dmgOB)  (dmgOB->inUse)

DMG_OBJECT *dmgCreate(size_t n,uint16_t valueSize);
void dmgRelease(DMG_OBJECT *dmgOB);
uint8_t dmgGetID(DMG_OBJECT *dmgOB);
uint8_t dmgInsertID(DMG_OBJECT *dmgOB, uint8_t id);
void dmgDelID(DMG_OBJECT *dmgOB, uint8_t id);
void *dmgGetValue(DMG_OBJECT *dmgOB, uint8_t id);
void dmgSetValue(DMG_OBJECT *dmgOB, uint8_t id, void *value);
uint8_t dmgGetNextID(DMG_OBJECT *dmgOB, uint8_t id);



#endif

