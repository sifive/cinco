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
*/

#define ARDUINO_MAIN
#include "Arduino.h"

void freedom_e300_specific_initialization(void)
{
  // set things to a known state before running program
  GPIO_REG(GPIO_port) = 0;
  GPIO_REG(GPIO_output_en) = 0;
  GPIO_REG(GPIO_input_en) = 0;
  GPIO_REG(GPIO_iof_en) = 0;
  GPIO_REG(GPIO_iof_sel) = 0;
  GPIO_REG(GPIO_rise_ie) = 0;
  GPIO_REG(GPIO_fall_ie) = 0;
  GPIO_REG(GPIO_high_ie) = 0;
  GPIO_REG(GPIO_low_ie) = 0;
  PWM0_REG(PWM_CFG) = 0;
  PWM1_REG(PWM_CFG) = 0;
  PWM2_REG(PWM_CFG) = 0;
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
