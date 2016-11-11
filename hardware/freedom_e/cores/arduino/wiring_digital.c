// See LICENSE file for license details.

#include "Arduino.h"

__BEGIN_DECLS

void
pinMode(uint32_t pin, uint32_t mode)
{
  
  if (pin >= variant_pin_map_size)
    return;
  
  GPIO_REG(GPIO_output_xor)  &= ~digitalPinToBitMask(pin);
  GPIO_REG(GPIO_iof_en)      &= ~digitalPinToBitMask(pin);
  
  switch (mode) {
  case INPUT_PULLUP:
    GPIO_REG(GPIO_input_en)  |=  digitalPinToBitMask(pin);
    GPIO_REG(GPIO_output_en) &= ~digitalPinToBitMask(pin);
    GPIO_REG(GPIO_pullup_en) |=  digitalPinToBitMask(pin);
    break;
  case INPUT:
    GPIO_REG(GPIO_input_en)  |=  digitalPinToBitMask(pin);
    GPIO_REG(GPIO_output_en) &= ~digitalPinToBitMask(pin);
    GPIO_REG(GPIO_pullup_en) &= ~digitalPinToBitMask(pin);
    break;
  case OUTPUT:
    GPIO_REG(GPIO_input_en)  &= ~digitalPinToBitMask(pin);
    GPIO_REG(GPIO_output_en) |=  digitalPinToBitMask(pin);
    GPIO_REG(GPIO_pullup_en) &= ~digitalPinToBitMask(pin);
    break;
  }
}


void
digitalWrite(uint32_t pin, uint32_t val)
{
  if (pin >= variant_pin_map_size)
    return;
  
  if (val)
    GPIO_REG(GPIO_port) |=  digitalPinToBitMask(pin);
  else
    GPIO_REG(GPIO_port) &= ~digitalPinToBitMask(pin);

}

int
digitalRead(uint32_t pin)
{
  if (pin >= variant_pin_map_size)
    return 0;

  return ((GPIO_REG(GPIO_value) & digitalPinToBitMask(pin)) != 0)
}

__END_DECLS
