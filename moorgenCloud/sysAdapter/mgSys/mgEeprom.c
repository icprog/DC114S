#include "mgEeprom.h"


#define at24c_log(M, ...) custom_log("at24c", M, ##__VA_ARGS__)

/* I2C device */
mico_i2c_device_t at24c_i2c_device = {
  AT24C_I2C_DEVICE, AT24C_I2C_ADDR, I2C_ADDRESS_WIDTH_7BIT, I2C_STANDARD_SPEED_MODE
};

static uint8_t array[AT24C_PAGESIZE +2];
static uint8_t checkBuf[200];
static mico_mutex_t i2c_mutex;		

static mico_i2c_message_t apds_i2c_msg = {NULL, NULL, 0, 0, 0, false};

OSStatus at24c_I2C_bus_write(uint16_t reg_addr, uint8_t *reg_data, uint16_t  cnt)
{
	OSStatus err = kNoErr;

    //at24c_log("at24c_I2C_bus_write");
   // at24c_log("reg_addr:%d",reg_addr);
   //at24c_log("cnt:%d",cnt);


    if(cnt>AT24C_PAGESIZE)
        return kGeneralErr;

	array[0] = (uint8_t)(reg_addr>>8);
	array[1] = (uint8_t)(reg_addr);

    if(cnt>0)
        memcpy(&array[2],reg_data,cnt);


	err = MicoI2cBuildTxMessage(&apds_i2c_msg, array, cnt + 2, 20);
	require_noerr( err, write_exit );

	err = MicoI2cTransfer(&at24c_i2c_device, &apds_i2c_msg, 1);
	require_noerr( err, write_exit );

write_exit:  

    at24c_log("write err:%d",err);  //²»ÄÜÈ¥µô
	return err;

	
}



OSStatus at24c_I2C_bus_read(uint16_t reg_addr,uint8_t *reg_data, uint16_t cnt)
{
	OSStatus err = kNoErr;

	array[0] = (uint8_t)(reg_addr>>8);
	array[1] = (uint8_t)(reg_addr);


	err = MicoI2cBuildCombinedMessage(&apds_i2c_msg, array,reg_data,2, cnt, 3);
	require_noerr( err, read_exit );

	err = MicoI2cTransfer(&at24c_i2c_device, &apds_i2c_msg, 1);
	require_noerr( err, read_exit );

read_exit:
     //at24c_log("read err:%d",err);

	return err;
}



int at24c_page_write(uint16_t reg_addr, uint8_t *reg_data, uint16_t cnt)
{
    uint8_t retry_times=0;
    while(1)
    {
        at24c_I2C_bus_write(reg_addr, reg_data,cnt);
        at24c_I2C_bus_read(reg_addr,checkBuf, cnt);
        if(cnt==1)
        {
            if(*reg_data!=*checkBuf)
            {
                retry_times++;
                if(retry_times>3)
                {
                    at24c_log("at24c_page_write fail");



                    return -1;
                }
                at24c_log("at24c_page_write ERROR");
                continue;
            }
        } 
        else
        {   
            if(memcmp(reg_data,checkBuf,cnt)!=0)
            {
                retry_times++;
                if(retry_times>3)
                {
                    at24c_log("at24c_page_write fail");    

                    return -1;
                }
                at24c_log("at24c_page_write ERROR");
                continue;
            }
        }

        break;
        
    }


    return 0;

}









OSStatus at24c_data_read(uint16_t reg_addr, uint8_t *reg_data, uint16_t cnt)
{
	OSStatus err = kNoErr;

    mico_rtos_lock_mutex(&i2c_mutex);

    //at24c_log("at24c_data_read");
   // at24c_log("reg_addr:%d",reg_addr);
   //at24c_log("cnt:%d",cnt);
	err = at24c_I2C_bus_read(reg_addr,reg_data, cnt);
    mico_rtos_unlock_mutex(&i2c_mutex);

	return err;
}




