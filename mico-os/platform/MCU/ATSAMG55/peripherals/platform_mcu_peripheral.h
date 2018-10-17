/**
 ******************************************************************************
 * @file    platform_mcu_peripheral.h
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide all the headers of functions for stm32f2xx platform
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#pragma once

#include "samg55.h"
#include "ioport.h"
#include "usart.h"
#include "spi.h"
#include "twi.h"
#include "efc.h"
#include "pdc.h"
#include "flexcom.h"
#include "rtt.h"
#include "supc.h"
#include "matrix.h"
#include "wdt.h"
#include "adc2.h"

#include "mico_rtos.h"
#include "RingBufferUtils.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#define CREATE_IOPORT_PIN(port, pin) ((port) * 32 + (pin))

#define PORTA       IOPORT_PIOA
#define PORTB       IOPORT_PIOB    
  
/**
 * \brief Set peripheral mode for IOPORT pins.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param port IOPORT port to configure
 * \param masks IOPORT pin masks to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_port_peripheral_mode(port, masks, mode) \
	do {\
		ioport_set_port_mode(port, masks, mode);\
		ioport_disable_port(port, masks);\
	} while (0)

/**
 * \brief Set peripheral mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_pin_peripheral_mode(pin, mode) \
	do {\
		ioport_set_pin_mode(pin, mode);\
		ioport_disable_pin(pin);\
	} while (0)

/******************************************************
 *                    Constants
 ******************************************************/

 /* GPIOA to I */
#define NUMBER_OF_GPIO_PORTS      (8)

#define NUMBER_OF_GPIO_IRQ_LINES  (7)

/* USART1 to 6 */
#define NUMBER_OF_UART_PORTS      (6)


/* Invalid UART port number */
#define INVALID_UART_PORT_NUMBER  (0xff)

 /* SPI1 to SPI3 */
#define NUMBER_OF_SPI_PORTS       (3)

/******************************************************
 *                   Enumerations
 ******************************************************/


/******************************************************
 *                 Type Definitions
 ******************************************************/

/* SPI port */
typedef Spi     platform_spi_port_t;
typedef Adc     platform_adc_port_t;
typedef Twi     platform_i2c_port_t;

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    ioport_pin_t    pin;
    bool            is_wakeup_pin;
    uint8_t         wakeup_pin_number; /* wakeup pin number: 0 .. 15                     */
    uint8_t         trigger;           /* wakeup trigger: IOPORT_SENSE_FALLING or RISING */

} platform_gpio_t;

 typedef struct
 {
    enum adc_channel_num          channel;
    enum adc_interrupt_source     interrupt;
    enum adc_resolution           resolution;
 } platform_adc_t;


typedef struct {
    uint8_t                uart_id;
    void*                  port;             /* Usart* or Uart*  */
    Flexcom*               flexcom_base;
    uint32_t               peripheral_id;
    const platform_gpio_t* tx_pin;           /* Tx pin           */
    ioport_mode_t          tx_pin_mux_mode;  /* Tx pin mux mode  */
    const platform_gpio_t* rx_pin;           /* Rx pin           */
    ioport_mode_t          rx_pin_mux_mode;  /* Tx pin mux mode  */
    const platform_gpio_t* cts_pin;          /* CTS pin          */
    ioport_mode_t          cts_pin_mux_mode; /* Tx pin mux mode  */
    const platform_gpio_t* rts_pin;          /* RTS pin          */
    ioport_mode_t          rts_pin_mux_mode; /* Tx pin mux mode  */
} platform_uart_t;


typedef struct
{
    platform_uart_t*           peripheral;
    ring_buffer_t*             rx_ring_buffer;
#ifndef NO_MICO_RTOS
    mico_semaphore_t           rx_complete;
    mico_semaphore_t           tx_complete;
    mico_mutex_t               tx_mutex;
    mico_semaphore_t           sem_wakeup;
#else
    volatile bool              rx_complete;
    volatile bool              tx_complete;
#endif
    volatile uint32_t          tx_size;
    volatile uint32_t          rx_size;
    volatile OSStatus          last_receive_result;
    volatile OSStatus          last_transmit_result;
} platform_uart_driver_t;

typedef struct
{
    uint8_t                spi_id;
    platform_spi_port_t*   port;                /* Peripheral         */
    Flexcom*               flexcom_base;
    uint32_t               peripheral_id;
    const platform_gpio_t* mosi_pin;            /* MOSI pin           */
    ioport_mode_t          mosi_pin_mux_mode;   /* MOSI pin mux mode  */
    const platform_gpio_t* miso_pin;            /* MISO pin           */
    ioport_mode_t          miso_pin_mux_mode;   /* MISO pin mux mode  */
    const platform_gpio_t* clock_pin;           /* CLOCK pin          */
    ioport_mode_t          clock_pin_mux_mode;  /* CLOCK pin mux mode */
} platform_spi_t;

typedef struct
{
    platform_spi_t*           peripheral;
    mico_mutex_t              spi_mutex;
} platform_spi_driver_t;

typedef struct
{
    uint8_t unimplemented;
} platform_spi_slave_driver_t;


typedef struct
{
    uint8_t unimplemented;
} platform_pwm_t;

typedef struct
{
    uint8_t                 i2c_id;
    platform_i2c_port_t*    port;
    Flexcom*                flexcom_base;
    uint32_t                peripheral_id;
    const platform_gpio_t*  sda_pin;
    ioport_mode_t           sda_pin_mux_mode;  
    const platform_gpio_t*  scl_pin;
    ioport_mode_t           scl_pin_mux_mode;
} platform_i2c_t;

typedef struct
{
    mico_mutex_t              i2c_mutex;
} platform_i2c_driver_t;

typedef struct
{
    uint32_t                   flash_type;
    uint32_t                   flash_start_addr;
    uint32_t                   flash_length;
    uint32_t                   flash_protect_opt;
} platform_flash_t;

typedef struct
{
    const platform_flash_t*    peripheral;
    mico_mutex_t               flash_mutex;
    volatile bool              initialized;
} platform_flash_driver_t;

/******************************************************
 *                 Global Variables
 ******************************************************/


/******************************************************
 *               Function Declarations
 ******************************************************/
OSStatus platform_gpio_irq_manager_init      ( void );
OSStatus platform_powersave_enable_wakeup_pin( const platform_gpio_t* gpio );


OSStatus platform_mcu_powersave_init         ( void );

OSStatus platform_rtc_init                   ( void );

OSStatus platform_gpio_peripheral_pin_init( const platform_gpio_t* gpio, ioport_mode_t pin_mode );

void     platform_uart_irq                   ( platform_uart_driver_t* driver );
void     platform_uart_tx_dma_irq            ( platform_uart_driver_t* driver );
void     platform_uart_rx_dma_irq            ( platform_uart_driver_t* driver );
void     platform_i2c_irq                    ( uint8_t i2c_id );



#ifdef __cplusplus
} /* extern "C" */
#endif





