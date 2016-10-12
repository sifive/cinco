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

#define SPI_BASE_ADDR   SPI1_BASE_ADDR
#define INT_SPI         INT_SPI1_BASE

//#define IOF_MASK        IOF0_SPI1_MASK // AC(0x000007FC,UL)
// we only want to use 3 HW SPI pins (SCK,MOSI,MISO) and bit bang CS
#define IOF_MASK        (0x00000038)
#define OE_MASK         (0x00000028)
#define IE_MASK         (0x00000010)
#define PUE_MASK        (0x00000010)

#define _REG32(p, i)    (*(volatile uint32_t *)((p) + (i)))
#define SPI_REG(i)      _REG32(SPI_BASE_ADDR, (i))
#define GPIO_REG(i)     _REG32(GPIO_BASE_ADDR, (i))
#define PLIC_REG(i)     _REG32(PLIC_BASE_ADDR, (i))

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
//   - beginTransaction(pin, SPISettings settings) (if transactions are available)
#define SPI_HAS_EXTENDED_CS_PIN_HANDLING 1

#define SPI_MODE0 0x02
#define SPI_MODE1 0x00
#define SPI_MODE2 0x03
#define SPI_MODE3 0x01

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
			sckdiv = (F_CPU / (clock + 1)) - 1; // FIXME
		}
                fmt = SPI_FMT_PROTO(SPI_PROTO_S) | SPI_FMT_ENDIAN(bitOrder ? SPI_ENDIAN_LSB : SPI_ENDIAN_MSB) | SPI_FMT_DIR(SPI_DIR_RX) | SPI_FMT_LEN(8);
                sckmode = 0;
                csid = 0;
                csdef = 0xFFFF;
                csmode = SPI_CSMODE_AUTO;
	}
        uint8_t sckmode; // mode bits to set polarity and phase of spi clock
        uint8_t sckdiv; // spi clock frequency = F_CPU/2*(sckdiv-1), maximum is half of F_CPU 
        uint8_t csid;   // csid = index of chip select aka slave select pin, should be set to 0 for now
        uint16_t csdef; // inactive state of chip select pins (high or low)
        uint8_t csmode; // chip select mode (0 =auto = CS toggles with frame)
//         delay0,1 = behavior of cs @ frame start, end, between frames
        uint32_t fmt; // 1/2/4 ch, endianness, dir (whether or not to enq rx data)
         //txdata = when read, bit 31 signals full.  write data to xmit
         //rxdata = bit 31 signals empty + data if there is any
         //others regs :txmark, rxmark, ie, ip, plus non PIO mode sttuff

	friend class SPIClass;
};



class SPIClass {
  public:
	SPIClass(uint32_t _id, void(*_initCb)(void));

	// Transfer functions
	byte transfer(byte _pin, uint8_t _data, SPITransferMode _mode = SPI_LAST);
	uint16_t transfer16(byte _pin, uint16_t _data, SPITransferMode _mode = SPI_LAST);
	void transfer(byte _pin, void *_buf, size_t _count, SPITransferMode _mode = SPI_LAST);
	// Transfer functions on default pin BOARD_SPI_DEFAULT_SS
	byte transfer(uint8_t _data, SPITransferMode _mode = SPI_LAST) { return transfer(BOARD_SPI_DEFAULT_SS, _data, _mode); }
	uint16_t transfer16(uint16_t _data, SPITransferMode _mode = SPI_LAST) { return transfer16(BOARD_SPI_DEFAULT_SS, _data, _mode); }
	void transfer(void *_buf, size_t _count, SPITransferMode _mode = SPI_LAST) { transfer(BOARD_SPI_DEFAULT_SS, _buf, _count, _mode); }

	// Transaction Functions
	void usingInterrupt(uint8_t interruptNumber);
	void beginTransaction(SPISettings settings) { beginTransaction(BOARD_SPI_DEFAULT_SS, settings); }
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

	// These methods sets the same parameters but on default pin BOARD_SPI_DEFAULT_SS
	void setBitOrder(BitOrder _order) { setBitOrder(BOARD_SPI_DEFAULT_SS, _order); };
	void setDataMode(uint8_t _mode) { setDataMode(BOARD_SPI_DEFAULT_SS, _mode); };
	void setClockDivider(uint8_t _div) { setClockDivider(BOARD_SPI_DEFAULT_SS, _div); };

  private:
	void init();

//	Spi *spi;
	uint32_t id;
	BitOrder bitOrder[SPI_CHANNELS_NUM];
	uint32_t divider[SPI_CHANNELS_NUM];
	uint32_t mode[SPI_CHANNELS_NUM];
	void (*initCb)(void);
	bool initialized;
	uint8_t interruptMode;    // 0=none, 1-15=mask, 16=global
	uint8_t interruptSave;    // temp storage, to restore state
	uint32_t interruptMask[4];
};

#if SPI_INTERFACES_COUNT > 0
extern SPIClass SPI;
#endif

// For compatibility with sketches designed for AVR @ 16 MHz
// New programs should use SPI.beginTransaction to set the SPI clock
/*
#define SPI_CLOCK_DIV2	 11
#define SPI_CLOCK_DIV4	 21
#define SPI_CLOCK_DIV8	 42
#define SPI_CLOCK_DIV16	 84
#define SPI_CLOCK_DIV32	 168
#define SPI_CLOCK_DIV64	 255
#define SPI_CLOCK_DIV128 255
*/

#endif
