/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@arduino.cc>
 * Copyright (c) 2014 by Paul Stoffregen <paul@pjrc.com> (Transaction API)
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "SPI.h"

SPIClass::SPIClass(uint32_t _id) :
  id(_id)
{
	// Empty
}

void SPIClass::begin() {
  
  GPIO_REG(GPIO_IOF_SEL) &= ~SPI_IOF_MASK;
  GPIO_REG(GPIO_IOF_EN)  |= SPI_IOF_MASK;

  //setClockDivider(F_CPU/1000000);
  setDataMode(SPI_MODE0);
  setBitOrder(MSBFIRST);
  
}

// specifies chip select pin to attach to hardware SPI interface
void SPIClass::begin(uint8_t _pin) {
  	
        // enable CS pin for selected channel/pin
        uint32_t iof_mask = digitalPinToBitMask(_pin);
        GPIO_REG(GPIO_IOF_SEL)  &= ~iof_mask;
        GPIO_REG(GPIO_IOF_EN)   |=  iof_mask;

	// Default speed set to ~1Mhz
	//setClockDivider(_pin, F_CPU/1000000);
	setDataMode(_pin, SPI_MODE0);
	setBitOrder(_pin, MSBFIRST);

	this->begin();

}

void SPIClass::usingInterrupt(uint8_t interruptNumber)
{
}

// start an SPI transaction using specified SPIsettings
void SPIClass::beginTransaction(SPISettings settings)
{
  // before starting a transaction, set SPI peripheral to desired mode

  SPI_REG(SPI_REG_FMT) = SPI_FMT_PROTO(SPI_PROTO_S) |
    SPI_FMT_ENDIAN((settings.border == LSBFIRST) ? SPI_ENDIAN_LSB : SPI_ENDIAN_MSB) |
    SPI_FMT_DIR(SPI_DIR_RX) |
    SPI_FMT_LEN(8);
  
  SPI_REG(SPI_REG_SCKDIV)  = settings.sckdiv;

  SPI_REG(SPI_REG_SCKMODE) = settings.sckmode;

  // We Don't control CS, so this setting doesn't matter.
  //SPI_REG(SPI_REG_CSDEF)   = 0xFFFF;

}


// start an SPI transaction using specified CS pin and SPIsettings
void SPIClass::beginTransaction(uint8_t pin, SPISettings settings)
{
  
  // before starting a transaction, set SPI peripheral to desired mode
  SPI_REG(SPI_REG_CSID)   = SS_PIN_TO_CS_ID(pin); 
  SPI_REG(SPI_REG_CSMODE) = SPI_CSMODE_HOLD;

  // There is no way here to change the CS polarity.
  SPI_REG(SPI_REG_CSDEF)   = 0xFFFF;

  this->beginTransaction(settings);

}

void SPIClass::endTransaction(void) {
  SPI_REG(SPI_REG_CSMODE) = SPI_CSMODE_AUTO;
}

void SPIClass::end(uint8_t _pin) {
  GPIO_REG(GPIO_IOF_EN)  &= ~digitalPinToBitMask(_pin);    
}

void SPIClass::end() {
  GPIO_REG(GPIO_IOF_EN)  &= ~SPI_IOF_MASK;
}

void SPIClass::setBitOrder(BitOrder _bitOrder) {
  SPI_REG(SPI_REG_FMT) = SPI_FMT_PROTO(SPI_PROTO_S) |
    SPI_FMT_ENDIAN((_bitOrder == LSBFIRST) ? SPI_ENDIAN_LSB : SPI_ENDIAN_MSB) |
    SPI_FMT_DIR(SPI_DIR_RX) |
    SPI_FMT_LEN(8);
}

void SPIClass::setBitOrder(uint8_t _pin, BitOrder _bitOrder) {
	uint32_t ch = SS_PIN_TO_CS_ID(_pin);
	bitOrder[ch] = _bitOrder;
	// This gets used later?
}

