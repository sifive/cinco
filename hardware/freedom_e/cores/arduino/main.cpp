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
#include "fe300prci/fe300prci_driver.h"

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
  
  // In case we are executing from QSPI,
  // (which is quite likely) we need to
  // set the QSPI clock divider appropriately
  // before boosting the clock frequency.
  // This is quite pessmistic.

  // fsck = fin / 2(div + 1)
  // 50 MHz is a reasonable limit.
  //  2(div + 1) = F_CPU/50MHz
  uint32_t div = (F_CPU + 100000000)/100000000 - 1;
  if (div > 3) {  // the default 
    SPI0_REG(SPI_REG_SCKDIV) = div;
  }

  // We make no effort in this code to make the SPI
  // faster for a slow clock.
  
  if (F_CPU == 256000000UL) {						  
       
    PRCI_use_pll(1,    //Use HFXTAL as the reference
		 0,    // Bypass = 0, really use the PLL
		 1,    // Set DIVR to divide-by-2 to get 8MHz frequency
		 0x1F, // Set DIVF to get 512Mhz frequency
		 1,    // Set DIVQ to divide-by-2 to get 256 MHz frequency
		 1,    // Set final divider to div-by-1.
		 -1,   // We don't care about HFROSC 
		 -1);
    
    
  }  else if (F_CPU == 16000000UL) {
    
    PRCI_use_hfxosc(1);
 
  } else {

    PRCI_set_hfrosctrim_for_f_cpu(F_CPU, PRCI_FREQ_CLOSEST);

  }
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
