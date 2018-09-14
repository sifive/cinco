/*
  Example of the SimpleTouchscreen library usage - drawing with touch screen pen. 

  Tested with SPI "2.8 TFT SPI 240x320 V1.1" ILI9341 based LCD display panel
  with XPT2046 touch screen controller and HiFive1 board from SiFive.

  The example uses standard Adafruit libraries to operate the LCD.

  For HiFive1 we don't need 5V to 3.3V voltage converters, just set IOREF jumper
  to 3.3V. See README for LCD panel to HiFive1 connection details.

  For Arduino UNO you will need a voltage level conversion.

  By default, library is calibrated for SPI "2.8 TFT SPI 240x320 V1.1" panel.
  See README for how to calibrate it for different panels.

  Both, HiFive1 board and the TFT panel are capable of running at 40MHz SPI
  clock when connected directly as described in README. You can try this by
  passing 40000000 into lcd.begin().

  Michael Zaidman <michael.zaidman@wdc.com>, 2018

  This example code is in the public domain.
*/

#include "Adafruit_ILI9341.h"
#include "SimpleTouchscreen.h"

//LCD
#define TFT_DC 9
#define TFT_CS 10

//Touchscreen
#define T_SS 7
#define T_IRQ 2

Adafruit_ILI9341 lcd = Adafruit_ILI9341(TFT_CS, TFT_DC);
SimpleTouchscreen ts = SimpleTouchscreen(T_IRQ, T_SS);

Adafruit_GFX_Button bt;

word x, y;

void setRotation (byte rotation) {
  lcd.setRotation(rotation);
  ts.setRotation(rotation);
}

void setup() {
  lcd.begin(12000000);
  ts.begin();
  setRotation(LANDSCAPE_270);
  lcd.fillScreen(ILI9341_BLACK);
  bt.initButton(&lcd, 28, 20, 50, 30, ILI9341_YELLOW,
                ILI9341_RED, ILI9341_WHITE, "Clr", 2);
  bt.drawButton();
}

void loop() {
  if (ts.dataAvailable()) {
    ts.read();
    x = ts.getX();
    y = ts.getY();
    if (bt.contains(x, y)) {
      lcd.fillScreen(ILI9341_BLACK);
      bt.drawButton();  
    } else {
      lcd.drawFastVLine(x, y, 1, ILI9341_YELLOW);
    }
  }
}
