/* Hardware I2C Uno Stress Test Slave Recieve
 * By Roy Weisfeld <roi.weisfeld@wdc.com>
 *
 * Run this to test I2c reliability between 2 devices, a Master and a Slave.
 * The test runs a loop that sends chars from 0 to 256 the number of itirations the user defines.
 *
 * (This is the Slave Device Code used on the Uno.
 * It is recommended to use the SW_I2C_SiFi_StressTest_MasterTransmit.
 * sketch on a HiFive1 Device)
 *
 * Created 13 September 2018
 *
 * This example code is in the public domain.
 */

#include <Wire.h>

#define ITERATIONS 10 //Change value here for more itirations
#define CHARS 256

uint8_t temp;
uint16_t success[256];
uint16_t fails = 0;
int counter = 0;
int iteration = 0;

void setup() {
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  //Serial.begin(9600);
  //Serial.print("Started Test");
  //Serial.println();
}

void loop() {
  if(iteration == ITERATIONS) { //Only starts printing results after defined itiration number
    Serial.begin(9600);
    for(int j = 0; j < CHARS; j++) {
        Serial.print(j); //Uncomment this block to check if you would like
        Serial.print("\t|\t"); //To see a detailed list of where the test failed
        Serial.print(success[j]);
        Serial.println();
        if(success[j] < ITERATIONS) {
            fails +=  ITERATIONS - success[j];
          }
      }
      iteration++;
      Serial.print("Chars have failed ");
      Serial.print(fails);
      Serial.print(" times");
      Serial.println();
  }
}

void receiveEvent(int howMany) {
  if(iteration < ITERATIONS) {//Only stops reading test after defined itiration number
    if (Wire.available() > 0) {
      temp = Wire.read();
      success[temp]++;
      counter++;
      if(counter == CHARS) {
        iteration++;
        counter = 0;
      }
    }
  }
}
