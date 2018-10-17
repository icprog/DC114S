#include "mico.h"
#include "product.h"
#include "moorgenService.h"


#define LED_TRIGGER_INTERVAL 100 
#define LED_TRIGGER_INTERVAL_AFTER_CONNECT 500 

static mico_timer_t _Led_EL_timer;
static bool _Led_EL_timer_initialized = false;
static int current_status=0;

static void led_EL_Timeout_handler( void* arg )
{
  (void)(arg);
  MicoGpioOutputTrigger((mico_gpio_t)MICO_SYS_LED);
}

void dt72_led_high_speed_trigger()
{
      /*Led trigger*/
    if(_Led_EL_timer_initialized == true)
    {
        mico_stop_timer(&_Led_EL_timer);
        mico_deinit_timer( &_Led_EL_timer );
        _Led_EL_timer_initialized = false;
    }

    mico_init_timer(&_Led_EL_timer, LED_TRIGGER_INTERVAL, led_EL_Timeout_handler, NULL);
    mico_start_timer(&_Led_EL_timer);
    _Led_EL_timer_initialized = true;

    return;
}

void dt72_led_high_slow_trigger()
{
      /*Led trigger*/
    if(_Led_EL_timer_initialized == true)
    {
        mico_stop_timer(&_Led_EL_timer);
        mico_deinit_timer( &_Led_EL_timer );
        _Led_EL_timer_initialized = false;
    }

    mico_init_timer(&_Led_EL_timer, LED_TRIGGER_INTERVAL_AFTER_CONNECT, led_EL_Timeout_handler, NULL);
    mico_start_timer(&_Led_EL_timer);
    _Led_EL_timer_initialized = true;

    return;
}

void dt72_led_on()
{
    if(_Led_EL_timer_initialized == true)
    {
        mico_stop_timer(&_Led_EL_timer);
        mico_deinit_timer( &_Led_EL_timer );
        _Led_EL_timer_initialized = false;
    }

    MicoSysLed(true);


}

void dt72_led_off()
{
    if(_Led_EL_timer_initialized == true)
    {
        mico_stop_timer(&_Led_EL_timer);
        mico_deinit_timer( &_Led_EL_timer );
        _Led_EL_timer_initialized = false;
    }

    MicoSysLed(false);


}

void product_led_set(int led_status )
{

    switch(led_status)
    {
    case LED_POWER_ON:
        dt72_led_off();
        current_status=led_status;
        break;
    case LED_WIF_CONFIG:
        dt72_led_high_speed_trigger();
        current_status=led_status;

        break;
    case LED_WIFI_CONNECTING:
        dt72_led_high_slow_trigger();
        current_status=led_status;

        break;
    case LED_WIFI_CONNECTED:
        dt72_led_on();
        current_status=led_status;
        break;
    case LED_MQTT_CONNECTED:
        dt72_led_off();
        current_status=led_status;
        break;
    case LED_MQTT_DISCONNECTED:
        if(current_status>=LED_MQTT_CONNECTED)
        {
            dt72_led_on();
            current_status=led_status;
        }
        break;
    case LED_HUB_UPDATE:
        break;
    default:
        break;
    }
}



