/*
  Copyright (c) 2011 Arduino.  All right reserved.
  Copyright (c) 2013 by Paul Stoffregen <paul@pjrc.com> (delayMicroseconds)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _WIRING_
#define _WIRING_

__BEGIN_DECLS

/**
 *
 */
extern void initVariant( void ) ;
extern void init( void ) ;

struct variant_pin_map_s {
  uint8_t io_port;
  uint8_t bit_pos;
  uint8_t pwm_num;
  uint8_t pwm_cmp_num;
};

extern const struct variant_pin_map_s variant_pin_map[];
extern const uint32_t variant_pin_map_size;

extern const volatile void* variant_pwm[];
extern const uint32_t variant_pwm_size;

/**
 * \brief Returns the number of milliseconds since the board began running the current program.
 *
 * \return Number of milliseconds since the program started (uint64_t)
 */
extern uint64_t millis( void ) ;

/**
 * \brief Returns the number of microseconds since the board began running the current program.
 *
 * \note There are 1,000 microseconds in a millisecond and 1,000,000 microseconds in a second.
 */
extern uint64_t micros( void ) ;

/**
 * \brief Pauses the program for the amount of time (in miliseconds) specified as parameter.
 * (There are 1000 milliseconds in a second.)
 *
 * \param dwMs the number of milliseconds to pause (uint64_t)
 */
extern void delay( uint64_t dwMs ) ;

/**
 * \brief Pauses the program for the amount of time (in microseconds) specified as parameter.
 *
 * \param dwUs the number of microseconds to pause (uint32_t)
 */

static inline void delayMicroseconds(uint32_t) __attribute__((always_inline, unused));
static inline void delayMicroseconds(uint32_t usec){
  // This could be tuned to be more precise for short delays.
  if (usec == 0) {
    return;
  }
  uint64_t endwait = micros() + usec;
  while(micros() < endwait){
  }
}
__END_DECLS

#endif /* _WIRING_ */
