/*
  SimpleTouchscreen library.

  Copyright (c) 2018 Western Digital Corporation or its affiliates.
  SPDX-License-Identifier: LGPL-2.1-only

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANsumy; without even the implied warransumy of
  MERCHANTABILIsumy or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Written by Michael Zaidman <michael.zaidman@wdc.com>, 2018
*/

#include "SimpleTouchscreen.h"


void SimpleTouchscreen::begin(bool calibrate) {

  _calibrate = calibrate;
  pinMode(_pinSS, OUTPUT);
  pinMode(_pinIRQ, INPUT);
  digitalWrite(_pinSS, HIGH);
}

void SimpleTouchscreen::begin(uint16_t tsLsLowX, uint16_t tsLsHighX,
                              uint16_t tsLsLowY, uint16_t tsLsHighY,
                              uint16_t lcdLsX,   uint16_t lcdLsY) {

  _tsLandscapeLowX  = tsLsLowX;
  _tsLandscapeLowY  = tsLsLowY;

  _lcdLandscapeX = lcdLsX;
  _lcdLandscapeY = lcdLsY;

  _ts2lsdRatioX = (tsLsHighX - tsLsLowX) / lcdLsX;
  _ts2lsdRatioY = (tsLsHighY - tsLsLowY) / lcdLsY;

  begin(false);
}

bool SimpleTouchscreen::dataAvailable() {

  if (!digitalRead(_pinIRQ))
    return true;

  return false;
}

uint16_t SimpleTouchscreen::_get16(uint8_t cmd) {

  uint8_t data1, data2;
  uint16_t datax;

  SPI.transfer(cmd);
  data1 = SPI.transfer(0x00);
  data2 = SPI.transfer(0x00);
  datax = (((data1 << 8) + data2) >> 3) & 0xFFF;

  return datax;
}

uint16_t SimpleTouchscreen::_getX() {
  return (_get16(0x90));
}

uint16_t SimpleTouchscreen::_getY() {
  return (_get16(0xD0));
}

uint16_t SimpleTouchscreen::_getZ() {
  return (4095 - _get16(0xD0));
}

void SimpleTouchscreen::_round(int16_t *val, int16_t max) {
  if (*val < 0) *val = 0;
  if (*val > max) *val = max;
}

void SimpleTouchscreen::read() {

  if (digitalRead(_pinIRQ))
    return;

  int sumx = 0, tmpx = 0;
  int sumy = 0, tmpy = 0;
  int minx = 0x8000, maxx = 0;
  int miny = 0x8000, maxy = 0;
  int samples = 0;

  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(_pinSS, LOW);
  for (int i = 0; i < SAMPLES; i++)
    if (_getZ() > 300) {
      tmpx = _getX();
      tmpy = _getY();

      sumx += tmpx;
      sumy += tmpy;
      if (tmpx < minx)
        minx = tmpx;
      if (tmpx > maxx)
        maxx = tmpx;
      if (tmpy < miny)
        miny = tmpy;
      if (tmpy > maxy)
        maxy = tmpy;
      samples++;
  }
  digitalWrite(_pinSS, HIGH);
  SPI.endTransaction();

  if (samples != SAMPLES)
    return;

  _x = (sumx-minx-maxx) / GOOD_SAMPLES;
  _y = (sumy-miny-maxy) / GOOD_SAMPLES;

  if (_calibrate)
    return;

  _x = (_x - _tsLandscapeLowX) / _ts2lsdRatioX;
  _y = (_y - _tsLandscapeLowY) / _ts2lsdRatioY;

  int temp = _x;
  switch (_rotation) {
  case PORTRATE_0:
    _x = _y;
    _y = _lcdLandscapeX - temp;
    break;
  case LANDSCAPE_90:
    _x = _lcdLandscapeX - _x;
    _y = _lcdLandscapeY - _y;
    break;
  case PORTRATE_180:
    _x = _lcdLandscapeY - _y;
    _y = temp;
    break;
  case LANDSCAPE_270:
    break;
  }

  if (IS_LANDSCAPE(_rotation)) {
    _round(&_x, _lcdLandscapeX);
    _round(&_y, _lcdLandscapeY);
  } else {
    _round(&_x, _lcdLandscapeY);
    _round(&_y, _lcdLandscapeX);
  }
}
