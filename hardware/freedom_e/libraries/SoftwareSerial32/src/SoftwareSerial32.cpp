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
//#define _SWSERIAL32_DBG

#if F_CPU == 320000000L
  // 320 MHZ
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
  static const uint32_t BITS_PER_TICK_115200_Q13_320MHZ = 3;

  static const uint32_t CYCLES_PER_BIT_230400_320MHZ = 1389;
  static const uint32_t BITS_PER_TICK_230400_Q16_320MHZ = 47;

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

  static const uint32_t CYCLES_PER_BIT_230400_256MHZ    = 1111;
  static const uint32_t BITS_PER_TICK_230400_Q10_256MHZ = 1;

  static const uint32_t CYCLES_PER_BIT_250000_256MHZ = 1024;
  static const uint32_t BITS_PER_TICK_250000_Q10_256MHZ = 1;

  static const uint32_t CYCLES_PER_BIT_500000_256MHZ = 512;
  static const uint32_t BITS_PER_TICK_500000_Q9_256MHZ = 1;

  static const uint32_t CYCLES_PER_BIT_1000000_256MHZ = 256;
  static const uint32_t BITS_PER_TICK_1000000_Q8_256MHZ = 1;

#else
  // 16 MHZ
  static const uint32_t CYCLES_PER_BIT_4800_16MHZ = 3333;
  static const uint32_t BITS_PER_CYCLE_4800_Q16_16MHZ = 20;

  static const uint32_t CYCLES_PER_BIT_9600_16MHZ = 1667;
  static const uint32_t BITS_PER_TICK_9600_Q13_16MHZ = 5;
  
  static const uint32_t CYCLES_PER_BIT_19200_16MHZ = 833;
  static const uint32_t BITS_PER_TICK_19200_Q12_16MHZ = 5;

  static const uint32_t CYCLES_PER_BIT_31250_16MHZ = 512;
  static const uint32_t BITS_PER_TICK_31250_Q16_16MHZ = 128;

  static const uint32_t CYCLES_PER_BIT_38400_16MHZ = 417;
  static const uint32_t BITS_PER_TICK_38400_Q11_16MHZ = 5;

  static const uint32_t CYCLES_PER_BIT_57600_16MHZ = 278;
  static const uint32_t BITS_PER_TICK_57600_Q16_16MHZ = 236;

  static const uint32_t CYCLES_PER_BIT_115200_16MHZ = 139;
  static const uint32_t BITS_PER_TICK_115200_Q13_16MHZ = 59;

#endif

static const uint8_t WAITING_FOR_START_BIT = 0xFF;
void attachPCINT( uint8_t );

// An array to hold and manage active UART instances
static SoftwareSerial32* SWUARTInstanceMap[MAX_NUM_MUART];

