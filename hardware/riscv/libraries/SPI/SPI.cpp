/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "SPI.h"
#include "pins_arduino.h"

#include <dev/io.h>
#include <guava/platform/platform.h>
#include <guava/spi.h>
#include "variant.h"



SPIClass::SPIClass(Spi *_spi, uint32_t _id, void(*_initCb)(void)) :
	spi(_spi), id(_id), initCb(_initCb), initialized(false)
{
	// Empty
}

void SPIClass::begin() {
	init();
	// NPCS control is left to the user
	// Default speed set to 4Mhz
}

void SPIClass::begin(uint8_t _pin) {
	init();
}

void SPIClass::init() {
	if (initialized)
		return;
	initialized = true;
}

#ifndef interruptsStatus
#define interruptsStatus() 0
#endif

void SPIClass::usingInterrupt(uint8_t interruptNumber)
{
}

void SPIClass::beginTransaction(uint8_t pin, SPISettings settings)
{
}

void SPIClass::endTransaction(void)
{
}

void SPIClass::end(uint8_t _pin) {
}

void SPIClass::end() {
	initialized = false;
}

void SPIClass::setBitOrder(uint8_t _pin, BitOrder _bitOrder) {
}

void SPIClass::setDataMode(uint8_t _pin, uint8_t _mode) {
}

void SPIClass::setClockDivider(uint8_t _pin, uint8_t _divider) {
}

byte SPIClass::transfer(byte _pin, uint8_t _data, SPITransferMode _mode) {
        int32_t val;
//	int port = IO_SPI_SDCARD; // todo: depending on _pin select port
        // FIXME!
        volatile int32_t *rxf = ((volatile int32_t*)((SPI1_BASE_ADDR) + (SPI_REG_RXFIFO)));
        volatile int32_t *txf = ((volatile int32_t*)((SPI1_BASE_ADDR) + (SPI_REG_TXFIFO)));
        *txf = _data;
        do {
          val = *rxf;
        } while (val < 0);
        return (val & 0xff);
}

void SPIClass::attachInterrupt(void) {
	// Should be enableInterrupt()
}

void SPIClass::detachInterrupt(void) {
	// Should be disableInterrupt()
}

#if SPI_INTERFACES_COUNT > 0
static void SPI_0_Init(void) {
}

SPIClass SPI(SPI_INTERFACE, SPI_INTERFACE_ID, SPI_0_Init);
#endif


#endif
