/* Arduino Slow Software I2C Master 
   Copyright (c) 2017 Bernhard Nebel.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 3 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
   USA
*/

#include <SlowSoftI2CMaster.h>

#if defined (FREEDOM_E300_HIFIVE1)
  #if (F_CPU == 320000000)
    #define SKEW 7
  #elif (F_CPU == 256000000)
    #define SKEW 9
  #else
    #define SKEW = 1
  #endif
#elif defined (__AVR_ATmega328P__)
  #define SKEW 23
#else
  #define SKEW = 1
#endif

SlowSoftI2CMaster::SlowSoftI2CMaster(uint8_t sda, uint8_t scl, bool pullup) {
  _sda = sda;
  _scl = scl;
  _pullup = pullup;
}

// Init function. Needs to be called once in the beginning.
// Returns false if SDA or SCL are low, which probably means 
// a I2C bus lockup or that the lines are not pulled up.
bool SlowSoftI2CMaster::i2c_init(void) {
  _delay = 2;
  digitalWrite(_sda, LOW);
  digitalWrite(_scl, LOW);
  setHigh(_sda);
  setHigh(_scl);
  if (digitalRead(_sda) == LOW || digitalRead(_scl) == LOW) return false;
  return true;
}

// Set clock functions: clock is in Hz.
// The function is used for setting a custom clock.
// SKEW is the deviation of the clock input from the desired clock.
// If input clock is below max I2C clock rate then _delay = 2.
// 2 is the lowest delay that doesn't produce errors during a stress test.
void SlowSoftI2CMaster::setClock(uint32_t clock)
{
  if(clock < (1000000 / SKEW)) // Maximum possible I2C clock rate
    _delay = (1000000 / clock) - SKEW;
  else
    _delay = 2;
}

// Start transfer function: <addr> is the 8-bit I2C address (including the R/W
// bit). 
// Return: true if the slave replies with an "acknowledge", false otherwise
bool SlowSoftI2CMaster::i2c_start(uint8_t addr) {
  setLow(_sda);
  delayMicroseconds(_delay);
  setLow(_scl);
  return i2c_write(addr);
}

// Try to start transfer until an ACK is returned
bool SlowSoftI2CMaster::i2c_start_wait(uint8_t addr) {
  long retry = I2C_MAXWAIT;
  while (!i2c_start(addr)) {
    i2c_stop();
    if (--retry == 0) return false;
  }
  return true;
}

// Repeated start function: After having claimed the bus with a start condition,
// you can address another or the same chip again without an intervening 
// stop condition.
// Return: true if the slave replies with an "acknowledge", false otherwise
bool SlowSoftI2CMaster::i2c_rep_start(uint8_t addr) {
  setHigh(_sda);
  setHigh(_scl);
  delayMicroseconds(_delay);
  return i2c_start(addr);
}

// Issue a stop condition, freeing the bus.
void SlowSoftI2CMaster::i2c_stop(void) {
  setLow(_sda);
  delayMicroseconds(_delay);
  setHigh(_scl);
  delayMicroseconds(_delay);
  setHigh(_sda);
  delayMicroseconds(_delay);
}

// Write one byte to the slave chip that had been addressed
// by the previous start call. <value> is the byte to be sent.
// Return: true if the slave replies with an "acknowledge", false otherwise
bool SlowSoftI2CMaster::i2c_write(uint8_t value) {
  for (uint8_t curr = 0X80; curr != 0; curr >>= 1) {
    if (curr & value) setHigh(_sda); else  setLow(_sda); 
    setHigh(_scl);
    delayMicroseconds(_delay);
    setLow(_scl);
  }
  // get Ack or Nak
  setHigh(_sda);
  setHigh(_scl);
  delayMicroseconds(_delay);
  uint8_t ack = digitalRead(_sda);
  setLow(_scl);
  delayMicroseconds(_delay);  
  setLow(_sda);
  return ack == 0;
}

// Read one byte. If <last> is true, we send a NAK after having received 
// the byte in order to terminate the read sequence. 
uint8_t SlowSoftI2CMaster::i2c_read(bool last) {
  uint8_t b = 0;
  setHigh(_sda);
  for (uint8_t i = 0; i < 8; i++) {
    b <<= 1;
    delayMicroseconds(_delay);
    setHigh(_scl);
    if (digitalRead(_sda)) b |= 1;
    setLow(_scl);
  }
  if (last) setHigh(_sda); else setLow(_sda);
  setHigh(_scl);
  delayMicroseconds(_delay);
  setLow(_scl);
  delayMicroseconds(_delay);  
  setLow(_sda);
  return b;
}

void SlowSoftI2CMaster::setLow(uint8_t pin) {
    noInterrupts();
    if (_pullup) 
      digitalWrite(pin, LOW);
    pinMode(pin, OUTPUT);
    interrupts();
}


void SlowSoftI2CMaster::setHigh(uint8_t pin) {
    noInterrupts();
    if (_pullup) 
      pinMode(pin, INPUT_PULLUP);
    else
      pinMode(pin, INPUT);
    interrupts();
}