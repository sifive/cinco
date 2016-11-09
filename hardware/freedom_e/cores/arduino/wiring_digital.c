/*
 * ULX2S board GPIO & hardwired pins
 */

#include "Arduino.h"
#include "wiring_analog.h" // this is needed to turn off pwm
#include <dev/io.h>

__BEGIN_DECLS

void
pinMode(uint32_t pin, uint32_t mode)
{
	volatile uint32_t *input_en = (volatile uint32_t *) (GPIO_BASE_ADDR + GPIO_input_en);
	volatile uint32_t *output_en = (volatile uint32_t *) (GPIO_BASE_ADDR + GPIO_output_en);
	volatile uint32_t *pullup = (volatile uint32_t *) (GPIO_BASE_ADDR + GPIO_pullup_en);

//	if (pin >= variant_pin_map_size || digitalPinToPort(pin) != (GPIO_BASE_ADDR + GPIO_port))
//		return;

	if (pin > 31)
		return;

	switch (mode) {
	case INPUT_PULLUP:
		*input_en  |=  (1<<(pin));
		*output_en &= ~(1<<(pin));
		*pullup    |=  (1<<(pin));
		break;
	case INPUT:
		*input_en  |=  (1<<(pin));
		*output_en &= ~(1<<(pin));
		*pullup    &= ~(1<<(pin));
		break;
	case OUTPUT:
		*input_en  &= ~(1<<(pin));
		*output_en |=  (1<<(pin));
		*pullup    &= ~(1<<(pin));
		break;
	}
}


void
digitalWrite(uint32_t pin, uint32_t val)
{
	volatile uint32_t *port, *value;
	int8_t pwm_channel;

	if (pin > 31)
		return;		

	// if port has PWM channel, turn PWM off
/*	pwm_channel = variant_pin_map[pin].pwm;
	if(pwm_channel != OCP_NONE)
	{
          EMARD_TIMER[TC_CONTROL] &= ~pwm_enable_bitmask[pwm_channel].control_or;
          EMARD_TIMER[TC_APPLY] = pwm_enable_bitmask[pwm_channel].apply;
	} */

//	port = (PortRegister_t)digitalPinToPort(pin);
	
	port  = (PortRegister_t)(GPIO_BASE_ADDR + GPIO_port);

	if (val)
		*port |=  (1<<pin);
	else
		*port &= ~(1<<pin);
}


int
digitalRead(uint32_t pin)
{
	volatile uint32_t *value;

	if (pin > 31)
		return 0;

	value = (PortRegister_t)(GPIO_BASE_ADDR + GPIO_value);
	return ((*value & (1<<pin)) != 0);
}

__END_DECLS
