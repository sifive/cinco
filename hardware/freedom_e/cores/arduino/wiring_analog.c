#include "Arduino.h"
#include "wiring_analog.h"

#include <freedom_e300/platform/platform.h>

__BEGIN_DECLS

/* old arduino uses 490 Hz */
/* new arduino uses 980 Hz */
static uint32_t analog_write_frequency = 980;
static int _readResolution = 10;
static int _writeResolution = 8;

void analogReadResolution(int res) {
	_readResolution = res;
}

void analogWriteResolution(int res) {
	_writeResolution = res;
}

void analogWritePhase(uint32_t pin, uint32_t phase)
{
  int8_t pwm_num;
  volatile uint32_t *start, *stop;
  
  if(pin >= variant_pin_map_size)
    return;

  pwm_num = variant_pin_map[pin].pwm_num;
  if(pwm_num > variant_pwm_size)
    {
      //TODO!!! Whatever this is supposed to do.
    }
}
      
void analogOutputInit( void )
{
}


void analogWrite(uint32_t pin, uint32_t ulValue)
{

  uint8_t pwm_num;
  uint8_t pwm_cmp_num;
  uint32_t pwm_period;
  
  if (pin > variant_pin_map_size) {
    return;
  }

  pwm_num = variant_pin_map[pin].pwm_num;
  if (pwm_num > variant_pwm_size) {
    return;
  }

  pwm_period = (1 << _writeResolution) - 1;
  
  *((volatile uint32_t*) (variant_pwm[pwm_num] + PWM_CFG))  = (PWM_CFG_ZEROCMP | PWM_CFG_ENALWAYS);
  *((volatile uint32_t*) (variant_pwm[pwm_num] + PWM_CMP0)) = pwm_period;
  
  GPIO_REG(GPIO_pullup_en)  &= ~(1<<pin);
  GPIO_REG(GPIO_out_xor)    &= ~digitalPinToBitMask(pin);
  GPIO_REG(GPIO_iof_sel)    |= digitalPinToBitMask(pin);
  GPIO_REG(GPIO_iof_en)     |= digitalPinToBitMask(pin);
  
  *((volatile uint32_t*) (variant_pwm[pwm_num] + PWM_CMP0 + pwm_cmp_num)) =
    (ulValue > pwm_period) ? pwm_period : ulValue;
  
}

//TODO!!!
uint32_t analogRead(uint32_t pin)
{
  return 0;
}

__END_DECLS
