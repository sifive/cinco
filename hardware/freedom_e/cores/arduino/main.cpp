/*
  main.cpp - Main loop for Arduino sketches
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  This code has been modified by SiFive, Inc, in order to support
  the SiFive Freedom E300 Platform development boards.
*/

#define ARDUINO_MAIN
#include "Arduino.h"
#include "encoding.h"

extern uint32_t trap_entry;

#define cmb() __asm__ __volatile__ ("" ::: "memory")

uint32_t mtime_lo(void)
{
  return *(volatile uint32_t *)(CLINT_BASE_ADDR + CLINT_MTIME);
}

static void freedom_e300_clock_setup () {

  // This is a very coarse parameterization. To revisit in the future
  // as more chips and boards are added.
#ifdef FREEDOM_E300_HIFIVE1

  //Ensure that we aren't running off the PLL before we mess with it.
  if (PRCI_REG(PRCI_PLLCFG) & (PLL_SEL(1))) {
    //Make sure the HFROSC is running at its default setting
    PRCI_REG(PRCI_HFROSCCFG)  = ((ROSC_DIV(4)) | (ROSC_TRIM(16)) | (ROSC_EN(1)));
    while ((PRCI_REG(PRCI_HFROSCCFG) & (ROSC_RDY(1))) == 0) {
      cmb();
    }
    PRCI_REG(PRCI_PLLCFG) &= ~(PLL_SEL(1));
  }

  // Set PLL Source to be HFXOSC if available.
  uint32_t config_value = 0;
  
#if HAS_HFXOSC					  
  config_value |= (PLL_REFSEL(1));
#else
  // TODO: Read HFROSC Trim and set it appropriately
  // to achieve 16 MHz.
#endif

  if (F_CPU == 256000000UL) {						  

    // In case we are executing from QSPI,
    // (which is quite likely) we need to
    // set the QSPI clock divider appropriately
    // before boosting the clock frequency.
    // This is quite pessmistic.

    // Div = f_sck/2
    SPI0_REG(SPI_REG_SCKDIV) = 8;
    
    // Set DIV Settings for PLL
    // Both HFROSC and HFXOSC are modeled as ideal
    // 16MHz sources (assuming dividers are set properly for
    // HFROSC).
    // (Legal values of f_REF are 6-48MHz)
    
    // Set DIVR to divide-by-2 to get 8MHz frequency
    // (legal values of f_R are 6-12 MHz)
    
    config_value |= (PLL_BYPASS(1));
    config_value |= (PLL_R(0x1));
    
    // Set DIVF to get 512Mhz frequncy
    // There is an implied multiply-by-2, 16Mhz.
    // So need to write 32-1
    // (legal values of f_F are 384-768 MHz)
    config_value |= (PLL_F(0x1F));
    
    // Set DIVQ to divide-by-2 to get 256 MHz frequency
    // (legal values of f_Q are 50-400Mhz)
    config_value |= (PLL_Q(0x1));
    
    // Set our Final output divide to divide-by-1:
    PRCI_REG(PRCI_PLLDIV) = (PLL_FINAL_DIV_BY_1(1) | PLL_FINAL_DIV(0));

    PRCI_REG(PRCI_PLLCFG) = config_value;

    // Un-Bypass the PLL.
    PRCI_REG(PRCI_PLLCFG) &= ~(PLL_BYPASS(1) );

    // Wait for PLL Lock
    // Note that the Lock signal can be glitchy.
    // Need to wait 100 us
    // RTC is running at 32kHz. 
    // So wait 4 ticks of RTC.
    uint32_t now = mtime_lo();
    while ((mtime_lo() - now) <  4){
      cmb();
    }
    
    // Now it is safe to check for PLL Lock
    while ((PRCI_REG(PRCI_PLLCFG) & PLL_LOCK(1)) == 0){
      cmb();
    }

  } else { // if (F_CPU == 16000000UL) TODO: Other frequencies.

    // Bypass
    config_value |= (PLL_BYPASS(1));

    PRCI_REG(PRCI_PLLCFG) = config_value;
    
    // If we don't have an HFXTAL, this doesn't really matter.
    // Set our Final output divide to divide-by-1:
    PRCI_REG(PRCI_PLLDIV) = (PLL_FINAL_DIV_BY_1(1) | PLL_FINAL_DIV(0));
    
 
  }
  
  // Switch over to PLL Clock source
  PRCI_REG(PRCI_PLLCFG) |= (PLL_SEL(1));
  
#ifdef HAS_HFXOSC
  // Turn off the HFROSC
   PRCI_REG(PRCI_HFROSCCFG)  &= ~(ROSC_EN(1));
#endif
   
#ifdef HAS_LFROSC_BYPASS
   // Turn off the LFROSC
   AON_REG(AON_LFROSC) &= ~(ROSC_EN(1));
#endif

#endif
   
}

void freedom_e300_specific_initialization(void)
{

  write_csr(mtvec, &trap_entry);
  if (read_csr(misa) & (1 << ('F' - 'A'))) { // if F extension is present
    write_csr(mstatus, MSTATUS_FS); // allow FPU instructions without trapping
    write_csr(fcsr, 0); // initialize rounding mode, undefined at reset
  }
  
  freedom_e300_clock_setup();
  
}

/*
 * \brief Main entry point of Arduino application
 */
int main( void )
{
  //	init();
  // Compute F_CPU inverse, used for millis and micros functions.
  calc_inv(F_CPU/1000, &f_cpu_1000_inv);
  calc_inv(F_CPU/1000000, &f_cpu_1000000_inv);
  freedom_e300_specific_initialization();
  setup();
  
  do {
    loop();
    if (serialEventRun)
      serialEventRun();
  } while (1);
  
  return 0;
}
