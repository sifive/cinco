
#ifndef _VARIANT_FREEDOM_E300_
#define _VARIANT_FREEDOM_E300_


#include <stdint.h>

#define SIFIVE_FREEDOM_E300_PLATFORM
#define FREEDOM_E300
#define RISCV
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

// Should translate from these to pins.
#define PIN_SPI1_SCK    (13u)
#define PIN_SPI1_MISO   (12u)
#define PIN_SPI1_MOSI   (11u)
#define PIN_SPI1_SS0    (10u)
#define PIN_SPI1_SS1    (14u) // Note, this is not actually connected!
#define PIN_SPI1_SS2    (15u)
#define PIN_SPI1_SS3    (16u)

#define SS_PIN_TO_CS_ID(x) \
  ((x==PIN_SPI1_SS0 ? 0 :		 \
    (x==PIN_SPI1_SS1 ? 1 :		 \
     (x==PIN_SPI1_SS2 ? 2 :		 \
      (x==PIN_SPI1_SS3 ? 3 :		 \
       -1))))) 

#define SPI_REG(x) SPI1_REG(x)

// we only want to enable 3 peripheral managed SPI pins: SCK, MOSI, MISO
// CS pins can either be handled by hardware or bit banged as GPIOs

static const uint8_t SS   = PIN_SPI1_SS0;
static const uint8_t SS1  = PIN_SPI1_SS1;
static const uint8_t SS2  = PIN_SPI1_SS2;
static const uint8_t SS3  = PIN_SPI1_SS3;
static const uint8_t MOSI = PIN_SPI1_MOSI;
static const uint8_t MISO = PIN_SPI1_MISO;
static const uint8_t SCK  = PIN_SPI1_SCK;

static const uint32_t SPI_IOF_MASK = (1 << IOF_SPI1_SCK) | (1 << IOF_SPI1_MOSI) | (1 << IOF_SPI1_MISO);

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
#define VARIANT_NUM_PIN (20)

#define VARIANT_PWM_LIST {PWM0_BASE_ADDR, PWM1_BASE_ADDR, PWM2_BASE_ADDR}

#define VARIANT_NUM_PWM (3)
#define VARIANT_NUM_SPI (1)
// For interfacing with the onboard SPI Flash.
#define VARIANT_NUM_QSPI (1)
#define VARIANT_NUM_UART (1)

#endif 
