#ifndef __MG_EEPROM_H__
#define __MG_EEPROM_H__


#include "mico.h"

#define AT24C_I2C_DEVICE     MICO_I2C_1
#define AT24C_I2C_ADDR       0x50  

#define AT24C_PAGESIZE       128

/*
#define SYSTEM_EE_ADDR  0   //128
#define KERNEL_EE_ADDR  128 //128            
#define DEVICE_EE_ADDR  256  // 2k  3072     
#define SCENE_EE_ADDR   3328     // 25k    25600
#define TIMER_EE_ADDR   28928    // 25k    25600
#define ASTIME_EE_ADDR  54528    //5K 5120
#define USER_EE_ADDR    59648
*/

#define SYSTEM_EE_ADDR  0   // 1k
#define KERNEL_EE_ADDR  1024 // 128     

#define DEVICE_EE_ADDR  1152  // 5k  5120  

#define SCENE_EE_ADDR   6272     // 25k    25600
#define TIMER_EE_ADDR   31872    // 25k    25600

#define ASTIME_EE_ADDR  57472    // 2K 2048
#define USER_EE_ADDR    59520



int mg_eeprom_init(void);
void mg_eeprom_read(uint16_t reg_addr, uint8_t *reg_data, uint16_t len);
int mg_eeprom_write(uint16_t reg_addr, uint8_t *reg_data, uint16_t len);


#endif
