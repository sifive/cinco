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
//
// Methods
// -------
//
// begin(baudRate) - initialization, optionally set baudrate, and then enable RX
// listen() - enables RX interrupts, allowing data to enter the RX buffer
// ignore() - disables RX interrupts
// setBaudRate(baudRate) - selects the baud rate (9600, 19200, 31250, 38400)
//                             - any other value is ignored
// available() - returns the number of characters in the RX buffer
// read() - returns a single character from the buffer
// write(s) - transmits a string
//
// print() is supported
//=============================================================================

#include <SoftwareSerial32.h>

#if F_CPU == 320000000L
  static const uint32_t CYCLES_PER_BIT_9600_320MHZ = 33333;
  static const uint32_t BITS_PER_TICK_9600_Q16_320MHZ = 2;
    // 9600 * CYCLE_RESOLUTION_320MHZ(1/320M) = 9600 * 0.000000003125 * (2^16)
    // = 0.00003 * (2^16) = 1.96608 = ~2

  // These could be omiited and used as a right shift due to them being multiples of 2
  static const uint32_t CYCLES_PER_BIT_19200_320MHZ = 16667;
  static const uint32_t BITS_PER_TICK_19200_Q16_320MHZ = 4; 

  static const uint32_t CYCLES_PER_BIT_31250_320MHZ = 10240;
  static const uint32_t BITS_PER_TICK_31250_Q17_320MHZ = 13;

  static const uint32_t CYCLES_PER_BIT_38400_320MHZ = 8333;
  static const uint32_t BITS_PER_TICK_38400_Q16_320MHZ = 8;

  static const uint32_t CYCLES_PER_BIT_57600_320MHZ = 5556;
  static const uint32_t BITS_PER_TICK_57600_Q15_320MHZ = 6;

  static const uint32_t CYCLES_PER_BIT_115200_320MHZ = 2778;
  static const uint32_t BITS_PER_TICK_115200_Q14_320MHZ = 6;

#elif F_CPU == 256000000L
  // 256 MHZ
  static const uint32_t CYCLES_PER_BIT_9600_256MHZ = 26667;
  static const uint32_t BITS_PER_TICK_9600_Q17_256MHZ = 5; // need to find another multiplier since it's 2.5

  static const uint32_t CYCLES_PER_BIT_19200_256MHZ = 13333;
  static const uint32_t BITS_PER_TICK_19200_Q16_256MHZ = 5;

  static const uint32_t CYCLES_PER_BIT_31250_256MHZ = 8192;
  static const uint32_t BITS_PER_TICK_31250_Q16_256MHZ = 8;

  static const uint32_t CYCLES_PER_BIT_38400_256MHZ = 6667;
  static const uint32_t BITS_PER_TICK_38400_Q16_256MHZ = 10;

  static const uint32_t CYCLES_PER_BIT_57600_256MHZ = 4444;
  static const uint32_t BITS_PER_TICK_57600_Q18_256MHZ = 59;

  static const uint32_t CYCLES_PER_BIT_115200_256MHZ = 2222;
  static const uint32_t BITS_PER_TICK_115200_Q17_256MHZ = 59;

#else
  // 16 MHZ
  static const uint32_t CYCLES_PER_BIT_4800_16MHZ = 3333;
  static const uint32_t BITS_PER_CYCLE_4800_Q16_16MHZ = 20;

  static const uint32_t CYCLES_PER_BIT_9600_16MHZ = 1667;
  static const uint32_t BITS_PER_TICK_9600_Q16_16MHZ = 39;
  
  static const uint32_t CYCLES_PER_BIT_19200_16MHZ = 833;
  static const uint32_t BITS_PER_TICK_19200_Q16_16MHZ = 79;

  static const uint32_t CYCLES_PER_BIT_31250_16MHZ = 512;
  static const uint32_t BITS_PER_TICK_31250_Q16_16MHZ = 128;

  static const uint32_t CYCLES_PER_BIT_38400_16MHZ = 417;
  static const uint32_t BITS_PER_TICK_38400_Q16_16MHZ = 157; 

#endif

