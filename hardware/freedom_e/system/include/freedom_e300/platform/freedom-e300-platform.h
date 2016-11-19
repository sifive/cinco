// See LICENSE for license details.

#ifndef _SIFIVE_GUAVA_PLATFORM_H
#define _SIFIVE_GUAVA_PLATFORM_H

/****************************************************************************
 * Platform definitions
 *****************************************************************************/

#define MASKROM_BASE_ADDR       _AC(0x00001000,UL)
// CONFIG_STRING_ADDR is defined in encoding.h; perhaps it should not be?
//#define CONFIG_STRING_ADDR      _AC(0x0000100C,UL)
#define TRAPVEC_TABLE_BASE_ADDR _AC(0x00001010,UL)
#define OTP_MMAP_ADDR           _AC(0x00020000,UL)
#define SPI0_MMAP_ADDR          _AC(0x20000000,UL)
#define AON_BASE_ADDR           _AC(0x10000000,UL)
#define PRCI_BASE_ADDR          _AC(0x10008000,UL)
#define OTP_BASE_ADDR           _AC(0x10010000,UL)
#define GPIO_BASE_ADDR          _AC(0x10012000,UL)
#define UART0_BASE_ADDR         _AC(0x10013000,UL)
#define UART1_BASE_ADDR         _AC(0x10023000,UL)
#define SPI0_BASE_ADDR          _AC(0x10014000,UL)
#define SPI1_BASE_ADDR          _AC(0x10024000,UL)
#define SPI2_BASE_ADDR          _AC(0x10034000,UL)
#define PWM0_BASE_ADDR          _AC(0x10015000,UL)
#define PWM1_BASE_ADDR          _AC(0x10025000,UL)
#define PWM2_BASE_ADDR          _AC(0x10035000,UL)

#define IOF0_SPI1_MASK          _AC(0x000007FC,UL)
#define SPI11_NUM_SS     (4)
#define IOF_SPI1_SS0          (2u)
#define IOF_SPI1_SS1          (8u)
#define IOF_SPI1_SS2          (9u)
#define IOF_SPI1_SS3          (10u)
#define IOF_SPI1_MOSI         (3u)
#define IOF_SPI1_MISO         (4u)
#define IOF_SPI1_SCK          (5u)
#define IOF_SPI1_DQ0          (3u)
#define IOF_SPI1_DQ1          (4u)
#define IOF_SPI1_DQ2          (6u)
#define IOF_SPI1_DQ3          (7u)

#define IOF0_SPI2_MASK          _AC(0xFC000000,UL)
#define SPI2_NUM_SS       (1)
#define IOF_SPI2_SS0          (26u)
#define IOF_SPI2_MOSI         (27u)
#define IOF_SPI2_MISO         (28u)
#define IOF_SPI2_SCK          (29u)
#define IOF_SPI2_DQ0          (27u)
#define IOF_SPI2_DQ1          (28u)
#define IOF_SPI2_DQ2          (30u)
#define IOF_SPI2_DQ3          (31u)

//#define IOF0_I2C_MASK          _AC(0x00003000,UL)

#define IOF0_UART0_MASK         _AC(0x00030000, UL)
#define IOF_UART0_RX   (16u)
#define IOF_UART0_TX   (17u)

#define IOF0_UART1_MASK         _AC(0x03000000, UL)
#define IOF_UART1_RX (24u)
#define IOF_UART1_TX (25u)

#define IOF1_PWM0_MASK          _AC(0x0000000F, UL)
#define IOF1_PWM1_MASK          _AC(0x00780000, UL)
#define IOF1_PWM2_MASK          _AC(0x00003C00, UL)

// Setting these correctly makes the PLIC
// initialization scripts
// run faster.
#define PLIC_NUM_SOURCES 53
#define PLIC_NUM_PRIORITIES 3

#define INT_RESERVED    0
#define INT_WDOGCMP     1
#define INT_RTCCMP      2
#define INT_UART0_BASE  3
#define INT_UART1_BASE  4
#define INT_SPI0_BASE   5
#define INT_SPI1_BASE   6
#define INT_SPI2_BASE   7
#define INT_GPIO_BASE   8
#define INT_PWM0_BASE   40
#define INT_PWM1_BASE   44
#define INT_PWM2_BASE   48


// helper macros
#define _REG32(p, i)    (*(volatile uint32_t *)((p) + (i)))
#define SPI0_REG(i)     _REG32(SPI0_BASE_ADDR, (i))
#define SPI1_REG(i)     _REG32(SPI1_BASE_ADDR, (i))
#define SPI2_REG(i)     _REG32(SPI2_BASE_ADDR, (i))
#define PLIC_REG(i)     _REG32(PLIC_BASE_ADDR, (i))
#define PWM0_REG(i)     _REG32(PWM0_BASE_ADDR, (i))
#define PWM1_REG(i)     _REG32(PWM1_BASE_ADDR, (i))
#define PWM2_REG(i)     _REG32(PWM2_BASE_ADDR, (i))
#define OTP_REG(i)      _REG32(OTP_BASE_ADDR,  (i))
#define AON_REG(i)      _REG32(AON_BASE_ADDR,  (i))
#define PRCI_REG(i)     _REG32(PRCI_BASE_ADDR, (i))
#define UART0_REG(i)    _REG32(UART0_BASE_ADDR,(i))
#define UART1_REG(i)    _REG32(UART0_BASE_ADDR,(i))
#define GPIO_REG(i)     _REG32(GPIO_BASE_ADDR, (i))

#endif /* _SIFIVE_GUAVA_PLATFORM_H */