uint32_t SoftwareSerial32::bitTimes( uint32_t dt)
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

  noInterrupts();
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
        rxWindowWidth = CYCLES_PER_BIT_9600_256MHZ / 3;
        txBitWidth = CYCLES_PER_BIT_9600_256MHZ;
        bitsPerTick = BITS_PER_TICK_9600_Q17_256MHZ;
        shiftScaler = 17;
      #else
        rxWindowWidth = CYCLES_PER_BIT_9600_16MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_9600_16MHZ;
        bitsPerTick = BITS_PER_TICK_9600_Q13_16MHZ;
        shiftScaler = 13;
      #endif
      break;
    case 19200:
      #if F_CPU == 320000000L
        rxWindowWidth = CYCLES_PER_BIT_19200_320MHZ / 3;
        txBitWidth = CYCLES_PER_BIT_19200_320MHZ;
        bitsPerTick = BITS_PER_TICK_19200_Q16_320MHZ;
        shiftScaler = 16;
      #elif F_CPU == 256000000L
        rxWindowWidth = CYCLES_PER_BIT_19200_256MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_19200_256MHZ;
        bitsPerTick = BITS_PER_TICK_19200_Q16_256MHZ;
        shiftScaler = 16;
      #else
        rxWindowWidth = CYCLES_PER_BIT_19200_16MHZ / 3;
        txBitWidth = CYCLES_PER_BIT_19200_16MHZ;
        bitsPerTick = BITS_PER_TICK_19200_Q12_16MHZ;
        shiftScaler = 12;
      #endif
      break;
    case 31250:
      #if F_CPU == 320000000L
        rxWindowWidth = CYCLES_PER_BIT_31250_320MHZ / 3;
        txBitWidth = CYCLES_PER_BIT_31250_320MHZ;
        bitsPerTick = BITS_PER_TICK_31250_Q17_320MHZ;
        shiftScaler = 17;
      #elif F_CPU == 256000000L
        rxWindowWidth = CYCLES_PER_BIT_31250_256MHZ / 3;
        txBitWidth = CYCLES_PER_BIT_31250_256MHZ;
        bitsPerTick = BITS_PER_TICK_31250_Q16_256MHZ;
        shiftScaler = 16;
      #else
        rxWindowWidth = CYCLES_PER_BIT_31250_16MHZ / 3;
        txBitWidth = CYCLES_PER_BIT_31250_16MHZ;
        bitsPerTick = BITS_PER_TICK_31250_Q16_16MHZ;
        shiftScaler = 16;
      #endif
      break;
    case 38400:
      #if F_CPU == 320000000L
        rxWindowWidth = CYCLES_PER_BIT_38400_320MHZ / 3;
        txBitWidth = CYCLES_PER_BIT_38400_320MHZ;
        bitsPerTick = BITS_PER_TICK_38400_Q16_320MHZ;
        shiftScaler = 16;
      #elif F_CPU == 256000000L
        rxWindowWidth = CYCLES_PER_BIT_38400_256MHZ / 3;
        txBitWidth = CYCLES_PER_BIT_38400_256MHZ;
        bitsPerTick = BITS_PER_TICK_38400_Q16_256MHZ; 
        shiftScaler = 16;
      #else
        rxWindowWidth = CYCLES_PER_BIT_38400_16MHZ / 3;
        txBitWidth = CYCLES_PER_BIT_38400_16MHZ;
        bitsPerTick = BITS_PER_TICK_38400_Q11_16MHZ;
        shiftScaler = 11;
      #endif
      break;
    case 57600:
      #if F_CPU == 320000000L
        rxWindowWidth = CYCLES_PER_BIT_57600_320MHZ / 3;
        txBitWidth = CYCLES_PER_BIT_57600_320MHZ;
        bitsPerTick = BITS_PER_TICK_57600_Q15_320MHZ;
        shiftScaler = 15;
      #elif F_CPU == 256000000L
        rxWindowWidth = CYCLES_PER_BIT_57600_256MHZ / 3;
        txBitWidth = CYCLES_PER_BIT_57600_256MHZ;
        bitsPerTick = BITS_PER_TICK_57600_Q18_256MHZ;
        shiftScaler = 18;
      #else
        rxWindowWidth = CYCLES_PER_BIT_57600_16MHZ / 3;
        txBitWidth = CYCLES_PER_BIT_57600_16MHZ;
        bitsPerTick = BITS_PER_TICK_57600_Q16_16MHZ;
        shiftScaler = 16;
      #endif
      break;
    case 115200:
      #if F_CPU == 320000000L
        rxWindowWidth = CYCLES_PER_BIT_115200_320MHZ / 3;
        txBitWidth = CYCLES_PER_BIT_115200_320MHZ;
        bitsPerTick = BITS_PER_TICK_115200_Q13_320MHZ;
        shiftScaler = 13;
      #elif F_CPU == 256000000L
        rxWindowWidth = CYCLES_PER_BIT_115200_256MHZ / 3;
        txBitWidth = CYCLES_PER_BIT_115200_256MHZ;
        bitsPerTick = BITS_PER_TICK_115200_Q17_256MHZ;
        shiftScaler = 17;
      #else
        rxWindowWidth = CYCLES_PER_BIT_115200_16MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_115200_16MHZ;
        bitsPerTick = BITS_PER_TICK_115200_Q13_16MHZ;
        shiftScaler = 13;
      #endif
      break;
    case 230400:
      #if F_CPU == 320000000L
        rxWindowWidth = CYCLES_PER_BIT_230400_320MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_230400_320MHZ;
        bitsPerTick = BITS_PER_TICK_230400_Q16_320MHZ;
        shiftScaler = 16;
      #elif F_CPU == 256000000L
        rxWindowWidth = CYCLES_PER_BIT_230400_256MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_230400_256MHZ;
        bitsPerTick = BITS_PER_TICK_230400_Q10_256MHZ;
        shiftScaler = 10;
      #endif
      break;
    case 250000:
      #if F_CPU == 256000000L
        rxWindowWidth = CYCLES_PER_BIT_250000_256MHZ >> 1;
        txBitWidth = CYCLES_PER_BIT_250000_256MHZ;
        bitsPerTick = BITS_PER_TICK_250000_Q10_256MHZ;
        shiftScaler = 10;
      #endif
      break;
    case 500000:
      #if F_CPU == 256000000L
        rxWindowWidth = CYCLES_PER_BIT_500000_256MHZ >> 1;
        txBitWidth    = CYCLES_PER_BIT_500000_256MHZ;
        bitsPerTick   = BITS_PER_TICK_500000_Q9_256MHZ;
        shiftScaler   = 9;
      #endif
      break;
    case 1000000:
      #if F_CPU == 256000000L
        rxWindowWidth = CYCLES_PER_BIT_1000000_256MHZ >> 1;
        txBitWidth    = CYCLES_PER_BIT_1000000_256MHZ;
        bitsPerTick   = BITS_PER_TICK_1000000_Q8_256MHZ;
        shiftScaler   = 8;
      #endif
      break;
    default:
      break;
  }

  listener = this;
  SWUARTInstanceMap[rxPin] = this;