uint8_t shiftScaler;
void attachPCINT(uint8_t);
static SoftwareSerial32 *listener = (SoftwareSerial32 *) NULL;
static uint32_t txBitWidth;
static uint32_t rxWindowWidth;
static uint32_t bitsPerTick;
static const uint8_t WAITING_FOR_START_BIT = 0xFF;
static uint8_t rxState;  // 0: got start bit; >0: bits rcvd
static uint64_t prev_t0; // mcycle is a 64-bit register which indicates the time t0 in cycles
static uint8_t rxMask;   // bit mask for building received character
static uint8_t rxValue;  // character being built

static const uint8_t RX_BUFFER_SIZE = 64;  // power of 2 for optimal speed
static uint8_t rxBuffer[RX_BUFFER_SIZE];
static uint8_t rxHead;   // buffer pointer input
static uint8_t rxTail;   // buffer pointer output

static          uint32_t rxBitMask, txBitMask;
static volatile uint32_t *txPort;  // Hifive1 has 32-bit registers

static uint32_t bitTimes( uint32_t dt )
{
  return (uint32_t)((uint64_t)((dt + rxWindowWidth) * bitsPerTick) >> shiftScaler);  
}

void SoftwareSerial32::begin(uint32_t baudRate)
{
  setBaudRate( baudRate );
  listen();
}

//----------------------------------------------------------------------------

void SoftwareSerial32::listen()
{
  if (listener)
    listener->ignore();

  pinMode(rxPin, INPUT);
  rxBitMask = digitalPinToBitMask( rxPin );
  rxPort    = portInputRegister( digitalPinToPort( rxPin ) );

  txBitMask = digitalPinToBitMask( txPin );
  txPort    = portOutputRegister( digitalPinToPort( txPin ) );
  if (txPort)
    *txPort  |= txBitMask;   // high = idle
  pinMode(txPin, OUTPUT);

  clear_csr(mie, MIP_MEIP); // Ext interrupts off
  clear_csr(mie, MIP_MTIP); // Timer interrupts off
  attachPCINT(rxPin);

  rxState  = WAITING_FOR_START_BIT;
  rxHead   = rxTail = 0;    // no characters in buffer
  flush();

  switch (_baudRate)
  {
    case 9600:
      #if F_CPU == 320000000L
        rxWindowWidth = CYCLES_PER_BIT_9600_320MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_9600_320MHZ;
        bitsPerTick = BITS_PER_TICK_9600_Q16_320MHZ;
        shiftScaler = 16;
      #elif F_CPU == 256000000L
        rxWindowWidth = CYCLES_PER_BIT_9600_256MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_9600_256MHZ;
        bitsPerTick = BITS_PER_TICK_9600_Q17_256MHZ;
        shiftScaler = 17;
      #else // F_CPU == 16000000L
        rxWindowWidth = CYCLES_PER_BIT_9600_16MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_9600_16MHZ;
        bitsPerTick = BITS_PER_TICK_9600_Q16_16MHZ;
        shiftScaler = 16;
      #endif
      break;
    case 19200:
      #if F_CPU == 320000000L
        rxWindowWidth = CYCLES_PER_BIT_19200_320MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_19200_320MHZ;
        bitsPerTick = BITS_PER_TICK_19200_Q16_320MHZ;
        shiftScaler = 16;
      #elif F_CPU == 256000000L
        rxWindowWidth = CYCLES_PER_BIT_19200_256MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_19200_256MHZ;
        bitsPerTick = BITS_PER_TICK_19200_Q16_256MHZ;
        shiftScaler = 16;
      #else
        rxWindowWidth = CYCLES_PER_BIT_19200_16MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_19200_16MHZ;
        bitsPerTick = BITS_PER_TICK_19200_Q16_16MHZ;
        shiftScaler = 16;
      #endif
      break;
    case 31250:
      #if F_CPU == 320000000L
        rxWindowWidth = CYCLES_PER_BIT_31250_320MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_31250_320MHZ;
        bitsPerTick = BITS_PER_TICK_31250_Q17_320MHZ;
        shiftScaler = 17;
      #elif F_CPU == 256000000L
        rxWindowWidth = CYCLES_PER_BIT_31250_256MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_31250_256MHZ;
        bitsPerTick = BITS_PER_TICK_31250_Q16_256MHZ;
        shiftScaler = 16;
      #else
        rxWindowWidth = CYCLES_PER_BIT_31250_16MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_31250_16MHZ;
        bitsPerTick = BITS_PER_TICK_31250_Q16_16MHZ;
        shiftScaler = 16;
      #endif
      break;
    case 38400:
      #if F_CPU == 320000000L
        rxWindowWidth = CYCLES_PER_BIT_38400_320MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_38400_320MHZ;
        bitsPerTick = BITS_PER_TICK_38400_Q16_320MHZ;
        shiftScaler = 16;
      #elif F_CPU == 256000000L
        rxWindowWidth = CYCLES_PER_BIT_38400_256MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_38400_256MHZ;
        bitsPerTick = BITS_PER_TICK_38400_Q16_256MHZ; 
        shiftScaler = 16;
      #else
        rxWindowWidth = CYCLES_PER_BIT_38400_16MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_38400_16MHZ;
        bitsPerTick = BITS_PER_TICK_38400_Q16_16MHZ;
          shiftScaler = 16;
      #endif
      break;
    case 57600:
      #if F_CPU == 320000000L
        rxWindowWidth = CYCLES_PER_BIT_57600_320MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_57600_320MHZ;
        bitsPerTick = BITS_PER_TICK_57600_Q15_320MHZ;
        shiftScaler = 15;
      #elif F_CPU == 256000000L
        rxWindowWidth = CYCLES_PER_BIT_57600_256MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_57600_256MHZ;
        bitsPerTick = BITS_PER_TICK_57600_Q18_256MHZ;
        shiftScaler = 18;
      #endif
      break;
    case 115200:
      #if F_CPU == 320000000L
        rxWindowWidth = CYCLES_PER_BIT_115200_320MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_115200_320MHZ;
        bitsPerTick = BITS_PER_TICK_115200_Q14_320MHZ;
        shiftScaler = 14;
      #elif F_CPU == 256000000L
        rxWindowWidth = CYCLES_PER_BIT_115200_256MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_115200_256MHZ;
        bitsPerTick = BITS_PER_TICK_115200_Q17_256MHZ;
        shiftScaler = 17;
      #endif
      break;
    default:
      break;
  }

  listener = this;
  interrupts(); // For some reason this is turned off by default
} // listen

