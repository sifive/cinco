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

#define SPI_REG_TXFIFO_FULL  (*(volatile int32_t *)(SPI_REG(SPI_REG_TXFIFO)) < 0)
#define SPI_REG_RXFIFO_VALUE (*(volatile int32_t *)(SPI_REG(SPI_REG_RXFIFO)))

SPIClass::SPIClass(uint32_t _id, void(*_initCb)(void)) :
	id(_id), initCb(_initCb), initialized(false)
{
	// Empty
}

void SPIClass::begin() {
	init();

        // rimas - NPCS = negative polarity chip select?
	// NPCS control is left to the user

	// Default speed set to 4Mhz
	setClockDivider(BOARD_SPI_DEFAULT_SS, 8);
	setDataMode(BOARD_SPI_DEFAULT_SS, SPI_MODE0);
	setBitOrder(BOARD_SPI_DEFAULT_SS, MSBFIRST);
}

// specifies chip select pin to attach to hardware SPI interface
void SPIClass::begin(uint8_t _pin) {
	init();
	uint32_t spiPin = BOARD_PIN_TO_SPI_PIN(_pin);

        // enable CS pin for selected channel/pin
        uint32_t iof_mask = (1UL << spiPin);
        GPIO_REG(GPIO_iof_sel)  &= ~iof_mask;
        GPIO_REG(GPIO_iof_en)   |=  iof_mask;

	// Default speed set to ~1Mhz
	setClockDivider(_pin, 32);
	setDataMode(_pin, SPI_MODE0);
	setBitOrder(_pin, MSBFIRST);
}

void SPIClass::init() {
	if (initialized)
		return;

        // interrupt state - unused for now
	interruptMode = 0;
	interruptSave = 0;
	interruptMask[0] = 0;
	interruptMask[1] = 0;
	interruptMask[2] = 0;
	interruptMask[3] = 0;
      
	initCb();

        // select SPI function (iof_0) for MOSI, MISO, SCK pins
        // and then enable iof_en for thjose bits
        GPIO_REG(GPIO_iof_sel)  &= ~SPI_IOF_MASK;
        GPIO_REG(GPIO_iof_en)   |=  SPI_IOF_MASK;

//	SPI_Configure(spi, id, SPI_MR_MSTR | SPI_MR_PS | SPI_MR_MODFDIS);
//	SPI_Enable(spi);
	initialized = true;
}
/*
#ifndef interruptsStatus
#define interruptsStatus() __interruptsStatus()
static inline unsigned char __interruptsStatus(void) __attribute__((always_inline, unused));
static inline unsigned char __interruptsStatus(void) {
	unsigned int primask, faultmask;
	asm volatile ("mrs %0, primask" : "=r" (primask));
	if (primask) return 0;
	asm volatile ("mrs %0, faultmask" : "=r" (faultmask));
	if (faultmask) return 0;
	return 1;
}
#endif
*/
void SPIClass::usingInterrupt(uint8_t interruptNumber)
{
/*
	uint8_t irestore;

	irestore = interruptsStatus();
	noInterrupts();
	if (interruptMode < 16) {
		if (interruptNumber > NUM_DIGITAL_PINS) {
			interruptMode = 16;
		} else {
			Pio *pio = g_APinDescription[interruptNumber].pPort;
			uint32_t mask = g_APinDescription[interruptNumber].ulPin;
			if (pio == PIOA) {
				interruptMode |= 1;
				interruptMask[0] |= mask;
			} else if (pio == PIOB) {
				interruptMode |= 2;
				interruptMask[1] |= mask;
			} else if (pio == PIOC) {
				interruptMode |= 4;
				interruptMask[2] |= mask;
			} else if (pio == PIOD) {
				interruptMode |= 8;
				interruptMask[3] |= mask;
			} else {
				interruptMode = 16;
			}
		}
	}
	if (irestore) interrupts();
*/
}


// start an SPI transaction using specified CS pin and SPIsettings
void SPIClass::beginTransaction(uint8_t pin, SPISettings settings)
{
  /*
	uint8_t mode = interruptMode;
	if (mode > 0) {
		if (mode < 16) {
			if (mode & 1) PIOA->PIO_IDR = interruptMask[0];
			if (mode & 2) PIOB->PIO_IDR = interruptMask[1];
			if (mode & 4) PIOC->PIO_IDR = interruptMask[2];
			if (mode & 8) PIOD->PIO_IDR = interruptMask[3];
		} else {
			interruptSave = interruptsStatus();
			noInterrupts();
		}
	}
        */


  // before starting a transaction, set SPI peripheral to desired mode
  // next few lines possibly superflous
        uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(pin);
	bitOrder[ch] = settings.border;
        divider[ch] = settings.sckdiv;
        mode[ch] = settings.sckmode;

        SPI_REG(SPI_REG_FMT) = SPI_FMT_PROTO(SPI_PROTO_S) | \ // single channel SPI
                               SPI_FMT_ENDIAN(settings.border ? SPI_ENDIAN_MSB : SPI_ENDIAN_LSB) | \
                               SPI_FMT_DIR(SPI_DIR_RX) | \ // don't ignore incoming data
                               SPI_FMT_LEN(8); // 8 bit transactions 

        SPI_REG(SPI_REG_SCKDIV)  = settings.sckdiv; 
        SPI_REG(SPI_REG_SCKMODE) = settings.sckmode; 
// FIXME: deal with these properly
        SPI_REG(SPI_REG_CSID)    = settings.csid; 
        SPI_REG(SPI_REG_CSDEF)   = settings.csdef; 
        SPI_REG(SPI_REG_CSMODE)  = settings.csmode; 
}

