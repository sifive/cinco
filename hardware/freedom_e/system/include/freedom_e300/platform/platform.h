// See LICENSE for license details.

#ifndef _FREEDOM_E300_PLATFORM_H
#define _FREEDOM_E300_PLATFORM_H

#include <freedom_e300/const.h>
#include <freedom_e300/clint.h>
#include <freedom_e300/uart.h>
#include <freedom_e300/gpio.h>
#include <freedom_e300/spi.h>
#include <freedom_e300/prci.h>
#include <freedom_e300/aon.h>
#include <freedom_e300/pwm.h>
#include <freedom_e300/drivers_sifive/plic.h>

// Some things missing from the official encoding.h
#define MCAUSE_INT         0x80000000
#define MCAUSE_CAUSE       0x7FFFFFFF

/****************************************************************************
 * Platform definitions
 *****************************************************************************/

// These things are fixed by rocket-chip
#define MEM_BASE_ADDR          _AC(0x80000000,UL)
#define CLINT_BASE_ADDR        _AC(0x02000000,UL)
#define PLIC_BASE_ADDR         _AC(0x0C000000,UL)

#ifdef SIFIVE_FREEDOM_E300_PLATFORM
#include "freedom-e300-platform.h"
#endif

#ifdef SIFIVE_FREEDOM_U500_PLATFORM
#include "freedom-u500-platform.h"
#endif

#ifdef SIFIVE_GUAVA_PLATFORM
#include "guava-platform.h"
#endif


#ifndef __ASSEMBLER__

//TODO: Add if we actually need this.
//void write_hex(int fd, uint32_t hex);

#endif /* !__ASSEMBLER__ */


#endif /* _SIFIVE_PLATFORM_H */
