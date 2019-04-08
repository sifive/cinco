#if defined(FREEDOM_E300)
  #include <SoftwareSerial32.h>
  SoftwareSerial32 s(10, 11);
#else
  #include <SoftwareSerial.h>
  SoftwareSerial s(10, 11);
#endif

uint8_t character;
uint8_t counter = 0;

static const uint8_t burst_load = 50;

void setup() {
  s.begin(115200);
  character = 0;
  counter = 0;
}

void loop() {
  s.write(character);
  character++;
  counter++;
  if (counter % burst_load == 0) {
    delay(1);
    counter = 0;
  }
}
