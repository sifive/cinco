/*
  SimpleTouchscreen library.

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


  Tailored for SPI "2.8 TFT SPI 240x320 V1.1" ILI9341 based LCD display panel
  with XPT2046 touch screen controller.

  See README.md for how to calibrate for different panels.

  Tested with HiFive1 and Arduino Uno boards.
  Tested with Adafruit_ILI9341 and Adafruit_GFX.h LCD libraries.

  For HiFive1 we don't need 5V to 3.3V voltage converters, just set IOREF jumper
  to 3.3V. The pins were connected as following:

  TFTLCD     HiFive1
  ======     =======
  T_IRQ      D2
  T_DO       D12
  T_DI       D11
  T_CS       D7
  T_CLK      D13
  SDO(MISO)  D12
  LED        3.3V
  CLK        D13
  SDI(MOSI)  D11
  D/C        D9
  RESET      RESET
  CS         D10
  GND        GND
  VCC        3.3V

  Written by Michael Zaidman <michael.zaidman@wdc.com>, 2018
*/
#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include <SPI.h>
#include <Arduino.h>

// Default display geometry
#define LCD_LANDSCAPE_X 320
#define LCD_LANDSCAPE_Y 240

// Touch screen calibration for for SPI "2.8 TFT SPI 240x320 V1.1" LCD panel.
// See README.md to figure out how to calibrate library for different panels.
#define TS_LANDSCAPE_X_LO 390
#define TS_LANDSCAPE_Y_LO 220
#define TS_LANDSCAPE_X_HI 3920
#define TS_LANDSCAPE_Y_HI 3900

// Rotation positions, same encoding as in Adafruit_ILI9341 library.
#define PORTRATE_0    0
#define LANDSCAPE_90  1
#define PORTRATE_180  2
#define LANDSCAPE_270 3
#define IS_LANDSCAPE(rotation) (rotation & 1)

// Reading accuracy
#define SAMPLES 8 // keep it power of 2 + 4
#define GOOD_SAMPLES (SAMPLES - 4)

class SimpleTouchscreen {
  int16_t _x, _y;
  uint8_t _rotation;
  uint8_t _pinSS, _pinIRQ;

  // LCD landscape geometry (width and height resolution)
  uint16_t _lcdLandscapeX, _lcdLandscapeY;

  // Touch screen calibration
  uint16_t _tsLandscapeLowX, _tsLandscapeLowY;
  uint16_t _ts2lsdRatioX, _ts2lsdRatioY;

  uint16_t _get16(uint8_t cmd);
  void _round(int16_t *val, int16_t max);
  uint16_t _getX();
  uint16_t _getY();
  uint16_t _getZ();
  bool _calibrate;

public:
  SimpleTouchscreen( uint8_t tIRQ, uint8_t tSS) : _rotation(LANDSCAPE_270)
                              { _pinIRQ = tIRQ; _pinSS = tSS; }
  void setRotation(uint8_t n) { _rotation = n % 4; }

  void begin(bool calibrate);
  void begin(uint16_t tsLsLowX  = TS_LANDSCAPE_X_LO,
             uint16_t tsLsHighX = TS_LANDSCAPE_X_HI,
             uint16_t tsLsLowY  = TS_LANDSCAPE_Y_LO,
             uint16_t tsLsHighY = TS_LANDSCAPE_Y_HI,
             uint16_t lcdLsX    = LCD_LANDSCAPE_X,
             uint16_t lcdLsY    = LCD_LANDSCAPE_Y);

  bool dataAvailable();
  void read();
  uint16_t getX() { return (this->_x); };
  uint16_t getY() { return (this->_y); };
};

#endif