void SPIClass::endTransaction(void)
{
/*
	uint8_t mode = interruptMode;
	if (mode > 0) {
		if (mode < 16) {
			if (mode & 1) PIOA->PIO_IER = interruptMask[0];
			if (mode & 2) PIOB->PIO_IER = interruptMask[1];
			if (mode & 4) PIOC->PIO_IER = interruptMask[2];
			if (mode & 8) PIOD->PIO_IER = interruptMask[3];
		} else {
			if (interruptSave) interrupts();
		}
	}
*/
}

void SPIClass::end(uint8_t _pin) {
	uint32_t spiPin = BOARD_PIN_TO_SPI_PIN(_pin);
        GPIO_REG(GPIO_iof_en)  &= ~(1UL << spiPin);
//        GPIO_REG(GPIO_iof_en)  &= ~SPI_IOF_MASK;

	// Setting the pin as INPUT will disconnect it from SPI peripheral
//	pinMode(_pin, INPUT);
}

void SPIClass::end() {
//	SPI_Disable(spi);
        GPIO_REG(GPIO_iof_en)  &= ~SPI_IOF_MASK;
	initialized = false;
}

void SPIClass::setBitOrder(uint8_t _pin, BitOrder _bitOrder) {
	uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(_pin);
	bitOrder[ch] = _bitOrder;
}

void SPIClass::setDataMode(uint8_t _pin, uint8_t _mode) {
	uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(_pin);
	mode[ch] = _mode;
}

void SPIClass::setClockDivider(uint8_t _pin, uint8_t _divider) {
	uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(_pin);
	divider[ch] = _divider;
	// SPI_CSR_DLYBCT(1) keeps CS enabled for 32 MCLK after a completed
	// transfer. Some device needs that for working properly.
//	SPI_ConfigureNPCS(spi, ch, mode[ch] | SPI_CSR_SCBR(divider[ch]) | SPI_CSR_DLYBCT(1));
}

byte SPIClass::transfer(byte _pin, uint8_t _data, SPITransferMode _mode) {
	uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(_pin);

//	if (_mode == SPI_LAST)
//		d |= SPI_TDR_LASTXFER;

	// SPI_Write(spi, _channel, _data);
	while (SPI_REG(SPI_REG_TXFIFO) < 0) ;
        SPI_REG(SPI_REG_TXFIFO) = d;

	// return SPI_Read(spi);
        volatile int32_t x;
	while ((x = SPI_REG(SPI_REG_RXFIFO)) < 0);
	return x & 0xFF;
}

uint16_t SPIClass::transfer16(byte _pin, uint16_t _data, SPITransferMode _mode) {
	union { uint16_t val; struct { uint8_t lsb; uint8_t msb; }; } t;
	uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(_pin);

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

//	uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(_pin);
//	bool reverse = (bitOrder[ch] == LSBFIRST);

	// Send the first byte
//	while (((int32_t)SPI_REG(SPI_REG_TXFIFO)) < 0)
//		;
        while (SPI_REG_TXFIFO_FULL) ;
        SPI_REG(SPI_REG_TXFIFO) = *buffer;

//        volatile int32_t x;
        uint8_t r;
	while (_count > 1) {
		// Prepare next byte
		d = *(buffer+1);
/*		if (reverse)
			d = __REV(__RBIT(d)); */
/*		if (_count == 2 && _mode == SPI_LAST)
			d |= SPI_TDR_LASTXFER; */

		// Read transferred byte and send next one straight away
//	        while ((x = SPI_REG(SPI_REG_RXFIFO)) < 0)
//	        	;
                while ((x = SPI_REG(SPI_REG_RXFIFO)) < 0) ;
		r = x & 0xFF;

                SPI_REG(SPI_REG_TXFIFO) = d;

		// Save read byte
//		if (reverse)
//			r = __REV(__RBIT(r));
		*buffer = r;
		buffer++;
		_count--;
	}

	// Receive the last transferred byte
	while ((x = SPI_REG(SPI_REG_RXFIFO)) < 0)
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
static void SPI_0_Init(void) {
        // select SPI function (iof_0) for MOSI, MISO, SCK pins
        GPIO_REG(GPIO_iof_sel)  &= ~SPI_IOF_MASK;;
        GPIO_REG(GPIO_iof_en)   |=  SPI_IOF_MASK;
}

SPIClass SPI(SPI_INTERFACE_ID, SPI_0_Init);
#endif

