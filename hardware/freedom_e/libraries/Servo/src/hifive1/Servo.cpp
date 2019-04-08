/*
  This is a port of the Servo library for the HiFive1

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

  This is a port of the Servo library https://github.com/arduino-libraries/Servo
  Copyright (c) 2016 Arduino. All right reserved.
  under GNU Lesser General Public License v2.1 or later (LGPL-2.1-or-later)
*/

#if defined(FREEDOM_E300_HIFIVE1)
#include <Arduino.h>
#include <Servo.h>

int pwmvalues[20] = {0}; // one for each pin for now!

static servo_t servos[MAX_SERVOS];       // static array of servo structures

uint8_t ServoCount = 0;                  // the total number of attached servos

Servo::Servo()
{
  if (ServoCount < MAX_SERVOS)
    this->servoIndex = ServoCount++;  // assign a servo index to this instance
  else
    this->servoIndex = INVALID_SERVO; // too many servos
}

uint8_t Servo::attach(int pin)
{
  return this->attach(pin, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
}

uint8_t Servo::attach(int pin, int min, int max)
{
  if (this->servoIndex < MAX_SERVOS)
  {
    pinMode(pin, OUTPUT);                        // set servo pin to output
    servos[this->servoIndex].Pin.nbr = pin;
    this->min  = min;
    this->max  = max;
    servos[this->servoIndex].Pin.isActive = true;
    writeMicroseconds(DEFAULT_PULSE_WIDTH);
  }
  return this->servoIndex;
}

void Servo::detach()
{
  servos[this->servoIndex].Pin.isActive = false;
}

void Servo::write(int value)
{
  if (value < 0)
    value = 0;
  else if (value > 180)
    value = 180;
  value = map(value, 0, 180, this->min, this->max);

  writeMicroseconds(value);
}

void Servo::writeMicroseconds(int us_value)
{
  if (us_value < this->min)
    us_value = this->min;
  else if (us_value > this->max)
    us_value = this->min;

  int pin = servos[this->servoIndex].Pin.nbr;

  uint32_t pwmpin = digitalPinToBitMask(pin);

  // configure GPIO port for pin
  GPIO_REG(GPIO_IOF_EN)     |=  pwmpin;
  GPIO_REG(GPIO_IOF_SEL)    |=  pwmpin;
  GPIO_REG(GPIO_OUTPUT_XOR) |=  pwmpin;     // flip output

  unsigned char pwmscale = 10;  // 2^10=1024
  unsigned int  period = (0.020 * F_CPU) / 1024;  // 20ms period or freq=50Hz
  unsigned int  pulse =  (us_value * (F_CPU / 1000000)) / 1000; //  pulse width

  switch (pin)
  {
    case 3:
    //case 4:  // PWM_CMP0
    case 5:
    case 6:
      PWM1_REG(PWM_CFG)   = 0;  // disable pwm
      PWM1_REG(PWM_COUNT) = 0;  // counter increments at the F_CPU speed
      PWM1_REG(PWM_CMP0)  = period;   // pin 4
      if (pin == 3)        PWM1_REG(PWM_CMP1)  = pulse;   // pin 3
      else if (pin == 5)   PWM1_REG(PWM_CMP2)  = pulse;
      else if (pin == 6)   PWM1_REG(PWM_CMP3)  = pulse;
      PWM1_REG(PWM_CFG)   = pwmscale | PWM_CFG_ENALWAYS | PWM_CFG_ZEROCMP;
      break;

    //case 16:// GPIO10  PWM2_0   16  16bit J4
    case 17:  // GPIO11  PWM2_1   17
    case 18:  // GPIO12  PWM2_2   18        J3 & J4
    case 19:  // GPIO13  PWM2_3   19        J3 & J4
      PWM2_REG(PWM_CFG)   = 0;  // disable pwm
      PWM2_REG(PWM_COUNT) = 0;  // counter increments at the F_CPU speed
      PWM2_REG(PWM_CMP0)  = period;
      if (pin == 17)       PWM2_REG(PWM_CMP1)  = pulse;
      else if (pin == 18)  PWM2_REG(PWM_CMP2)  = pulse;
      else if (pin == 19)  PWM2_REG(PWM_CMP3)  = pulse;
      PWM2_REG(PWM_CFG)   = pwmscale | PWM_CFG_ENALWAYS | PWM_CFG_ZEROCMP;
      break;

    // 8bit PWM0 cannot generate the 50Hz frequency required for the pattern.
    //case 8: // GPIO0    PWM0_0  8   8bit  J3
    case 9:   // GPIO1    PWM0_1  9
    case 10:  // GPIO2    PWM0_2  10
    case 11:  // GPIO3    PWM0_3  11
      // 8bit PWM0 is not supported
      break;

    default:
      return;
  }
  pwmvalues[pin] = us_value;
}

int Servo::read() // return the value as degrees
{
  return map(readMicroseconds(), this->min, this->max, 0, 180);
}

int Servo::readMicroseconds()
{
  uint8_t pin = servos[this->servoIndex].Pin.nbr;
  return pwmvalues[pin];
}

bool Servo::attached()
{
  return servos[this->servoIndex].Pin.isActive;
}

#endif
