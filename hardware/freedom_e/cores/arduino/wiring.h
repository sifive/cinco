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

extern void initVariant(void);
extern void init(void);

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

typedef struct {
  uint32_t n;
  uint32_t mult;
  uint32_t shift;
} int_inverse;

extern int_inverse f_cpu_1000_inv;
extern int_inverse f_cpu_1000000_inv;

void calc_inv(uint32_t n, int_inverse * res);

uint32_t divide32_using_inverse(uint32_t n, int_inverse *inv);
uint64_t divide64_using_inverse(uint64_t n, int_inverse *inv);

#define rdmcycle(x) {                                         \
  uint32_t lo, hi, hi2;                                       \
  __asm__ __volatile__ ("1:\n\t"                              \
                        "csrr %0, mcycleh\n\t"                \
                        "csrr %1, mcycle\n\t"                 \
                        "csrr %2, mcycleh\n\t"                \
                        "bne  %0, %2, 1b\n\t"                 \
                        : "=r" (hi), "=r" (lo), "=r" (hi2)) ; \
  *(x) = lo | ((uint64_t) hi << 32);                          \
}

/**
 * Use asm to prevent GCC from performing loop unrolling even though the flags
 * corresponding to this behaviour explicitly say don't do so. Otherwise, innocent
 * call to the inlined delayMicroseconds(10) is expressed in about 0.35KB of
 * code size.
 */
#define usdelay16mz(usec) {                                   \
  uint32_t cnt = usec;                                        \
  __asm__ __volatile__ ("1:\n\t"                              \
                       "nop\n\t"                              \
                       "nop\n\t"                              \
                       "nop\n\t"                              \
                       "nop\n\t"                              \
                       "nop\n\t"                              \
                       "nop\n\t"                              \
                       "nop\n\t"                              \
                       "nop\n\t"                              \
                       "nop\n\t"                              \
                       "nop\n\t"                              \
                       "nop\n\t"                              \
                       "addi  %[cnt], %[cnt], -1\n\t"         \
                       "bnez  %[cnt], 1b\n\t"                 \
                        : [cnt] "+r" (cnt));                  \
}

/**
 * \brief Returns the number of milliseconds since the board began running the current program.
 *
 * \return Number of milliseconds since the program started (uint32_t)
 */
extern uint32_t millis(void);

/**
 * \brief Returns the number of microseconds since the board began running the current program.
 *
 * \note There are 1,000 microseconds in a millisecond and 1,000,000 microseconds in a second.
 */
extern uint32_t micros(void);

/**
 * \brief Pauses the program for the amount of time (in miliseconds) specified as parameter.
 * (There are 1000 milliseconds in a second.)
 *
 * \param dwMs the number of milliseconds to pause (uint32_t)
 */
extern void delay(uint32_t dwMs);

/**
 * \brief Pauses the program for the amount of time (in microseconds) specified as parameter.
 *
 * \param dwUs the number of microseconds to pause (uint32_t)
 */

static inline void delayMicroseconds(uint32_t) __attribute__((always_inline, unused));
static inline void delayMicroseconds(uint32_t usec) {
  if (usec == 0) {
    return;
  }
  uint64_t current, later;
#if F_CPU==16000000
  if (usec <= 4) {
    /**
     * The rdmcycle() takes 10 cycles or ~630ns to complete at 16MHz CPU clk.
     * Together with 64bit arithmetics it led to low accuracy and high jitter
     * for below 4 usec delay values. Replacing the rdmcycle() polling with
     * "nop" sequence improved the situation.
     *
     * Test results (excluding GPIO overhead of about 1.3us x 2 when measured
     * with "GPIO_REG(GPIO_OUTPUT_VAL) ^= (1 << PIN_3_OFFSET)")
     *
     * usec      "nop"        "rdmcycle()"    "rdmcycle() w/o overhead"
     *  1         1.13         1.6 - 2.2        1.6 - 2.2
     *  2         2.3          2.9 - 3.1        2.2 - 2.4
     *  3         3.1          4.07             2.9 - 3.5
     *  4         4            5.2              3.8 - 4
     *  5         4.8          5.8              5.2
     *  10        9            10.8             10.2
     */
     usdelay16mz(usec);
  } else {
#endif
    rdmcycle(&current);
    // Minus 12 cycles to compensate rdmcycle() loop overhead
    later = current + usec * (F_CPU/1000000) - 12;
    while (later > current)
      rdmcycle(&current);
#if F_CPU==16000000
  }
#endif
}

#define rdcyclelo() ({                        \
  uint32_t lo;                                \
  __asm__ __volatile__ ("csrr %0, mcycle\n\t" \
                        : "=r" (lo));         \
  lo;                                         \
})

static inline void delayCycles(uint32_t) __attribute__((always_inline, unused));
static inline void delayCycles(uint32_t cycles) {
  volatile uint32_t current;
  uint32_t later;
  current = rdcyclelo();
  later = current + cycles;
  if (later > current) { // usual case
    while (later > current) {
      current = rdcyclelo();
    }
  } else { // wrap around case
    while (later < current) {
      current = rdcyclelo();
    }
    while (current < later) {
      current = rdcyclelo();
    }
  }
}

__END_DECLS

#endif /* _WIRING_ */
