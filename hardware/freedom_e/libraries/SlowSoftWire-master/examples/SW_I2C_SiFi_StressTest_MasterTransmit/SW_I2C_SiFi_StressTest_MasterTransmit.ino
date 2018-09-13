/* Hardware I2C Uno Stress Test Slave Recieve
 * By Roy Weisfeld <roi.weisfeld@wdc.com>
 *
 * Run this to test I2c reliability between 2 devices, a Master and a Slave.
 * The test runs a loop that sends chars from 0 to 256 the number of itirations the user defines.
 * (This is the Master Device Code used on the HiFive1)
 * SlowSoftWire Library is needed for this sketch to work.
 * HiFive1 needs to be connected via 2 digital ports of your choosing (In this case I used 18 for SDA and 19 for SCL),
 * and also connect the ground from this device to the second one.
 *
 * Created 13 September 2018
 *
 * This example code is in the public domain.
 */
 
#include <SlowSoftWire.h>

int counter = 0;

SlowSoftWire Wire = SlowSoftWire(18, 19, true); //HiFive1 can use internal pullups
uint8_t character;
void setup() {
  Wire.begin();
  Serial.begin(9600);
  character = 0;
}

void loop() {
  Wire.beginTransmission(8);
  Wire.write(character);
  character++;
  counter++;
  if(counter == 256) {
    delay(1);
    counter = 0;
  }
  if(character == 256) {
      character = 0;
    }
  }
