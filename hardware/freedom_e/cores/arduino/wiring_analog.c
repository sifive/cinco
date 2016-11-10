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

static inline uint32_t mapResolution(uint32_t value, uint32_t from, uint32_t to) {
	if (from == to)
		return value;
	if (from > to)
		return value >> (from-to);
	else
		return value << (to-from);
}


/* todo: handle the pin */
//void analogWritePhase(uint32_t pin, uint32_t phase)
//{
//  int8_t pwm_channel;
//  volatile uint32_t *start, *stop;
//
//  if(pin >= variant_pin_map_size)
//    return;
//
//  pwm_channel = variant_pin_map[pin].pwm;
//  if(pwm_channel != OCP_NONE)
//  {
//    start  = &EMARD_TIMER[pwm_enable_bitmask[pwm_channel].ocp_start];
//    stop   = &EMARD_TIMER[pwm_enable_bitmask[pwm_channel].ocp_stop];
//    
//    *stop  = phase + (*stop - *start);
//    *start = phase;
//    
//    EMARD_TIMER[TC_APPLY] = pwm_enable_bitmask[pwm_channel].apply;
//  }
//}

/* setup the common parameters (why isn't it called at startup?)
*/
void analogOutputInit( void )
{
}


static uint8_t PWMEnabled = 0;
static uint8_t PWMEnabledPins[4] = {0, 0, 0, 0};
static uint32_t PWMPeriod = 0;

void analogWrite(uint32_t ulPin, uint32_t ulValue)
{

 // if (ulPin < 1 || ulPin > 3)
  if (ulPin < 19 || ulPin > 22)
    return;

  if (!PWMEnabled)
  {
    PWM0_REG(PWM_CFG) = (PWM_CFG_ZEROCMP | PWM_CFG_ENALWAYS);
    PWMPeriod = 0xffff; // ~ 1kHz at 65MHZ cpu freq
    PWM0_REG(PWM_CMP0) = PWMPeriod;
    PWMEnabled = 1;
  }

  if (!PWMEnabledPins[ulPin-19]) 
  {
    GPIO_REG(GPIO_input_en)   &= ~(1<<ulPin);
    GPIO_REG(GPIO_pullup_en)  &= ~(1<<ulPin);
    GPIO_REG(GPIO_out_xor)    |= (1<<ulPin);
    GPIO_REG(GPIO_output_en)  |= (1<<ulPin);
    GPIO_REG(GPIO_iof_sel)    |= (1<<ulPin);
    GPIO_REG(GPIO_iof_en)     |= (1<<ulPin);
    PWMEnabledPins[ulPin-19] = 1;
  }

//  ulValue = mapResolution(ulValue, _writeResolution, 16);
  
  PWM0_REG(PWM_CMP0 + (4*(ulPin-19))) = ulValue > PWMPeriod ? PWMPeriod : ulValue;

}
  
  


