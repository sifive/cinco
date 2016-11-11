
#ifndef _VARIANT_FREEDOM_E300_
#define _VARIANT_FREEDOM_E300_


#include <stdint.h>

#define SIFIVE_FREEDOM_E300_PLATFORM
#include <freedom_e300/platform/platform.h>

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

//TODO: These use GPIO numbers, not pin numbers.
// Should translate from these to pins.
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

#define VARIANT_DIGITAL_PIN_MAP  {{.io_port = 0, .bit_pos = 16, .pwm_num = 0xF, .pwm_cmp_num = 0}, \
				  {.io_port = 0, .bit_pos = 17, .pwm_num = 0xF, .pwm_cmp_num = 0}, \
				  {.io_port = 0, .bit_pos = 18, .pwm_num = 0xF, .pwm_cmp_num = 0}, \
				  {.io_port = 0, .bit_pos = 19, .pwm_num = 1,  .pwm_cmp_num = 1}, \
				  {.io_port = 0, .bit_pos = 20, .pwm_num = 1,  .pwm_cmp_num = 0}, \
				  {.io_port = 0, .bit_pos = 21, .pwm_num = 1,  .pwm_cmp_num = 2}, \
				  {.io_port = 0, .bit_pos = 22, .pwm_num = 1,  .pwm_cmp_num = 3}, \
				  {.io_port = 0, .bit_pos = 23, .pwm_num = 0xF, .pwm_cmp_num = 0}, \
				  {.io_port = 0, .bit_pos = 0,  .pwm_num = 0,  .pwm_cmp_num = 0}, \
				  {.io_port = 0, .bit_pos = 1,  .pwm_num = 0,  .pwm_cmp_num = 1}, \
				  {.io_port = 0, .bit_pos = 2,  .pwm_num = 0,  .pwm_cmp_num = 2}, \
				  {.io_port = 0, .bit_pos = 3,  .pwm_num = 0, .pwm_cmp_num  = 3}, \
				  {.io_port = 0, .bit_pos = 4,  .pwm_num = 0xF, .pwm_cmp_num = 0}, \
				  {.io_port = 0, .bit_pos = 5,  .pwm_num = 0xF, .pwm_cmp_num = 0}, \
				  {.io_port = 0, .bit_pos = 8,  .pwm_num = 0xF, .pwm_cmp_num = 0}, \
				  {.io_port = 0, .bit_pos = 9,  .pwm_num = 0xF, .pwm_cmp_num = 0}, \
				  {.io_port = 0, .bit_pos = 10, .pwm_num = 2,   .pwm_cmp_num = 0}, \
				  {.io_port = 0, .bit_pos = 11, .pwm_num = 2,   .pwm_cmp_num = 1}, \
				  {.io_port = 0, .bit_pos = 12, .pwm_num = 2,   .pwm_cmp_num = 2}, \
				    {.io_port = 0, .bit_pos = 13, .pwm_num = 2,   .pwm_cmp_num = 3}}

#define VARIANT_PWM_LIST {PWM0_BASE_ADDR, PWM1_BASE_ADDR, PWM2_BASE_ADDR}

#endif 