#if defined(_SWSERIAL32_DBG)
  GPIO_REG(GPIO_OUTPUT_EN)  |= (1 << RED_LED_OFFSET);
  GPIO_REG(GPIO_INPUT_EN)   &= ~(1 << RED_LED_OFFSET);
  GPIO_REG(GPIO_OUTPUT_VAL) &= ~(1 << RED_LED_OFFSET);
  GPIO_REG(GPIO_OUTPUT_EN)  |= (1 << GREEN_LED_OFFSET);
  GPIO_REG(GPIO_INPUT_EN)   &= ~(1 << GREEN_LED_OFFSET);
  GPIO_REG(GPIO_OUTPUT_VAL) &= ~(1 << GREEN_LED_OFFSET);
  GPIO_REG(GPIO_OUTPUT_EN)  |= (1 << BLUE_LED_OFFSET);
  GPIO_REG(GPIO_INPUT_EN)   &= ~(1 << BLUE_LED_OFFSET);
  GPIO_REG(GPIO_OUTPUT_VAL) &= ~(1 << BLUE_LED_OFFSET);

  Serial.println("SWUART Instance information:");
  Serial.print("\tlistener:\t");
  Serial.println((uint32_t)(listener), HEX);
  Serial.print("\ttxBitWidth:\t");
  Serial.println(txBitWidth);
  Serial.print("\trxWindowWidth:\t");
  Serial.println(rxWindowWidth);
  Serial.print("\tbitsPerTick:\t");
  Serial.println(bitsPerTick);
  Serial.print("\tshiftScaler:\t");
  Serial.println(shiftScaler);
  Serial.print("\trxState:\t");
  Serial.println(rxState);
  Serial.print("\tprev_t0:\t");
  Serial.println((uint32_t)(prev_t0));
  Serial.print("\trxMask:\t\t");
  Serial.println(rxMask);
  Serial.print("\trxValue:\t");
  Serial.println(rxValue);

  Serial.print("\trxBuffer:\t");
  Serial.print("[");
  for (int i = 0; i < RX_BUFFER_SIZE; i++) {
    Serial.print(rxBuffer[i], HEX);
    if (i < RX_BUFFER_SIZE - 1)
      Serial.print(", ");
  }
  Serial.println("]");

  Serial.print("\trxHead:\t\t");
  Serial.println(rxHead);
  Serial.print("\trxTail:\t\t");
  Serial.println(rxTail);
  Serial.print("\trxBitMask:\t");
  Serial.println(rxBitMask, BIN);
  Serial.print("\ttxBitMask:\t");
  Serial.println(txBitMask, BIN);
  Serial.print("\trxPort:\t\t");
  Serial.println((uint32_t)(rxPort), HEX);
  Serial.print("\ttxPort:\t\t");
  Serial.println((uint32_t)(txPort), HEX);

  Serial.println("=====SWUARTInstanceMap=====");
  for (int i = 0; i < MAX_NUM_MUART; i++) {
    Serial.print((uint32_t)(SWUARTInstanceMap[i]), HEX);
    if (i < MAX_NUM_MUART - 1)
      Serial.print(",\t");
    if (i != 0 && i % 5 == 0)
      Serial.println();
  }
  Serial.println();
  Serial.println();
