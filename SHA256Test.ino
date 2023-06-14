#include <util/delay.h>
#include<Ascon128.h>
#include <Crypto.h>
// Select the desired frequency by uncommenting one of the following lines
//#define FREQ_16MHZ
//#define FREQ_8MHZ
//#define FREQ_4MHZ
#define FREQ_2MHZ

// 16/8 MHz :19200
// 4MHz :38400
//2MHz:  74880
uint32_t clock_prescaler;


uint32_t adjustedMillis() {
  return millis() * clock_prescaler;
}

uint32_t adjustedMicros() {
  return micros() * clock_prescaler;
}
void setClockPrescaler(uint8_t prescaler) {
  uint8_t clkpr_value;

  switch (prescaler) {
    case 1: clkpr_value = 0; break;
    case 2: clkpr_value = 1; break;
    case 4: clkpr_value = 2; break;
    case 8: clkpr_value = 3; break;
    default: return;
  }

  cli(); // Disable interrupts
  CLKPR = _BV(CLKPCE); // Enable change of the clock prescaler
  CLKPR = clkpr_value; // Set the clock prescaler value
  sei(); // Enable interrupts
}
void setup() {
  Serial.begin(74880);
  
  #if defined(FREQ_16MHZ)
    clock_prescaler = 1;
  #elif defined(FREQ_8MHZ)
    clock_prescaler = 2;
  #elif defined(FREQ_4MHZ)
    clock_prescaler = 4;
  #elif defined(FREQ_2MHZ)
    clock_prescaler = 8;
  #else
    #error "Please define a frequency"
  #endif

  // Set the clock prescaler
  setClockPrescaler(clock_prescaler);

measureTime();
  
}
uint32_t elapsedMicros;
uint32_t startMicros;
void loop() {
  
}

void measureTime() {

   startMicros = adjustedMicros();

  Serial.print("Start micros: ");
  Serial.println(startMicros);

  myFunction();


  elapsedMicros = adjustedMicros() - startMicros;
  Serial.print("Elapsed time: ");

  Serial.print(elapsedMicros);
  Serial.println(" us)");
}


void customDelay(uint32_t delay_ms) {
  uint32_t start = adjustedMillis();
  while (adjustedMillis() - start < delay_ms) {
    // do nothing, just wait
  }
}


void myFunction() {
  
SHA256 sha256;
  byte hmac[68];
const uint8_t key[32] = {
  0x2b, 0x7e, 0x15, 0x16,
  0x28, 0xae, 0xd2, 0xa6,
  0xab, 0xf7, 0x15, 0x88,
  0x09, 0xcf, 0x4f, 0x3c,
    0x2b, 0x7e, 0x15, 0x16,
  0x28, 0xae, 0xd2, 0xa6,
  0xab, 0xf7, 0x15, 0x88,
  0x09, 0xcf, 0x4f, 0x3c,
};
const uint8_t data1[] = {
  0x6b, 0xc1, 0xbe, 0xe2,
  //1 raw = 32 bits
  0x2e, 0x40, 0x9f, 0x96,
  0xe9, 0x3d, 0x7e, 0x11,
  0x73, 0x93, 0x17, 0x2a, 
  0xae, 0x2d, 0x8a, 0x57,
  0x1e, 0x03, 0xac, 0x9c,
  0x9e, 0xb7, 0x6f, 0xac,
  0x45, 0xaf, 0x8e, 0x51,
// 8 raws = 256 bits

  0x30, 0xc8, 0x1c, 0x46,
  0xa3, 0x5c, 0xe4, 0x11,
  0x6b, 0xc1, 0xbe, 0xe2,
  0x2e, 0x40, 0x9f, 0x96,
  0xe9, 0x3d, 0x7e, 0x11,
  0x73, 0x93, 0x17, 0x2a,
  0xae, 0x2d, 0x8a, 0x57,
  0x1e, 0x03, 0xac, 0x9c,
  //16 raw = 512 bits
  0xae, 0x2d, 0x8a, 0x57,

};
   unsigned int timecnt;


 sha256.resetHMAC(key, sizeof(key));
 sha256.update(data1, sizeof(data1));
 Serial.println("   start  ");
 timecnt = micros();
 sha256.finalizeHMAC(key, sizeof(key), hmac, sizeof(hmac));
}
