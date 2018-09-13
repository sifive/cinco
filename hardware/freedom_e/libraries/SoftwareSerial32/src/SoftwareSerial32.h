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


class SoftwareSerial32 : public Stream
{
  SoftwareSerial32( const SoftwareSerial32 & ); // Not allowed
  SoftwareSerial32 & operator =( const SoftwareSerial32 & ); // Not allowed

public:
  SoftwareSerial32(uint8_t receivePin, uint8_t transmitPin)
  {
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

private:
           uint8_t  rxPin, txPin;
  volatile uint32_t *rxPort;

  uint32_t _baudRate;
  isr_t    _isr;

  static void rxChar( uint8_t rx ); // buffer or dispatch one received character

  bool checkRxTime();

  static void startChar();

public:
  // visible only so the ISRs can call it...
  static void rxISR( uint32_t port_input_register );

  //#define NEOSWSERIAL_EXTERNAL_PCINT // uncomment to use your own PCINT ISRs
};
#endif