void SPIClass::setDataMode(uint8_t _mode) {
  SPI_REG(SPI_REG_SCKMODE) = _mode;
}

void SPIClass::setDataMode(uint8_t _pin, uint8_t _mode) {
	uint32_t ch = SS_PIN_TO_CS_ID(_pin);
	mode[ch] = _mode;
	// This gets used later?
}

void SPIClass::setClockDivider(uint8_t _divider) {
  SPI_REG(SPI_REG_SCKDIV) = _divider;
}

void SPIClass::setClockDivider(uint8_t _pin, uint8_t _divider) {
  uint32_t ch = SS_PIN_TO_CS_ID(_pin);
  divider[ch] = _divider;
}


byte SPIClass::transfer(uint8_t _data, SPITransferMode _mode) {

  // SPI_Write(spi, _channel, _data);
  while (SPI_REG(SPI_REG_TXFIFO) & SPI_TXFIFO_FULL) ;
  SPI_REG(SPI_REG_TXFIFO) = _data;
  
  // return SPI_Read(spi);
  volatile int32_t x;
  while ((x = SPI_REG(SPI_REG_RXFIFO)) & SPI_RXFIFO_EMPTY);
  return x & 0xFF;
  
  if (_mode == SPI_LAST) {
    SPI_REG(SPI_REG_CSMODE) = SPI_CSMODE_AUTO;
  }
  
}

byte SPIClass::transfer(byte _pin, uint8_t _data, SPITransferMode _mode) {

  // No need to do anything with the pin, because that was already
  // set up earlier.
  return this->transfer(_data, _mode);
 
}

uint16_t SPIClass::transfer16(byte _pin, uint16_t _data, SPITransferMode _mode) {
	union { uint16_t val; struct { uint8_t lsb; uint8_t msb; }; } t;
	uint32_t ch = SS_PIN_TO_CS_ID(_pin);

	t.val = _data;

	if (bitOrder[ch] == LSBFIRST) {
		t.lsb = transfer(_pin, t.lsb, SPI_CONTINUE);
		t.msb = transfer(_pin, t.msb, _mode);
	} else {
		t.msb = transfer(_pin, t.msb, SPI_CONTINUE);
		t.lsb = transfer(_pin, t.lsb, _mode);
	}

	return t.val;
}

void SPIClass::transfer(byte _pin, void *_buf, size_t _count, SPITransferMode _mode) {
  
  if (_count == 0)
    return;
  
  uint8_t *buffer = (uint8_t *)_buf;
  if (_count == 1) {
    *buffer = transfer(_pin, *buffer, _mode);
    return;
  }

  // Send the first byte
  while (SPI_REG(SPI_REG_TXFIFO) < 0) ;
  SPI_REG(SPI_REG_TXFIFO) = *buffer;

  volatile int32_t x;
  uint8_t r,d;
  while (_count > 1) {
    // Prepare next byte
    d = *(buffer+1);
    // Read transferred byte and send next one straight away
    while ((x = (SPI_REG(SPI_REG_RXFIFO)) & SPI_RXFIFO_EMPTY))
      ;
    r = x & 0xFF;
    while (SPI_REG(SPI_REG_TXFIFO) & SPI_TXFIFO_FULL);
    SPI_REG(SPI_REG_TXFIFO) = d;

		// Save read byte
		*buffer = r;
		buffer++;
		_count--;
	}

	// Receive the last transferred byte
  while ((x = (SPI_REG(SPI_REG_RXFIFO)) & SPI_RXFIFO_EMPTY))
    ;
  r = x & 0xFF;
  *buffer = r;
}

void SPIClass::attachInterrupt(void) {
	// Should be enableInterrupt()
}

void SPIClass::detachInterrupt(void) {
	// Should be disableInterrupt()
}

#if SPI_INTERFACES_COUNT > 0
SPIClass SPI(1);
#endif

