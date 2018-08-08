/*
  Lightweight RGB LED library.

  Copyright (c) 2018 Western Digital Corporation or its affiliates.
  SPDX-License-Identifier: LGPL-2.1-only

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Written by Michael Zaidman <michael.zaidman@wdc.com>, 2018
*/

#include "RGBL.h"

RGBL::RGBL(int brightnessLow, int brightnessHigh, int redPin, int greenPin, int bluePin) {

  pin[RED] = redPin;
  pin[GREEN] = greenPin;
  pin[BLUE] = bluePin;

  lowLightLimit = brightnessLow;
  highLightLimit = brightnessHigh;

  redVal = highLightLimit;
  greenVal = lowLightLimit;
  blueVal = lowLightLimit;

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void RGBL::on(led_color led) {
  analogWrite(pin[led],mapVal(highLightLimit));
}

void RGBL::off(led_color led) {
  analogWrite(pin[led],mapVal(0));
}

void RGBL::offAll() {
  writeAll(0, 0, 0);
}

void RGBL::writeAll(int redVal, int greenVal, int blueVal) {
  analogWrite(pin[RED],mapVal(redVal));
  analogWrite(pin[GREEN],mapVal(greenVal));
  analogWrite(pin[BLUE],mapVal(blueVal));
}

void RGBL::blink(led_color led, int msec) {
  on(led);
  delay(msec/2);
  off(led);
  delay(msec/2);
}

void RGBL::fade(int msec)
{
  if(redVal > lowLightLimit && blueVal == lowLightLimit) {
    redVal--;
    greenVal++;
  }
  if(greenVal > lowLightLimit && redVal == lowLightLimit) {
    greenVal--;
    blueVal++;
  }
  if(blueVal > lowLightLimit && greenVal == lowLightLimit) {
    blueVal--;
    redVal++;
  }
  writeAll(redVal, greenVal, blueVal);
  delay(msec);
}
