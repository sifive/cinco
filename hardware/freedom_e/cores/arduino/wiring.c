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

// Efficient divide routines provided by Bruce Hoult on forums.sifive.com

int_inverse f_cpu_1000_inv;
int_inverse f_cpu_1000000_inv;

void calc_inv(uint32_t n, int_inverse * res){
  uint32_t one = ~0;
  uint32_t d = one/n;
  uint32_t r = one%n + 1;
  if (r >= n) ++d;
  if (d == 0) --d;
  uint32_t shift = 0;
  while ((d & 0x80000000) == 0){
    d <<= 1;
    ++shift;
  }
  res->n = n;
  res->mult = d;
  res->shift = shift;
}

inline uint32_t divide32_using_inverse(uint32_t n, int_inverse *inv){

 uint32_t d =  (uint32_t)(((uint64_t)n * inv->mult) >> 32);
   d >>= inv->shift;
  if (n - d*inv->n >= inv->n) ++d;
  return d;
}

// Almost full-range 64/32 divide.
// If divisor-1 has i bits, then the answer is exact for n of up to 64-i bits
// e.g. for divisors up to a million, n can have up to 45 bits
// On RV32IM with divide32_using_inverse inlines this uses 5 multiplies,
// 33 instructions, zero branches, 3 loads, 0 stores.
uint64_t divide64_using_inverse(uint64_t n, int_inverse *inv){
  uint32_t preshift = (31 - inv->shift) & 31;
  uint64_t d = (uint64_t)divide32_using_inverse(n >> preshift, inv) << preshift;
  uint32_t r = n - d * inv->n;
  d += divide32_using_inverse(r, inv);
  return d;
}


uint32_t
millis()
{
  uint64_t x;
  rdmcycle(&x);
  x = divide64_using_inverse(x, &f_cpu_1000_inv);
  return((uint32_t) (x & 0xFFFFFFFF));
}

uint32_t 
micros(void)
{
  uint64_t x;
  rdmcycle(&x);
  // For Power-of-two MHz F_CPU,
  // this compiles into a simple shift,
  // and is faster than the general solution.
#if F_CPU==16000000
  x = x / (F_CPU / 1000000);
#else
#if  F_CPU==256000000
  x = x / (F_CPU / 1000000);
#else
  x = divide64_using_inverse(x, &f_cpu_1000000_inv);
#endif
#endif
  return((uint32_t) (x & 0xFFFFFFFF));
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
