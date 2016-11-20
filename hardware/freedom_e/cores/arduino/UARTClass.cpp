/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "UARTClass.h"

#include <dev/io.h>
#include <freedom_e300/platform/platform.h>
#include <freedom_e300/uart.h>
#include "variant.h"

#define _REG32(p,i)     (*(volatile uint32_t *)((p) + (i)))
#define _REG32P(p,i)    ((volatile uint32_t *)((p) + (i)))
#define GPIO_REG(i)     _REG32(GPIO_BASE_ADDR, (i))
#define UART_REG(i)     _REG32(UART0_BASE_ADDR, (i))
#define UART_REGP(i)    _REG32P(UART0_BASE_ADDR, (i))
#define PLIC_REG(i)     _REG32(PLIC_BASE_ADDR, (i))

#define PIN_RX          (1U << 16)
#define PIN_TX          (1U << 17)

#define PLIC_REG_PRIORITY(i)    (PLIC_PRIORITY_OFFSET + ((i) << PLIC_PRIORITY_SHIFT_PER_SOURCE))
#define PLIC_REG_ENABLE(i)      (PLIC_ENABLE_OFFSET + (((i) >> 5) << 2))
#define PLIC_ENABLE_MASK(i)     (1U << ((i) & 0x1f))


UARTClass Serial;
#if defined(UART1_BASE_ADDR)
UARTClass Serial1(UART1_BASE_ADDR);
#endif

int
UARTClass::sio_probe_rx()
{
  int32_t c;

  if ((c = UART_REG(UART_REG_RXFIFO)) >= 0) {
    sio_rxbuf[sio_rxbuf_head++] = c;
    sio_rxbuf_head &= SIO_RXBUFMASK;
    return(1);
  }
  return(0);
}


int
UARTClass::sio_getchar(int blocking)
{
  int c, busy;

  do {
    sio_probe_rx();
    busy = (sio_rxbuf_head == sio_rxbuf_tail);
  } while (blocking && busy);

  if (busy)
    return (-1);
  c = sio_rxbuf[sio_rxbuf_tail++];
  sio_rxbuf_tail &= SIO_RXBUFMASK;
  return (c);
}

int
UARTClass::sio_putchar(char c, int blocking)
{
  volatile uint32_t *val = UART_REGP(UART_REG_TXFIFO);
  uint32_t busy = (*val) & 0x80000000;
  if (blocking) {
    while (*val & 0x80000000);
  } else if (busy) {
      return 1;
  }
  UART_REG(UART_REG_TXFIFO) = c;
  return 0; 
}


/*
 * Set RS-232 baudrate.  Works well with FT-232R from 300 to 3000000 bauds.
 */
void
UARTClass::sio_setbaud(int bauds)
{

  //F_Baud = f_in/(div+1) 

  UART_REG(UART_REG_DIV) = F_CPU / bauds - 1;
 
}


// Public Methods //////////////////////////////////////////////////////////////

void
UARTClass::begin(unsigned long bauds)
{
  GPIO_REG(GPIO_input_en)  |= PIN_TX;
  GPIO_REG(GPIO_output_en) |= PIN_RX;
  GPIO_REG(GPIO_iof_sel)   &= ~(PIN_RX | PIN_TX);
  GPIO_REG(GPIO_iof_en)    |= IOF0_UART0_MASK;

  //F_Baud = f_in/(div+1) 

  UART_REG(UART_REG_DIV) = F_CPU / bauds - 1;
  UART_REG(UART_REG_TXCTRL) |= UART_TXEN;
  UART_REG(UART_REG_RXCTRL) |= UART_RXEN;

	
//  sio_setbaud(bauds);
}


void
UARTClass::end(void)
{
  GPIO_REG(GPIO_input_en)  &= ~PIN_TX;
  GPIO_REG(GPIO_output_en) &= ~PIN_RX;
  GPIO_REG(GPIO_iof_en)    &= ~IOF0_UART0_MASK;

  UART_REG(UART_REG_TXCTRL) &= ~UART_TXEN;
  UART_REG(UART_REG_RXCTRL) &= ~UART_RXEN;

}


int
UARTClass::available(void)
{

  sio_probe_rx();
  return (!(sio_rxbuf_head == sio_rxbuf_tail));
}


int
UARTClass::availableForWrite(void)
{
  int in, busy;
  in = sio_probe_rx();
  busy = ((int32_t)UART_REG(UART_REG_TXFIFO) < 0);
  return (!busy);
}


int
UARTClass::peek(void)
{
  sio_probe_rx();
  if (sio_rxbuf_tail == sio_rxbuf_head)
    return (-1);
  else
    return (sio_rxbuf[sio_rxbuf_tail]);
}


int
UARTClass::read(void)
{

  return (sio_getchar(1));
}


void
UARTClass::flush(void)
{
}


size_t
UARTClass::write(const uint8_t uc_data)
{

  sio_putchar(uc_data, 1);
  return (1);
}
