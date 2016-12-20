#include "Arduino.h"
#include "wiring_analog.h"
#include "platform.h"

__BEGIN_DECLS

static int _readResolution = 10;
static int _writeResolution = 8;

static uint8_t pwm_enabled[VARIANT_NUM_PWM];
static int8_t pwm_enabled_pin[VARIANT_NUM_PIN];

void analogReadResolution(int res) {
	_readResolution = res;
}

void analogWriteResolution(int res) {
	_writeResolution = res;
}

void analogWritePhase(uint32_t pin, uint32_t phase)
{
  int8_t pwm_num;
  
  if(pin >= variant_pin_map_size)
    return;

  pwm_num = variant_pin_map[pin].pwm_num;
  if(pwm_num > variant_pwm_size)
    {
      //TODO -- Not sure what this function is supposed to
      //        do exactly
    }
}
      
void analogOutputInit( void )
{
}


void analogWrite(uint32_t pin, uint32_t ulValue)
{

  volatile uint8_t pwm_num;
  volatile uint8_t pwm_cmp_num;
  uint32_t pwm_period;
  
  if (pin > variant_pin_map_size) {
    return;
  }

  pwm_num = variant_pin_map[pin].pwm_num;
  pwm_cmp_num = variant_pin_map[pin].pwm_cmp_num;

  if (pwm_num > variant_pwm_size) {
    return;
  }

  pwm_period = (1 << _writeResolution) - 1;

  
  // This also sets the scale to 0.
  if (!pwm_enabled[pwm_num]) {
    *((volatile uint32_t*) (variant_pwm[pwm_num] + PWM_CFG))   = 0;
    *((volatile uint32_t*) (variant_pwm[pwm_num] + PWM_COUNT)) = 0;
    *((volatile uint32_t*) (variant_pwm[pwm_num] + PWM_CMP0))  = pwm_period;
    *((volatile uint32_t*) (variant_pwm[pwm_num] + PWM_CFG))   = (PWM_CFG_ZEROCMP | PWM_CFG_ENALWAYS);
    pwm_enabled[pwm_num] = 1;
  }
  
  if (!pwm_enabled_pin[pin]) {
    GPIO_REG(GPIO_PULLUP_EN)  &= ~digitalPinToBitMask(pin);
    GPIO_REG(GPIO_OUTPUT_XOR)    &= ~digitalPinToBitMask(pin);
    GPIO_REG(GPIO_IOF_SEL)    |= digitalPinToBitMask(pin);
    GPIO_REG(GPIO_IOF_EN)     |= digitalPinToBitMask(pin);
    pwm_enabled_pin[pin] = 1;
  }

  // On the FE300 Platform, the outputs are LOW until their comparator matches.
  // So use the period subtracted from the specified value.
  *((volatile uint32_t*) (variant_pwm[pwm_num] + PWM_CMP0 + pwm_cmp_num*4)) =
    (ulValue > pwm_period) ? 0 : (pwm_period -  ulValue);

}

// FE300 Does not have analog inputs
uint32_t analogRead(uint32_t pin)
{
  return 0;
}

__END_DECLS
