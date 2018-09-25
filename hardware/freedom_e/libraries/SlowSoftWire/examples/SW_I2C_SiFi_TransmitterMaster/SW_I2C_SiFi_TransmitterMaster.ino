/* Software I2C SiFi Transmitter Master
 * By Roy Weisfeld <roi.weisfeld@wdc.com>
 *
 * Example for basic I2C Transmitting using Software I2C Library with SiFive1 board
 * For this to work you also need a reciever slave (An example exists in the library)
 * Connect this SiFive1 board to another board using SDA --> SDA, SCL --> SCL
 * and GND --> GND.
 *
 * Created 13 September 2018
 *
 * This example code is in the public domain.
 */
#include <SlowSoftWire.h>

SlowSoftWire Wire = SlowSoftWire(18, 19, true); //SDA on port 18, SCL on port 19

void setup(void) {
  Serial.begin(57600);
  Wire.begin();
}

byte x = 0;

void loop(void) {
  Wire.beginTransmission(8); //Begin Master transmission to Address 8
  Wire.write(x);              // sends one byte
  Wire.endTransmission();    // stop transmitting
  x++;
  delay(100);
}
