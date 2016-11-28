// See LICENSE file for license details.

#include "Arduino.h"

__BEGIN_DECLS

void
pinMode(uint32_t pin, uint32_t mode)
{
  
  if (pin >= variant_pin_map_size)
    return;
  
  GPIO_REG(GPIO_OUTPUT_XOR)  &= ~digitalPinToBitMask(pin);
  GPIO_REG(GPIO_IOF_EN)      &= ~digitalPinToBitMask(pin);
//pwm_enabled_pin[pin] = 0;
  
  switch (mode) {
  case INPUT_PULLUP:
    GPIO_REG(GPIO_INPUT_EN)  |=  digitalPinToBitMask(pin);
    GPIO_REG(GPIO_OUTPUT_EN) &= ~digitalPinToBitMask(pin);
    GPIO_REG(GPIO_PULLUP_EN) |=  digitalPinToBitMask(pin);
    break;
  case INPUT:
    GPIO_REG(GPIO_INPUT_EN)  |=  digitalPinToBitMask(pin);
    GPIO_REG(GPIO_OUTPUT_EN) &= ~digitalPinToBitMask(pin);
    GPIO_REG(GPIO_PULLUP_EN) &= ~digitalPinToBitMask(pin);
    break;
  case OUTPUT:
    GPIO_REG(GPIO_INPUT_EN)  &= ~digitalPinToBitMask(pin);
    GPIO_REG(GPIO_OUTPUT_EN) |=  digitalPinToBitMask(pin);
    GPIO_REG(GPIO_PULLUP_EN) &= ~digitalPinToBitMask(pin);
    break;
  }
}


void
digitalWrite(uint32_t pin, uint32_t val)
{
  if (pin >= variant_pin_map_size)
    return;
  
  if (val)
    GPIO_REG(GPIO_OUTPUT_VAL) |=  digitalPinToBitMask(pin);
  else
    GPIO_REG(GPIO_OUTPUT_VAL) &= ~digitalPinToBitMask(pin);

}

int
digitalRead(uint32_t pin)
{
if (pin >= variant_pin_map_size)
  return 0;

 return ((GPIO_REG(GPIO_INPUT_VAL) & digitalPinToBitMask(pin)) != 0);
}

__END_DECLS
