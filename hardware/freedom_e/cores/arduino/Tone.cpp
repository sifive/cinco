/*
  A Tone Generator Library for the HiFive1

  Copyright (c) 2018 Western Digital Corporation or its affiliates.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

  SPDX-License-Identifier: LGPL-2.1-only

  This is a port of the Arduino Tone library
  https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/Tone.cpp
  Copyright (c) 2015 Brett Hagman
  under GNU Lesser General Public License v2.1 or later (LGPL-2.1-or-later)
*/

#include "Arduino.h"
#include "encoding.h"

// Set scaling exponent based on clk to support 20hz-20khz human hearing range
#if F_CPU == 16000000L
  #define PWM_SCALE_16 0x6  // 4-125000 Hz, precision loss after D10 (18795 Hz)
  #define PWM_SCALE_8 0x8   // 246-31250 Hz, precision loss after D8 (4699 Hz)
#elif F_CPU == 256000000L
  #define PWM_SCALE_16 0xA  // 4-125000 Hz
  #define PWM_SCALE_8 0xC   // 246-31250 Hz
#elif F_CPU == 320000000L
  #define PWM_SCALE_16 0xA  // 5-156250 Hz, precision loss after G10 (25088 Hz)
  #define PWM_SCALE_8 0xC   // 307-39062 Hz, precision loss after G8 (6272 Hz)
#else
  #define PWM_SCALE_16 0x0
  #define PWM_SCALE_8 0x0
#endif

// Lookup table of comparator offsets
static uint8_t pwm_cmp_offsets[4] = {
  PWM_CMP0,
  PWM_CMP1,
  PWM_CMP2,
  PWM_CMP3
};

// Which PWM pin currently has an active duration counting down
static volatile uint8_t timed_pin = 0;

void noTone(uint8_t pin) {
  uint32_t pwm_base_addr = (uint32_t) variant_pwm[variant_pin_map[pin].pwm_num];
  uint8_t pwm_cmp_offset = pwm_cmp_offsets[variant_pin_map[pin].pwm_cmp_num];

  _REG32(pwm_base_addr, PWM_CFG) = 0;
  _REG32(pwm_base_addr, PWM_COUNT) = 0;
  _REG32(pwm_base_addr, PWM_CMP0) = 0;
  _REG32(pwm_base_addr, pwm_cmp_offset) = 0;
  GPIO_REG(GPIO_IOF_SEL) &= ~digitalPinToBitMask(pin);
  GPIO_REG(GPIO_IOF_EN) &= ~digitalPinToBitMask(pin);
}

static void tone_duration_isr() {
  AON_REG(AON_RTCCMP) = 0xffffffff; // Clear interrupt by maxing out comparator
  AON_REG(AON_RTCCFG) &= ~(1 << 12);  // Clear rtcenalways
  noTone(timed_pin);
  timed_pin = 0;
}

void tone(uint8_t pin, unsigned int frequency, unsigned long duration) {
  uint8_t pwm_num = variant_pin_map[pin].pwm_num;
  uint8_t pwm_cmp_num = variant_pin_map[pin].pwm_cmp_num;
  // Do nothing if not a PWM pin, or frequency is invalid
  if (pwm_num > variant_pwm_size || pwm_cmp_num == 0 || frequency == 0) {
    return;
  }

  if (duration > 0) {
    // If timer is already active for another pin, stop the tone on that pin
    if (timed_pin != 0) {
      noTone(timed_pin);
    }
    // If rtcenalways set, RTC probably in use, so only start timer if cleared
    if (!(AON_REG(AON_RTCCFG) & (1 << 12))) {
      AON_REG(AON_RTCHI) = 0;
      AON_REG(AON_RTCLO) = 0;
      // Equivalent to round(32.768 * duration). Max duration is 131071999ms
      AON_REG(AON_RTCCMP) = (uint32_t) ((32768ULL * duration + 500) / 1000);
      AON_REG(AON_RTCCFG) = 1 << 12;  // RTC counter enable (rtcenalways)

      timed_pin = pin;
      attachInterrupt(INT_RTCCMP, tone_duration_isr, RISING);
      set_csr(mstatus, MSTATUS_MIE);
    }
  }

  uint8_t scale = (pin >= 9 && pin <= 11) ? PWM_SCALE_8 : PWM_SCALE_16;
  // Equivalent to round((F_CPU >> scale) / frequency)
  uint16_t threshold = ((F_CPU >> scale) + frequency / 2) / frequency;

  uint32_t pwm_base_addr = (uint32_t) variant_pwm[pwm_num];
  uint8_t pwm_cmp_offset = pwm_cmp_offsets[pwm_cmp_num];

  // Set I/O function to PWM
  GPIO_REG(GPIO_IOF_SEL) |= digitalPinToBitMask(pin);
  GPIO_REG(GPIO_IOF_EN) |= digitalPinToBitMask(pin);

  // PWMx_REG(offset) is a nicer macro but we need to pass in PWM base addrs
  // Set PWM registers with CMP0 as the period, and CMPx as the square wave
  _REG32(pwm_base_addr, PWM_CFG) = 0;
  _REG32(pwm_base_addr, PWM_COUNT) = 0;
  _REG32(pwm_base_addr, PWM_CFG) = (PWM_CFG_ZEROCMP | PWM_CFG_ENALWAYS | scale);
  // Set the period on CMP0, and 50% duty cycle on other comparator
  _REG32(pwm_base_addr, PWM_CMP0) = (uint16_t) threshold;
  _REG32(pwm_base_addr, pwm_cmp_offset) = (uint16_t) (threshold / 2);
}