int mg_eeprom_write(uint16_t reg_addr, uint8_t *reg_data, uint16_t cnt)
{
    uint16_t NumOfPage = 0, NumOfSingle = 0, count = 0;
    uint16_t Addr = 0;
    OSStatus err = kGeneralErr;

    Addr = reg_addr % AT24C_PAGESIZE;
    count = AT24C_PAGESIZE - Addr;
    NumOfPage =  cnt / AT24C_PAGESIZE;
    NumOfSingle = cnt % AT24C_PAGESIZE;
 
    mico_rtos_lock_mutex(&i2c_mutex);

  /*!< If WriteAddr is sEE_PAGESIZE aligned  */
    if(Addr == 0) 
    {
        /*!< If NumByteToWrite < sEE_PAGESIZE */
        if(NumOfPage == 0) 
        {
            /* Start writing data */
            err=at24c_page_write(reg_addr,reg_data, NumOfSingle);
            require_noerr(err, exit);
        }
        /*!< If NumByteToWrite > sEE_PAGESIZE */
        else  
        {
            while(NumOfPage--)
            {
                err=at24c_page_write(reg_addr,reg_data, AT24C_PAGESIZE); 
                require_noerr(err, exit);

                reg_addr +=  AT24C_PAGESIZE;
                reg_data += AT24C_PAGESIZE;
            }

            if(NumOfSingle!=0)
            {
                err=at24c_page_write(reg_addr,reg_data, NumOfSingle);
                require_noerr(err, exit);
            }
        }
    }
  /*!< If WriteAddr is not sEE_PAGESIZE aligned  */
    else 
    {
        /*!< If NumByteToWrite < sEE_PAGESIZE */
        if(NumOfPage== 0) 
        {
            /*!< If the number of data to be written is more than the remaining space 
            in the current page: */
            if (cnt > count)
            {
       
                /*!< Write the data contained in same page */
                err=at24c_page_write(reg_addr,reg_data,count);
                require_noerr(err, exit);   
        
                /*!< Write the remaining data in the following page */
                err=at24c_page_write((reg_addr + count),(uint8_t*)(reg_data + count), (cnt - count));
                require_noerr(err, exit);   
     
            }      
            else      
            {       
                err=at24c_page_write(reg_addr,reg_data, NumOfSingle);
                require_noerr(err, exit);   
      
            }     
        }
        /*!< If NumByteToWrite > sEE_PAGESIZE */
        else
        {
            cnt -= count;
            NumOfPage =  cnt / AT24C_PAGESIZE;
            NumOfSingle = cnt % AT24C_PAGESIZE;

            if(count != 0)
            {  
     
                err=at24c_page_write(reg_addr,reg_data, count);
                require_noerr(err, exit);   

                reg_addr += count;
                reg_data += count;
            } 

            while(NumOfPage--)
            {
        
                err=at24c_page_write(reg_addr,reg_data, AT24C_PAGESIZE);
                require_noerr(err, exit);   
                reg_addr +=  AT24C_PAGESIZE;
                reg_data += AT24C_PAGESIZE;  
            }
            if(NumOfSingle != 0)
            {
                /* Store the number of data to be written */
                err=at24c_page_write(reg_addr,reg_data, NumOfSingle); 
                require_noerr(err, exit);   

            }
        }
    }


exit:

    mico_rtos_unlock_mutex(&i2c_mutex);

    return err;
}




int mg_eeprom_init(void)
{
	OSStatus err = kUnknownErr;

    mico_rtos_init_mutex(&i2c_mutex);
	// I2C init
	MicoI2cFinalize(&at24c_i2c_device);   // in case error

	err = MicoI2cInitialize(&at24c_i2c_device);
	require_noerr_action( err, exit, at24c_log("AT24C_ERROR: MicoI2cInitialize err = %d.", err) );
	if( false == MicoI2cProbeDevice(&at24c_i2c_device, 5) )
	{
		at24c_log("AT24C_ERROR: no i2c device found!");
		err = kNotFoundErr;
		goto exit;
	}
  	at24c_log("AT24C_OK");

exit:
	return err;
}



void mg_eeprom_read(uint16_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    int i,j;
    uint16_t addr=reg_addr;
    uint8_t *data=reg_data;
    i=len/AT24C_PAGESIZE;
    j=len%AT24C_PAGESIZE;

    while(i--)
    {
        at24c_data_read(addr, data, AT24C_PAGESIZE);

        data +=AT24C_PAGESIZE;
        addr +=AT24C_PAGESIZE;
            
    }

    if(j!=0)
    {
        at24c_data_read(addr, data, j);

    }

}



