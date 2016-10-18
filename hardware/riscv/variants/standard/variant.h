
#ifndef _VARIANT_FREEDOM_E300_GUAVA_
#define _VARIANT_FREEDOM_E300_GUAVA_

#include <stdint.h>

#include <guava/platform/platform.h>

/*----------------------------------------------------------------------------
*        Headers
*----------------------------------------------------------------------------*/

#include "Arduino.h"
#ifdef __cplusplus
#include "UARTClass.h"
#endif

// helper macros
#define SPI_REG(i)      _REG32(SPI_BASE_ADDR, (i))
#define GPIO_REG(i)     _REG32(GPIO_BASE_ADDR, (i))
#define PLIC_REG(i)     _REG32(PLIC_BASE_ADDR, (i))

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

//#define IOF_MASK        IOF0_SPI1_MASK // AC(0x000007FC,UL)
// we only want to enable 3 peripheral managed SPI pins: SCK, MOSI, MISO
// CS pins can either be handled by hardware or bit banged as GPIOs

#define SPI_IOF_MASK        (0x00000038)
//#define OE_MASK         (0x00000028)
//#define IE_MASK         (0x00000010)
//#define PUE_MASK        (0x00000010)


static const uint8_t SS   = BOARD_SPI_SS0;
static const uint8_t SS1  = BOARD_SPI_SS1;
static const uint8_t SS2  = BOARD_SPI_SS2;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

//
///*
// * Analog pins
// */
//static const uint8_t A0  = 16;
//static const uint8_t A1  = 17;
//
///*
//** PWM pins
//*/
//static const uint8_t PWM0  = 14;
//static const uint8_t PWM1  = 15;
//
//
//#define VARIANT_DIGITAL_PIN_MAP \
//{ \
///* pin     port             bit  */ \
//  [ 0] = { PIN_TYPE_GPIO,    16, }, \
//  [ 1] = { PIN_TYPE_GPIO,    17, }, \
//  [ 2] = { PIN_TYPE_GPIO,    18, }, \
//  [ 3] = { PIN_TYPE_GPIO,    19, }, \
//  [ 4] = { PIN_TYPE_GPIO,    20, }, \
//  [ 5] = { PIN_TYPE_GPIO,    21, }, \
//  [ 6] = { PIN_TYPE_GPIO,    22, }, \
//  [ 7] = { PIN_TYPE_GPIO,    23, }, \
//  [ 8] = { PIN_TYPE_GPIO,    0, }, \
//  [ 9] = { PIN_TYPE_GPIO,    1, }, \
//  [10] = { PIN_TYPE_GPIO,    2,  }, \
//  [11] = { PIN_TYPE_GPIO,    3,  }, \
//  [12] = { PIN_TYPE_GPIO,    4,  }, \
//  [13] = { PIN_TYPE_GPIO,    5,  }, \
//  [14] = { PIN_TYPE_GPIO,    9,  }, \
//  [15] = { PIN_TYPE_GPIO,    10,  }, \
//  [16] = { PIN_TYPE_GPIO,    11, }, \
//  [17] = { PIN_TYPE_GPIO,    12, }, \
//  [18] = { PIN_TYPE_GPIO,    13, }, \
//}
#endif 
