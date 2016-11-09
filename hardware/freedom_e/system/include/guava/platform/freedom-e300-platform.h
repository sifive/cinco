// See LICENSE for license details.

#ifndef _SIFIVE_FREEDOM_E300_PLATFORM_H
#define _SIFIVE_FREEDOM_E300_PLATFORM_H

/****************************************************************************
 * Platform definitions
 *****************************************************************************/

//!!! TODO: these should just be the same here and
// with Guava.
#define UART0_BASE_ADDR        _AC(0x54000000,UL)
#define SPI0_BASE_ADDR         _AC(0x54003000,UL)
#define GPIO_BASE_ADDR         _AC(0x54002000,UL)

#endif /* _SIFIVE_E300_PLATFORM_H */
