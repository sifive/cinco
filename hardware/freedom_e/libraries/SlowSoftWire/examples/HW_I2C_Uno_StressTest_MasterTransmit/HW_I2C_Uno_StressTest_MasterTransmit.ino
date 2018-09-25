/* Hardware I2C Uno Stress Test Master Transmit
 * By Roy Weisfeld <roi.weisfeld@wdc.com>
 *
 * Run this to test I2c reliability between 2 devices, a Master and a Slave.
 * The test runs a loop that sends chars from 0 to 256 the number of itirations the user defines.
 * (This is the Master Device Code used on the Uno)
 * SlowSoftWire Library is needed for this sketch to work.
 * Uno needs to be connected via A4 --> A4, A5 --> A5, GND --> GND.
 *
 * Created 13 September 2018
 *
 * This example code is in the public domain.
 */
 
#include <Wire.h>

#define CHARS 256

int counter = 0;
uint8_t character;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Wire.setClock(250000L);
  character = 0;
}

void loop() {
  Wire.beginTransmission(8);
  Wire.write(character);
  Wire.endTransmission();
  character++;
  counter++;
  if(counter == CHARS) {
    counter = 0;
  }
  if(character == CHARS) {
      character = 0;
    }
  }