// void analogWrite(uint32_t ulPin, uint32_t ulValue)
// {
//   int8_t pwm_channel;
//   volatile uint32_t *start, *stop;
//   
//   if(ulPin >= variant_pin_map_size)
//     return;
//     
//   pwm_channel = variant_pin_map[ulPin].pwm;
//   if(pwm_channel != OCP_NONE)
//   {
//     /* standard PWM frequency is 
//     ** 490 Hz on old arduino
//     ** 980 Hz in new arduino
//     ** for better high freq coverage and compatibility with
//     ** 12-bit analogWrite we need to recompile bitstream with 
//     ** PRESCALER_BITS=10 and with TIMER_BITS=12
//     ** to calculate increment:
//     ** increment = (frequency_hz << (TIMER_BITS+PRESCALER_BITS) ) / TIMER_CLOCK_hz;
//     */
//     EMARD_TIMER[TC_INCREMENT] = (((uint64_t)analog_write_frequency) 
//                                    << (TIMER_BITS+PRESCALER_BITS)) / TIMER_CLOCK;
// 
//     EMARD_TIMER[TC_CONTROL] &= pwm_enable_bitmask[pwm_channel].control_and;
//     /* shift timer step value to match the set resolution */
//     if(analog_write_resolution_bits < TIMER_BITS)
//       ulValue <<= (TIMER_BITS-analog_write_resolution_bits);
//     if(analog_write_resolution_bits > TIMER_BITS)
//       ulValue >>= (analog_write_resolution_bits-analog_write_resolution_bits);
//       
//     start = &EMARD_TIMER[pwm_enable_bitmask[pwm_channel].ocp_start];
//     stop  = &EMARD_TIMER[pwm_enable_bitmask[pwm_channel].ocp_stop];
//     
//     *stop  = /* ((1<<TIMER_BITS)-1) & */
//              ( *start
//              + ( ulValue < (1<<TIMER_BITS) ? ulValue : (1<<TIMER_BITS) )
//              );
// 
//     if( *start <= *stop )
//       EMARD_TIMER[TC_CONTROL] |= pwm_enable_bitmask[pwm_channel].control_and_or;
//     
//     #if 0
//     /* input caputre setting
//     ** this code doesn't belong here
//     ** initializing ICP engine to capture all events
//     */
//     #endif
// 
//     EMARD_TIMER[TC_CONTROL] |= pwm_enable_bitmask[pwm_channel].control_or;
//     EMARD_TIMER[TC_APPLY] = pwm_enable_bitmask[pwm_channel].apply;
//   }
// }

uint32_t analogRead(uint32_t ulPin)
{
  /* TODO
     external parallel RC circuit to GND,
     3 state output hardware that
     sets pin to output high, (charges C),
     set pin to input (high impedance, no pullup), 
     and captures time when input becomes low (C discharges),
     time is captured in a I/O mapped register
     which represents analog value of RC circuit
     
     similar as input capture, but with 3-state output and input
     on the same pin
  */
  uint32_t r = 0;

#if defined(IO_ADC_A0)
  /* TO-DONE: The above was done on FleaFPGA-Uno board and this code reads the result. :-) */
    volatile uint32_t *adc = (volatile uint32_t *)IO_ADC_A0;
	volatile uint32_t *port = (volatile uint32_t *) IO_GPIO_CTL;
	
	if (ulPin >= A0 && ulPin <= A5)
	{
		*port &= ~(1<<variant_pin_map[ulPin].bit_pos);	// make sure set to INPUT
		int16_t v = adc[ulPin-A0];
		if (v < IO_ADC_MINVAL)
			v = IO_ADC_MINVAL;
		if (v > IO_ADC_MAXVAL)
			v = IO_ADC_MAXVAL;
		r = (v - IO_ADC_MINVAL) * 1023 / (IO_ADC_MAXVAL - IO_ADC_MINVAL);	// scale 0 to 1023
	}
#endif

  return r;
}

/* input capture setting */
uint32_t setInputCapture(uint32_t ulPin)
{
  int8_t icp_channel;
  
  if(ulPin >= variant_pin_map_size)
    return 0;
    
  icp_channel = variant_pin_map[ulPin].icp;
  if(icp_channel != ICP_NONE)
  {

    #if 0
    /* input caputre setting
    ** this code doesn't belong here
    ** here belongs RC co
    
    ** initializing ICP engine to capture all events
    */
    EMARD_TIMER[TC_CONTROL] &= pwm_enable_bitmask[icp_channel].control_and;
    EMARD_TIMER[TC_CONTROL] |= pwm_enable_bitmask[icp_channel].control_or;
    EMARD_TIMER[TC_APPLY] = pwm_enable_bitmask[icp_channel].apply;
    /*
    todo:
    [ ] make RC component, and 3 state pin
    [ ] create icp control array
    [ ] 
    */
    #endif

  }
  return 0;
}

__END_DECLS
