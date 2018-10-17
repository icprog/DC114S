/**
 ******************************************************************************
 * @file    platform_adc.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide ADC driver functions.
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "platform.h"
#include "platform_logging.h"
#include "platform_peripheral.h"


/******************************************************
 *                    Constants
 ******************************************************/
static int8_t channel_num = 0;
volatile bool initialized = false;
/******************************************************
 *                   Enumerations
 ******************************************************/
enum adc_channel{
  adc_channel_0 = 0,
  adc_channel_1,
  adc_channel_2,
  adc_channel_3,
  adc_channel_4,
  adc_channel_5,
  adc_channel_6,
  adc_channel_7,
};
/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

/**
 * \brief ADC interrupt callback function.
 */
static void adc_end_conversion(void)
{
  switch(channel_num){
  case adc_channel_0:
    adc_channel_get_value(ADC, ADC_CHANNEL_0);
    break;
  case adc_channel_1:
    adc_channel_get_value(ADC, ADC_CHANNEL_1);
    break;
  case adc_channel_2:
    adc_channel_get_value(ADC, ADC_CHANNEL_2);
    break;
  case adc_channel_3:
    adc_channel_get_value(ADC, ADC_CHANNEL_3);
    break;
  case adc_channel_4:
    adc_channel_get_value(ADC, ADC_CHANNEL_4);
    break;
  case adc_channel_5:
    adc_channel_get_value(ADC, ADC_CHANNEL_5);
    break;
  case adc_channel_6:
    adc_channel_get_value(ADC, ADC_CHANNEL_6);
    break;
  case adc_channel_7:
    adc_channel_get_value(ADC, ADC_CHANNEL_7);
    break;
  }
}


OSStatus platform_adc_init( const platform_adc_t* adc, uint32_t sample_cycle )
{
  OSStatus    err = kNoErr;
  struct adc_config adc_cfg;
  UNUSED_PARAMETER(sample_cycle);

  platform_mcu_powersave_disable();
  
  require_action_quiet( adc != NULL, exit, err = kParamErr);
  
  if( initialized != true )
  {
    adc_enable();
    
    adc_select_clock_source_mck(ADC);
    
    adc_get_config_defaults(&adc_cfg);
    
    adc_init(ADC, &adc_cfg);
    
    adc_set_trigger(ADC, ADC_TRIG_SW);
    
     adc_set_resolution(ADC, adc->resolution);
    
    initialized = true;
  }
  
exit:
  platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_adc_take_sample( const platform_adc_t* adc, uint16_t* output )
{
  OSStatus    err = kNoErr;
 
  platform_mcu_powersave_disable();
  
  require_action_quiet( adc != NULL, exit, err = kParamErr);
  
  channel_num = adc->channel;
  
  adc_channel_enable(ADC, adc->channel);
    
  adc_set_callback(ADC, adc->interrupt, adc_end_conversion, 1);
  
  /* Start conversion */
  adc_start_software_conversion(ADC);
  adc_start_calibration(ADC);
  
  while (adc_get_interrupt_status(ADC) & (1 << adc->channel));
  
  *output = adc_channel_get_value(ADC, adc->channel);	
  mico_thread_msleep(1);
  adc_channel_disable(ADC, adc->channel);
  
exit:
  platform_mcu_powersave_enable();
  return err;  
}

OSStatus platform_adc_take_sample_stream( const platform_adc_t* adc, void* buffer, uint16_t buffer_length )
{
  UNUSED_PARAMETER(adc);
  UNUSED_PARAMETER(buffer);
  UNUSED_PARAMETER(buffer_length);
  platform_log("unimplemented");
  return kUnsupportedErr;
}

OSStatus platform_adc_deinit( const platform_adc_t* adc )
{
  OSStatus    err = kNoErr;
  
  platform_mcu_powersave_disable();
  
  require_action_quiet( adc != NULL, exit, err = kParamErr);
  
  adc_disable();
  
  initialized = false;
  
exit:
  platform_mcu_powersave_enable();
  return err;  
}
