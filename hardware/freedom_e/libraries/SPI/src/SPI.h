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

#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include "variant.h"
#include <stdio.h>

// SPI_HAS_TRANSACTION means SPI has
//   - beginTransaction()
//   - endTransaction()
//   - usingInterrupt()
//   - SPISetting(clock, bitOrder, dataMode)
#define SPI_HAS_TRANSACTION 1

// SPI_HAS_EXTENDED_CS_PIN_HANDLING means SPI has automatic 
// CS pin handling and provides the following methods:
//   - begin(pin)
//   - end(pin)
//   - setBitOrder(pin, bitorder)
//   - setDataMode(pin, datamode)
//   - setClockDivider(pin, clockdiv)
//   - transfer(pin, data, SPI_LAST/SPI_CONTINUE)
//   - beginTransaction(pin, SPISettings settings)
//     (if transactions are available)
#define SPI_HAS_EXTENDED_CS_PIN_HANDLING 1

#define SPI_MODE0 0x00
#define SPI_MODE1 0x01
#define SPI_MODE2 0x02
#define SPI_MODE3 0x03

enum SPITransferMode {
	SPI_CONTINUE,
	SPI_LAST
};

class SPISettings {
public:
	SPISettings(uint32_t clock, BitOrder bitOrder, uint8_t dataMode) {
		if (__builtin_constant_p(clock)) {
			init_AlwaysInline(clock, bitOrder, dataMode);
		} else {
			init_MightInline(clock, bitOrder, dataMode);
		}
	}
	SPISettings() { init_AlwaysInline(4000000, MSBFIRST, SPI_MODE0); }
private:
	void init_MightInline(uint32_t clock, BitOrder bitOrder, uint8_t dataMode) {
		init_AlwaysInline(clock, bitOrder, dataMode);
	}
	void init_AlwaysInline(uint32_t clock, BitOrder bitOrder, uint8_t dataMode) __attribute__((__always_inline__)) {
		if (clock < (F_CPU/512)) {
			sckdiv = 255;
		} else if (clock >= (F_CPU / 2)) {
			sckdiv = 0;
		} else {
			sckdiv = (F_CPU / (2*clock)) - 1;
		}
                sckmode = dataMode;
                csid = 0;
                csdef = 0xFFFF;
                csmode = SPI_CSMODE_AUTO;
                border = bitOrder;
	}
        uint8_t   sckmode; // mode bits to set polarity and phase of spi clock
        uint8_t   sckdiv;  // spi clock frequency = F_CPU/2*(sckdiv-1), maximum is half of F_CPU 
        uint8_t   csid;    // csid = index of chip select aka slave select pin, valid values are 0,1,2,3
        uint16_t  csdef;   // inactive state of chip select pins (high or low)
        uint8_t   csmode;  // chip select mode (0 = auto, 1 = CS toggles with frame)
        BitOrder  border;  // bit ordering : 0 = LSB first, 1 = MSB first (common case)

        // to read/write data over SPI interface, use next two FIFO ports 
        //   txdata = when read, bit 31 signals full.  write data to xmit
        //   rxdata = bit 31 signals empty, otherwise data in bits [7:0] is valid

        // currently unused SPI control registers:
        //   interrupt related : txmark, rxmark, ie, ip, plus non PIO mode stuff
        //   delay0,1 : behavior of cs @ frame start, end, between frames

	friend class SPIClass;
};



class SPIClass {
  public:
	SPIClass(uint32_t _id);

	// Transfer functions where the hardware controls the SS line
	byte transfer(byte _pin, uint8_t _data, SPITransferMode _mode = SPI_LAST);
	uint16_t transfer16(byte _pin, uint16_t _data, SPITransferMode _mode = SPI_LAST);
	void transfer(byte _pin, void *_buf, size_t _count, SPITransferMode _mode = SPI_LAST);

	// Transfer functions where the user controls the SS line
	byte transfer(uint8_t _data, SPITransferMode _mode = SPI_LAST);
	void transfer(void *_buf, size_t _count, SPITransferMode _mode = SPI_LAST);

	// Transaction Functions
	void usingInterrupt(uint8_t interruptNumber);
	void beginTransaction(SPISettings settings);
	void beginTransaction(uint8_t pin, SPISettings settings);
	void endTransaction(void);

	// SPI Configuration methods
	void attachInterrupt(void);
	void detachInterrupt(void);

	void begin(void);
	void end(void);

	// Attach/Detach pin to/from SPI controller
	void begin(uint8_t _pin);
	void end(uint8_t _pin);

	// These methods sets a parameter on a single pin
	void setBitOrder(uint8_t _pin, BitOrder);
	void setDataMode(uint8_t _pin, uint8_t);
	void setClockDivider(uint8_t _pin, uint8_t);

	// These methods sets the same parameters, but globally.
	void setBitOrder(BitOrder _order);
	void setDataMode(uint8_t _mode);
	void setClockDivider(uint8_t _div);

  private:
	uint32_t id;
	// These are for specific pins.
	BitOrder bitOrder[4+1];
	uint32_t divider[4+1];
	uint32_t mode[4+1];
	
	uint8_t interruptMode;    // 0=none, 1-15=mask, 16=global
	uint8_t interruptSave;    // temp storage, to restore state
	uint32_t interruptMask[4];
};

#if SPI_INTERFACES_COUNT > 0
extern SPIClass SPI;
#endif

#endif // _SPI_H_INCLUDED