//----------------------------------------------------------------------------

void SoftwareSerial32::ignore()
{
  if (listener) {
    noInterrupts();
    listener = (SoftwareSerial32 *) NULL;
    ::detachInterrupt(digitalPinToInterrupt(rxPin)); // Call the globally defined PLIC handling detachInterrupt
    interrupts();
  }
} // ignore

//----------------------------------------------------------------------------

void SoftwareSerial32::setBaudRate(uint32_t baudRate)
{
  if ((
        ( baudRate ==  9600 ) ||
        ( baudRate == 19200 ) ||
        ( (baudRate == 31250) && (F_CPU >= 16000000L) )   ||
        ( (baudRate == 38400) && (F_CPU >= 16000000L) )   ||
        ( (baudRate == 57600) && (F_CPU >= 256000000L) )  ||
        ( (baudRate == 115200) && (F_CPU >= 256000000L) )
       )
           &&
      (_baudRate != baudRate)) {

    _baudRate = baudRate;

    if (this == listener)
      listen();
  }
} // setBaudRate

//----------------------------------------------------------------------------

int SoftwareSerial32::available()
{
  uint8_t avail = ((rxHead - rxTail + RX_BUFFER_SIZE) % RX_BUFFER_SIZE);

  if (avail == 0) {
    noInterrupts();
    if (checkRxTime()) {
      avail = 1;
    }
    interrupts();
  }

  return avail;

} // available

//----------------------------------------------------------------------------

int SoftwareSerial32::read()
{
  if (rxHead == rxTail) return -1;
  uint8_t c = rxBuffer[rxTail];
  rxTail = (rxTail + 1) % RX_BUFFER_SIZE;

  return c;

} // read

//----------------------------------------------------------------------------

void SoftwareSerial32::attachInterrupt( isr_t fn )
{
  noInterrupts();
  _isr = fn;
  interrupts();
} // attachInterrupt

