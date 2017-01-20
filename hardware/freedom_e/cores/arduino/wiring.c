/*-
 * Copyright (c) 2015 Marko Zec, University of Zagreb
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id$
 */

#include "Arduino.h"
#include "platform.h"
#include "encoding.h"

const struct variant_pin_map_s variant_pin_map[] = VARIANT_DIGITAL_PIN_MAP;
const uint32_t variant_pin_map_size = sizeof(variant_pin_map) / sizeof(struct variant_pin_map_s);

const volatile void * variant_pwm[] = VARIANT_PWM_LIST;
const uint32_t variant_pwm_size = sizeof(variant_pwm) / sizeof(uint32_t*);
  
uint32_t
millis(void)
{
  uint64_t x;
  rdmcycle(&x);
  return((x * 1000) / F_CPU);

}


uint32_t 
micros(void)
{
  uint64_t x;
  rdmcycle(&x);
  return (uint32_t) ((x * (1000000))/F_CPU);
}


void
delay(uint32_t dwMs)
{
  uint64_t current, later;
  rdmcycle(&current);
  later = current + dwMs * (F_CPU/1000);
  if (later > current) // usual case
    {
      while (later > current) {
	rdmcycle(&current);
      }
    }
  else // wrap. Though this is unlikely to be hit w/ 64-bit mcycle
    {
      while (later < current) {
	rdmcycle(&current);
      }
      while (current < later) {
	rdmcycle(&current);
      }
    }
}
