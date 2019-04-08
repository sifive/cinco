/*
  Example of the HiFive1 builtin RGB LED fading with RGBL library.

  On HiFive1 the RGB LED is connected to the following GPIOs:
    Red   - GPIO6
    Green - GPIO3
    Blue  - GPIO5

  This example code is in the public domain.

  By Michael Zaidman <michael.zaidman@wdc.com>, 2018
*/

#include <RGBL.h>

RGBL led;

void setup() {
}

void loop() {
  led.fade(10);
}
