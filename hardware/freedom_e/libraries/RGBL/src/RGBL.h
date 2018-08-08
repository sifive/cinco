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

#ifndef RGBL_H
#define RGBL_H

#include "Arduino.h"

typedef enum {
  RED = 0,
  GREEN,
  BLUE,
  COLOR
} led_color;

class RGBL {
  int redPin;
  int greenPin;
  int bluePin;
  int redVal;
  int greenVal;
  int blueVal;
  int lowLightLimit;
  int highLightLimit;
  int pin[COLOR];
  int mapVal(unsigned char val) { return 255 - (unsigned char)val; }
public:
  RGBL(int brightnessLow=0, int brightnessHigh=90, int redPin=6, int greenPin=3, int bluePin=5);
  void on(led_color led);
  void off(led_color led);
  void offAll();
  void writeAll(int redVal, int greenVal, int blueVal);
  void blink(led_color led, int msec);
  void fade(int msec);
};

#endif