//----------------------------------------------------------------------------

void SoftwareSerial32::startChar()
{
  rxState = 0;     // got a start bit
  rxMask  = 0x01;  // bit mask, lsb first
  rxValue = 0x00;  // RX character to be, a blank slate
} // startChar

//----------------------------------------------------------------------------

void SoftwareSerial32::rxISR( uint32_t pin )
{
  noInterrupts();
  uint64_t t0;
  rdmcycle(&t0);
  uint32_t rxPort = *portInputRegister(digitalPinToPort(pin));
  uint32_t d = rxPort & rxBitMask;

  if (rxState == WAITING_FOR_START_BIT) {

    // If it looks like a start bit then initialize;
    //   otherwise ignore the rising edge and exit.

    if (d != 0) return;   // it's high so not a start bit, exit
    startChar();

  } else {  // data bit or stop bit (probably) received

    // Determine how many bit periods have elapsed since the last transition.

    uint32_t rxBits          = bitTimes(t0 - prev_t0);
    uint8_t  bitsLeft        = 9 - rxState; // ignores stop bit
    bool     nextCharStarted = (rxBits > bitsLeft);

    uint32_t  bitsThisFrame   =  nextCharStarted ? bitsLeft : rxBits;

    rxState += bitsThisFrame;

    // Set all those bits

    if (d == 0) {
      // back fill previous bits with 1's
      while (bitsThisFrame-- > 0) {
        rxValue |= rxMask;
        rxMask   = rxMask << 1;
      }
      rxMask = rxMask << 1;
    } else { // d==1
      // previous bits were 0's so only this bit is a 1.
      rxMask   = rxMask << (bitsThisFrame-1);
      rxValue |= rxMask;
    }

    // If 8th bit or stop bit then the character is complete.

    if (rxState > 7) {
      rxChar( rxValue );

      if ((d == 1) || !nextCharStarted) {
        rxState = WAITING_FOR_START_BIT;
        // DISABLE STOP BIT TIMER

      } else {
        // The last char ended with 1's, so this 0 is actually
        //   the start bit of the next character.

        startChar();
      }
    }
  }
  prev_t0 = t0;  // remember time stamp
  // The following instructions are preferrable syntactically but these two instructions
  // Take 388 cycles, which breaks the support for high baudrates
  // Instead the pin bitmasks will be calculated manually using the formula (1 << ((pin + 16) % 24))
  // Wich take instead 31 cycles
  // GPIO_REG(GPIO_FALL_IP) = digitalPinToBitMask(pin);
  // GPIO_REG(GPIO_RISE_IP) = digitalPinToBitMask(pin);
  GPIO_REG(GPIO_FALL_IP) = (1 << ((pin + 16) % 24));
  GPIO_REG(GPIO_RISE_IP) = (1 << ((pin + 16) % 24));
  interrupts();
} // rxISR

//----------------------------------------------------------------------------

bool SoftwareSerial32::checkRxTime()
{
  if (rxState != WAITING_FOR_START_BIT) {

    uint32_t d = *rxPort & rxBitMask;

    if (d) {
      // Ended on a 1, see if it has been too long
      uint64_t t0;
      rdmcycle(&t0);
      uint32_t rxBits   = bitTimes( t0-prev_t0 );     
      uint8_t bitsLeft = 9 - rxState; 
      bool     completed = (rxBits > bitsLeft);

      if (completed) {

        while (bitsLeft-- > 0) {
          rxValue |= rxMask;
          rxMask   = rxMask << 1;
        }

        rxState = WAITING_FOR_START_BIT;
        rxChar( rxValue );
        if (!_isr)
          return true;
      }
    }
  }
  return false;

} // checkRxTime

//----------------------------------------------------------------------------

void SoftwareSerial32::rxChar( uint8_t c )
{
  if (listener) {
    if (listener->_isr)
      listener->_isr( c );
    else {
      uint8_t index = (rxHead+1) % RX_BUFFER_SIZE;
      if (index != rxTail) {
        rxBuffer[rxHead] = c;
        rxHead = index;
      }
    }
  }

} // rxChar

//----------------------------------------------------------------------------