#endif

  interrupts(); // For some reason this is turned off by default
  set_csr(mstatus, MSTATUS_MIE);
} // listen

//----------------------------------------------------------------------------

void SoftwareSerial32::ignore()
{
  if (listener) {
    noInterrupts();
    listener = (SoftwareSerial32 *) NULL;
    // Call the globally defined PLIC handling detachInterrupt
    ::detachInterrupt(digitalPinToInterrupt(rxPin));
    interrupts();
  }
} // ignore

//----------------------------------------------------------------------------

void SoftwareSerial32::setBaudRate(uint32_t baudRate)
{
  if (((baudRate ==  9600)         ||
       (baudRate == 19200)         ||
       (baudRate == 31250)         ||
       (baudRate == 38400)         ||
       (baudRate == 57600)         ||
       (baudRate == 115200)        ||
       ((F_CPU >= 256000000L)      &&
         ((baudRate == 230400)     ||
          (baudRate == 250000)     ||
          (baudRate == 500000)     ||
          (baudRate == 1000000)))) &&
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
    if (checkRxTime())
      avail = 1;

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
  #if defined(_SWSERIAL32_DBG)
    GPIO_REG(GPIO_OUTPUT_VAL) |= (1 << RED_LED_OFFSET);
  #endif
  noInterrupts();
  uint64_t t0;
  rdmcycle(&t0);

  // Load context from pinout to state map
  SoftwareSerial32* context = SWUARTInstanceMap[pin];

  uint32_t rxPort = *portInputRegister(digitalPinToPort(pin));
  uint32_t d = rxPort & context->rxBitMask;

  if (context->rxState == WAITING_FOR_START_BIT) {

    // If it looks like a start bit then initialize,
    // otherwise ignore the rising edge and exit.

    if (d != 0) return;   // it's high so not a start bit, exit
    context->startChar();

  } else {  // data bit or stop bit (probably) received

    // Determine how many bit periods have elapsed since the last transition.

    uint32_t rxBits          = context->bitTimes(t0 - context->prev_t0);
    uint8_t  bitsLeft        = 9 - context->rxState; // ignores stop bit
    bool     nextCharStarted = (rxBits > bitsLeft);

    uint32_t  bitsThisFrame   =  nextCharStarted ? bitsLeft : rxBits;

    context->rxState += bitsThisFrame;

    // Set all those bits

    if (d == 0) {
      // back fill previous bits with 1's
      while (bitsThisFrame-- > 0) {
        context->rxValue |= context->rxMask;
        context->rxMask   = context->rxMask << 1;
      }
      context->rxMask = context->rxMask << 1;
    } else { // d==1
      // previous bits were 0's so only this bit is a 1.
      context->rxMask   = context->rxMask << (bitsThisFrame-1);
      context->rxValue |= context->rxMask;
    }

    // If 8th bit or stop bit then the character is complete.

    if (context->rxState > 7) {
      context->rxChar( context->rxValue );

      if ((d == 1) || !nextCharStarted) {
        context->rxState = WAITING_FOR_START_BIT;
        // DISABLE STOP BIT TIMER

      } else {
        // The last char ended with 1's, so this 0 is actually
        //   the start bit of the next character.

        context->startChar();
      }
    }
  }
  context->prev_t0 = t0;  // remember time stamp
  // The same bitmask is used for the GPIO fields registers and is already obtained
  // by digitalPinToBitMask called by #::listen
  GPIO_REG(GPIO_FALL_IP) |= context->rxBitMask;
  GPIO_REG(GPIO_RISE_IP) |= context->rxBitMask;
  interrupts();
  #if defined(_SWSERIAL32_DBG)
    GPIO_REG(GPIO_OUTPUT_VAL) &= ~(1 << RED_LED_OFFSET);
  #endif
} // rxISR

//----------------------------------------------------------------------------

bool SoftwareSerial32::checkRxTime()
{
  #if defined(_SWSERIAL32_DBG)
    GPIO_REG(GPIO_OUTPUT_VAL) |= (1 << GREEN_LED_OFFSET);
  #endif
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
        if (!_isr) {
          #if defined(_SWSERIAL32_DBG)
            GPIO_REG(GPIO_OUTPUT_VAL) &= ~(1 << GREEN_LED_OFFSET);
          #endif
          return true;
        }
      }
    }
  }
  #if defined(_SWSERIAL32_DBG)
    GPIO_REG(GPIO_OUTPUT_VAL) &= ~(1 << GREEN_LED_OFFSET);
  #endif
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
    SoftwareSerial32::rxISR(pin);           \
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
  #if defined(_SWSERIAL32_DBG)
    GPIO_REG(GPIO_OUTPUT_VAL) |= (1 << BLUE_LED_OFFSET);
  #endif

  if (!txPort)
    return 0;

  uint32_t txBit  = 0;
  uint32_t b      = 0;
  noInterrupts();
  uint64_t t0, t1;
  rdmcycle(&t0);

  while (txBit++ < 9) {         // repeat for start bit + 8 data bits
    if (b)                      // if bit is set
      *txPort |= txBitMask;     // set TX line high
    else
      *txPort &= ~txBitMask;    // else set TX line low

    // Hold the line for the bit duration
    rdmcycle(&t1);
    while ((uint32_t)(t1 - t0) < txBitWidth) {
      // If a HW interrupt is pending while writing use rxISR to handle it
      // in order to be able to support the asynchronousity of a UART
      if (read_csr(mip) & MIP_MEIP) {
        clear_csr(mip, MIP_MEIP);
        GPIO_REG(GPIO_FALL_IP) |= txBitMask;
        GPIO_REG(GPIO_RISE_IP) |= txBitMask;
        rxISR(rxPin);
      } else {
        checkRxTime();
      }
      rdmcycle(&t1);
    }
    t0    += txBitWidth;    // advance start time
    b      = txChar & 0x01; // get next bit in the character to send
    txChar = txChar >> 1;   // shift character to expose the following bit
  }

  *txPort |= txBitMask;   // stop bit is high
  rdmcycle(&t1);
  interrupts();
  while ((uint32_t)(t1 - t0) < txBitWidth) {
    checkRxTime();
    rdmcycle(&t1);
  }

  #if defined(_SWSERIAL32_DBG)
    GPIO_REG(GPIO_OUTPUT_VAL) &= ~(1 << BLUE_LED_OFFSET);
  #endif

  return 1;               // 1 character sent
}
