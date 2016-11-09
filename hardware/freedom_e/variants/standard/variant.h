
#ifndef _VARIANT_FREEDOM_E300_
#define _VARIANT_FREEDOM_E300_


#include <stdint.h>

#define FREEDOM_E300_PLATFORM
#include <freedom_e300/platform/platform.h>

// helper macros
#define _REG32(p, i)    (*(volatile uint32_t *)((p) + (i)))
#define SPI_REG(i)      _REG32(SPI_BASE_ADDR, (i))
#define GPIO_REG(i)     _REG32(GPIO_BASE_ADDR, (i))
#define PLIC_REG(i)     _REG32(PLIC_BASE_ADDR, (i))
#define PWM0_REG(i)     _REG32(PWM0_BASE_ADDR, (i))
#define PWM1_REG(i)     _REG32(PWM1_BASE_ADDR, (i))
#define PWM2_REG(i)     _REG32(PWM2_BASE_ADDR, (i))

/*----------------------------------------------------------------------------
*        Headers
*----------------------------------------------------------------------------*/

#include "Arduino.h"
#ifdef __cplusplus
#include "UARTClass.h"
#endif


/* LEDs */
#define PIN_LED_13          6 
#define PIN_LED             6 
#define LED_BUILTIN         6 

#ifdef __cplusplus
extern UARTClass Serial;
#endif

/*
 * SPI Interfaces
 */

#define SPI_INTERFACES_COUNT 1

//#define SPI_INTERFACE        (0)
#define SPI_INTERFACE_ID     (0)
#define SPI_CHANNELS_NUM     3
#define PIN_SPI_SS0          (2u)
#define PIN_SPI_SS1          (9u)
#define PIN_SPI_SS2          (10u)
#define PIN_SPI_MOSI         (3u)
#define PIN_SPI_MISO         (4u)
#define PIN_SPI_SCK          (5u)
#define BOARD_SPI_SS0        (10u)
#define BOARD_SPI_SS1        (14u)
#define BOARD_SPI_SS2        (15u)
#define BOARD_SPI_DEFAULT_SS BOARD_SPI_SS0

#define BOARD_PIN_TO_SPI_PIN(x) \
        (x==BOARD_SPI_SS0 ? PIN_SPI_SS0 : \
        (x==BOARD_SPI_SS1 ? PIN_SPI_SS1 : PIN_SPI_SS2))

#define BOARD_PIN_TO_SPI_CHANNEL(x) \
        (x==BOARD_SPI_SS0 ? 0 : \
        (x==BOARD_SPI_SS1 ? 1 : 2))

#define SPI_BASE_ADDR   SPI1_BASE_ADDR
#define INT_SPI         INT_SPI1_BASE

// we only want to enable 3 peripheral managed SPI pins: SCK, MOSI, MISO
// CS pins can either be handled by hardware or bit banged as GPIOs

#define SPI_IOF_MASK        (0x00000038)

static const uint8_t SS   = BOARD_SPI_SS0;
static const uint8_t SS1  = BOARD_SPI_SS1;
static const uint8_t SS2  = BOARD_SPI_SS2;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

#endif 
