#if defined(FREEDOM_E300)
  #include <SoftwareSerial32.h>
  SoftwareSerial32 serial(10, 11);
#else
  #include <SoftwareSerial.h>
  SoftwareSerial serial(10, 11);
#endif

static const uint32_t iters = 10;

uint32_t succ_arr[256];

uint32_t count = 0;
uint32_t misses = 0;


void setup() {
  serial.begin(115200);
  Serial.begin(9600);
}

void loop() {
  Serial.println("Started test");
  while (count < (iters << 8)) {
    if (serial.available()) {
      succ_arr[serial.read()]++;
      count++;
    }
  }

  for (int i = 0; i < 256; i++) {
    if (succ_arr[i] < 10)
      misses += (10 - succ_arr[i]);
  }
  Serial.println("Chr\t|\tCount");
  for (int i = 0; i < 256; i++) {
    Serial.print(i, HEX);
    Serial.print("\t|\t");
    Serial.print(succ_arr[i]);
    Serial.println();  
  }
  Serial.print("misses: ");
  Serial.println(misses);
  while (1){}
}
