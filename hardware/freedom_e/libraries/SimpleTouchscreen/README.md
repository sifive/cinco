# README

The simple touch screen library for SPI "2.8 TFT SPI 240x320 V1.1" LCD display
panel like [https://www.amazon.com/HiLetgo-240X320-Resolution-Display-ILI9341/dp/B073R7BH1B](https://www.amazon.com/HiLetgo-240X320-Resolution-Display-ILI9341/dp/B073R7BH1B)

![alt text](DSC_0058.JPG)

- Calibrated for SPI "2.8 TFT SPI 240x320 V1.1" ILI9341 LCD display panel bundled
with XPT2046 touchscreen controller.
- Returns raw (in touchscreen resolution) or normilized (in LCD  resolution) X and Y reading.
- Tested with HiFive1 and Arduino Uno boards.
- Tested with Adafruit_ILI9341 and Adafruit_GFX.h LCD libraries.
- For HiFive1 we do not need 5V to 3.3V voltage converters, just set IOREF jumper
to 3.3V. The pins were connected as follows:

|TFTLCD    |HiFive1|
|----------|-------|
|T_IRQ     |D2     |
|T_DO      |D12    |
|T_DI      |D11    |
|T_CS      |D7     |
|T_CLK     |D13    |
|SDO(MISO) |D12    |
|LED       |3.3V   |
|CLK       |D13    |
|SDI(MOSI) |D11    |
|D/C       |D9     |
|RESET     |RESET  |
|CS        |D10    |
|GND       |GND    |
|VCC       |3.3V   |

For LCD I used the standard Adafruit_ILI9341 library with default SPI clock of 24MHz.
Both, HiFive1 and TFT panel SPIs can be run at 40MHz clock when HiFive1 is
configured to run at 256MHz or 320MHz CPU clock.

### Why yet another touchscreen library?
Looked for touch screen library for "2.8 TFT SPI 240x320 V1.1" aftermarket panel
to use with RISC-V based HiFive1 board. Examined URTouch and XPT2046 libraries
but nothing worked for HiFive1 board out of the box. On top of that, none of the
libraries provided touchscreen to LCD resolution normalization.
So, I wrote the SimpleTouchscreen library with intention to keep it simple :-)

### Why to calibrate?
- The resolution of the touchscreen and LCD are different. In our example,
the 320x240 LCD is bundled with 4096x4096 touch screen.
- The touch screen panel is assembled on top of the LCD panel. Different
manufacturers do this differently. The start coordinates of the touch screen
panel not necessary match the start coordinates of the LCD panel. Moreover, the
size of the touch screen can exceed the size of the LCD screen. For example, the
X=0, Y=0 of the above LCD corresponds to X=390, Y=220 of the touch screen panel.

### How to calibrate
- You will need to figure out new calibration values (LowX, LowY, HighX, HighY)
for your TFT panel and pass them into begin() method. This will override the
default calibration values.
- Run SimpleTouchCalibrate.ino. You should see the raw X and Y values shown on
the screen upon every touch.
- Make sure that the display is landscape oriented (which is a default).
- Use touch screen pen to press in the top left corner of the screen and write
down the LowX and LowY values.
```
For example, you got X=300 and Y=200
```
- Press in the bottom right corner of the screen and record the new
HighX and HighY values.
```
Let's assume you got X=3300 and Y=3200.
```
- Pass these values into the begin() method:
```
ts.begin(300, 3300, 200, 3200);
```
- If the geometry of the screen is different, pass the new geometry, like width
and height into begin() as well:
```
ts.begin(300, 3300, 200, 3200, 480, 320);
```
