/* Hardware I2C Uno Transmitter Test Master
 * By Roy Weisfeld <roi.weisfeld@wdc.com>
 *
 * Example for basic I2C Transmitting using Software I2C Library with Uno board
 * For this to work you also need a reciever slave (An example exists in the library)
 * Connect this Uno board to another board using SDA --> SDA, SCL --> SCL 
 * and GND --> GND.
 *
 * Created 13 September 2018
 *
 * This example code is in the public domain. 
 */
#include <SlowSoftWire.h>

SlowSoftWire Wire = SlowSoftWire(A4, A5); // SDA on port A4, SCL on port A5

void setup(void) {
  Serial.begin(57600);
  Wire.begin();
}

byte x = 0;

void loop(void) {
  Wire.beginTransmission(8); // Begin Master transmission to Address 8
  Wire.write("x is ");        // sends five bytes
  Wire.write(x);              // sends one byte
  Wire.endTransmission();    // stop transmitting
  x++;
  delay(1000);
}
