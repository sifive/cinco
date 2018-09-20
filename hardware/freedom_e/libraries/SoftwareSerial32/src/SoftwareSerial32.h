#ifndef SoftwareSerial32_h
#define SoftwareSerial32_h

#include "Arduino.h"


//---------------------------------------------------------------------------
//
// SoftwareSerial32, based on NeoSWSerial
// Copyright (C) 2015-2016, SlashDevin
// Copyright (c) 2018 Western Digital Corporation or its affiliates.
//
// SoftwareSerial32 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SoftwareSerial32 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details:
//
//     <http://www.gnu.org/licenses/>.
//
// Modified by Shadi Kamal <shadi.kamal@wdc.com>, 2018
//---------------------------------------------------------------------------

static const uint8_t RX_BUFFER_SIZE = 64;  // power of 2 for optimal speeds
static const uint8_t MAX_NUM_MUART = 20;

class SoftwareSerial32 : public Stream
{
  SoftwareSerial32( const SoftwareSerial32 & ); // Not allowed
  SoftwareSerial32 & operator =( const SoftwareSerial32 & ); // Not allowed

public:
  SoftwareSerial32(uint8_t receivePin, uint8_t transmitPin)
  {
    // TODO: Block generation of more than MAX_NUM_MUART UART instances
    rxPin = receivePin;
    txPin = transmitPin;
    _isr  = (isr_t) NULL;
  }

          void   begin(uint32_t baudRate=9600);   // initialize, set baudrate, listen
          void   listen();                        // enable RX interrupts
          void   ignore();                        // disable RX interrupts
          void   setBaudRate(uint32_t baudRate);  // 9600 [default], 19200, 38400

  virtual int    available();
  virtual int    read();
  virtual size_t write(uint8_t txChar);
  using Stream::write; // make the base class overloads visible
  virtual int    peek() { return 0; };
  virtual void   flush() {};
          void   end() { ignore(); }

  typedef void (* isr_t)( uint8_t );
  void attachInterrupt( isr_t fn );
  void detachInterrupt() { attachInterrupt( (isr_t) NULL ); };

          uint32_t bitTimes( uint32_t dt );

private:
           uint8_t  rxPin, txPin;
  volatile uint32_t *rxPort;

  uint32_t _baudRate;
  isr_t    _isr;

  void rxChar( uint8_t rx ); // buffer or dispatch one received character

  bool checkRxTime();

  void startChar();

  // Member variables
  SoftwareSerial32 *listener = (SoftwareSerial32 *) NULL;
  uint32_t txBitWidth;
  uint32_t rxWindowWidth;
  uint32_t bitsPerTick;
  uint8_t shiftScaler;
  uint8_t rxState;  // 0: got start bit; >0: bits rcvd
  uint64_t prev_t0; // mcycle is a 64-bit register which indicates the time t0 in cycles
  uint8_t rxMask;   // bit mask for building received character
  uint8_t rxValue;  // character being built
  uint8_t rxBuffer[RX_BUFFER_SIZE];
  uint8_t rxHead;   // buffer pointer input
  uint8_t rxTail;   // buffer pointer output
  uint32_t rxBitMask, txBitMask;
  volatile uint32_t *txPort;  // Hifive1 has 32-bit registers

public:
  // visible only so the ISRs can call it...
  static void rxISR( uint32_t pin_num );

  //#define NEOSWSERIAL_EXTERNAL_PCINT // uncomment to use your own PCINT ISRs
};
#endif
