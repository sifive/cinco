/* Software I2C Uno Stress Test Master Transmit
 * By Roy Weisfeld <roi.weisfeld@wdc.com>
 *
 * Run this to test I2c reliability between 2 devices, a Master and a Slave.
 * The test runs a loop that sends chars from 0 to 256 the number of itirations the user defines.
 * (This is the Master Device Code used on the Uno)
 * SlowSoftWire Library is needed for this sketch to work.
 * Uno needs to be connected via 2 digital ports of your choosing (In this case I used A4 == 18 for SDA and A5 == 19 for SCL),
 * and also connect the ground from this device to the second one.
 *
 * Created 13 September 2018
 *
 * This example code is in the public domain.
 */
 
#include <SlowSoftWire.h>

#define CHARS 256

int counter = 0;
uint8_t character;

SlowSoftWire Wire = SlowSoftWire(18, 19, true); //Uno can use internal pullups

void setup() {
  Wire.begin();
  Wire.setClock(100000L);
  Serial.begin(9600);
  character = 0;
}

void loop() {
  Wire.beginTransmission(8);
  Wire.write(character);
  character++;
  counter++;
  if(counter == CHARS) {
    //delay(1);
    counter = 0;
  }
  if(character == CHARS) {
      character = 0;
    }
  }
