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

static void freedom_e300_clock_setup () {

  // This is a very coarse parameterization. To revisit in the future
  // as more chips and boards are added.
#ifdef FREEDOM_E300_HIFIVE1
  volatile uint32_t * pllcfg     = (volatile uint32_t*)  (PRCI_BASE_ADDR + PRCI_pllcfg);
  volatile uint32_t * plldiv     = (volatile uint32_t*)  (PRCI_BASE_ADDR + PRCI_plldiv);
  volatile uint32_t * hfrosccfg  = (volatile uint32_t*)  (PRCI_BASE_ADDR + PRCI_hfrosccfg);
  volatile uint32_t * lfrosccfg  = (volatile uint32_t*)  (AON_BASE_ADDR + AON_LFROSC);
  volatile uint64_t * mtimecmp   = (volatile uint64_t*)  (CLINT_BASE_ADDR + CLINT_MTIMECMP_offset);
  volatile uint64_t * mtime      = (volatile uint64_t*)  (CLINT_BASE_ADDR + CLINT_MTIME_offset);


  //Ensure that we aren't running off the PLL before we mess with it.
  if (*pllcfg & (1 << PLL_SEL_offset)) {
    //Make sure the HFROSC is running at its default setting
    *hfrosccfg  = ((4 << ROSC_DIV_offset) | (16 << ROSC_TRIM_offset) | (1 << ROSC_EN_offset));
    while ((*hfrosccfg & (1 << ROSC_RDY_offset)) == 0) {}
    *pllcfg &= ~(1 << PLL_SEL_offset);
  }

  // Set PLL Source to be HFXOSC if available.
  uint32_t config_value = 0;
  
#ifdef HFXTAL					  
  config_value |= (1<<PLL_REFSEL_offset);
#else
  // TODO!!!: Setting trim of HFRSOC
#endif



  if (F_CPU == 256000000UL) {						  

    // In case we are executing from QSPI,
    // (which is quite likely) we need to
    // set the QSPI clock divider appropriately
    // before boosting the clock frequency.

    // Div = f_sck/2
    SPI0_REG(SPI_REG_SCKDIV) = 8;
    
    // Set DIV Settings for PLL
    // Both HFROSC and HFXOSC are modeled as ideal
    // 16MHz sources (assuming dividers are set properly for
    // HFROSC).
    // (Legal values of f_REF are 6-48MHz)
    
    // Set DIVR to divide-by-2 to get 8MHz frequency
    // (legal values of f_R are 6-12 MHz)
    
    config_value |= (1<<PLL_BYPASS_offset);
    config_value |= (0x1 << PLL_R_offset);
    
    // Set DIVF to get 512Mhz frequncy
    // There is an implied multiply-by-2, 16Mhz.
    // So need to write 32-1
    // (legal values of f_F are 384-768 MHz)
    config_value |= (0x1F << PLL_F_offset);
    
    // Set DIVQ to divide-by-2 to get 256 MHz frequency
    // (legal values of f_Q are 50-400Mhz)
    config_value |= (0x1 << PLL_Q_offset);
    
    // Set our Final output divide to divide-by-1:
    *plldiv = (1 << PLL_FINAL_DIV_BY_1_offset | 0 << PLL_FINAL_DIV_offset);

    *pllcfg = config_value;

    // Un-Bypass the PLL.
    *pllcfg &= ~(1 << PLL_BYPASS_offset );

    // Wait for PLL Lock
    // Note that the Lock signal can be glitchy.
    // Need to wait 100 us
    // RTC is running at 32kHz. 
    // So wait 4 ticks of RTC.
    uint64_t now = *mtime;
    uint64_t then = now + 4;

    while ((*mtime) < then){ 
    }
    
    // Now it is safe to check for PLL Lock
    while ((*pllcfg & (1 << PLL_LOCK_offset)) == 0) {
    }

  } else { // if (F_CPU == 16000000UL) TODO:  For all other frequencies, ignore the setting (for now).

    // Bypass
    config_value |= (1<<PLL_BYPASS_offset);

    *pllcfg = config_value;
    
    // If we don't have an HFXTAL, this doesn't really matter.
    // Set our Final output divide to divide-by-1:
    *plldiv = (1 << PLL_FINAL_DIV_BY_1_offset | 0 << PLL_FINAL_DIV_offset);
    
 
  }
  
  // Switch over to PLL Clock source
  *pllcfg |= (1 << PLL_SEL_offset);
  
#ifdef HFXTAL
  // Turn off the HFROSC
   * hfrosccfg  &= ~(1 << ROSC_EN_offset);
#endif
   
#ifdef LFCLKBYPASS
   // Turn off the LFROSC
   * lfrosccfg &= ~(1 << ROSC_EN_offset);
#endif

#endif
   
}

void freedom_e300_specific_initialization(void)
{

  freedom_e300_clock_setup();
  
}

/*
 * \brief Main entry point of Arduino application
 */
int main( void )
{
  //	init();
  freedom_e300_specific_initialization();
  setup();
  
  do {
    loop();
    if (serialEventRun)
      serialEventRun();
  } while (1);
  
  return 0;
}