#ifdef NEOSWSERIAL_EXTERNAL_PCINT

  // Client code must call SoftwareSerial32::rxISR(PINB) in PCINT handler

#else

  // Must define all of the vectors even though only one is used.

  // This handy PCINT code for different boards is based on PinChangeInterrupt.*
  // from the excellent Cosa project: http://github.com/mikaelpatel/Cosa

  #define PCINT_ISR(pin)                    \
  extern "C" {                              \
  static void PCINT ## pin ## _vect (void)  \
  {                                         \
    SoftwareSerial32::rxISR(pin);               \
  } }

  PCINT_ISR(2);
  PCINT_ISR(3);
  PCINT_ISR(4);
  PCINT_ISR(5);
  PCINT_ISR(6);
  PCINT_ISR(7);
  PCINT_ISR(8);
  PCINT_ISR(9);
  PCINT_ISR(10);
  PCINT_ISR(11);
  PCINT_ISR(12);
  PCINT_ISR(13);
  PCINT_ISR(15);
  PCINT_ISR(16);
  PCINT_ISR(17);
  PCINT_ISR(18);
  PCINT_ISR(19);

  // Initialize all interrupts per pin and put in the appropriate dispatcher
  // Unsupported interrupt pins: 0, 1 and 14

  static void emptyISR() {}

  typedef void (*PCINTisr_ptr_t)(void);
  PCINTisr_ptr_t PCINTdispatcher[20] = {
    emptyISR,
    emptyISR,
    PCINT2_vect,
    PCINT3_vect,
    PCINT4_vect,
    PCINT5_vect,
    PCINT6_vect,
    PCINT7_vect,
    PCINT8_vect,
    PCINT9_vect,
    PCINT10_vect,
    PCINT11_vect,
    PCINT12_vect, 
    PCINT13_vect,
    emptyISR,
    PCINT15_vect,
    PCINT16_vect,
    PCINT17_vect,
    PCINT18_vect,
    PCINT19_vect
  };

  void attachPCINT(uint8_t rxPin)
  {
    ::attachInterrupt(digitalPinToInterrupt(rxPin), PCINTdispatcher[rxPin], CHANGE);
  }

#endif

//-----------------------------------------------------------------------------
// Instead of using a TX buffer and interrupt
// service, the transmit function is a simple timer0 based delay loop.
//
// Interrupts are disabled while the character is being transmitted and
// re-enabled after each character.

size_t SoftwareSerial32::write(uint8_t txChar)
{
  if (!txPort)
    return 0;

  uint32_t width; // Cycles for one bit
  uint32_t txBit  = 0;
  uint32_t b      = 0;
  noInterrupts();
  uint64_t t0;
  rdmcycle(&t0);

  while (txBit++ < 9) {   // repeat for start bit + 8 data bits
    if (b)      // if bit is set
      *txPort |= txBitMask;     //   set TX line high
    else
      *txPort &= ~txBitMask;    //   else set TX line low

    width = txBitWidth;

    // Hold the line for the bit duration
    uint64_t t1;
      rdmcycle(&t1);
      while ((uint64_t)(t1 - t0) < width) {
        // If a HW interrupt is pending while writing use rxISR to handle it 
        // in order to be able to support the asynchronousity of a UART
        if (read_csr(mip) & MIP_MEIP)
        {
          clear_csr(mip, MIP_MEIP);
          GPIO_REG(GPIO_FALL_IP) = (1 << ((rxPin + 16) % 24));
          GPIO_REG(GPIO_RISE_IP) = (1 << ((rxPin + 16) % 24));
          rxISR(*portInputRegister(digitalPinToPort(rxPin)));
        } else {
          checkRxTime();
        }
        rdmcycle(&t1);
      }
    t0    += width;         // advance start time
    b      = txChar & 0x01; // get next bit in the character to send
    txChar = txChar >> 1;   // shift character to expose the following bit
                 // Q: would a signed >> pull in a 1?
  }

  *txPort |= txBitMask;   // stop bit is high
  uint64_t t1;
  rdmcycle(&t1);
  interrupts();
  while ((uint64_t)(t1 - t0) < width) {
    checkRxTime();
    rdmcycle(&t1);
  }

  return 1;               // 1 character sent

} // write
