#include <util/delay.h>
#include <ACAN2515.h>
#include <Crypto.h>
#include<Ascon128.h>
// Select the desired frequency by uncommenting one of the following lines
//#define FREQ_16MHZ
//#define FREQ_8MHZ
#define FREQ_4MHZ
//#define FREQ_2MHZ
unsigned int timecnt;
bool same_mac=false;
uint32_t clock_prescaler;

uint32_t measureTime(uint32_t (*functionToMeasure)()) {
  uint32_t startMicros = adjustedMicros();
  
  uint32_t elapsedTime = functionToMeasure(); // Run the function
  
  uint32_t elapsedMicros = adjustedMicros() - startMicros;
  
  return elapsedMicros+elapsedTime;
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

void customDelayms(uint32_t delay_ms) {
  uint32_t start = adjustedMillis();
  while (adjustedMillis() - start < delay_ms) {
    // do nothing, just wait
  }
}

void customDelayus(uint32_t delay_us) {
  uint32_t start = adjustedMicros();
  while (adjustedMicros() - start < delay_us) {
    // do nothing, just wait
  }
}

uint32_t adjustedMillis() {
  return millis() * clock_prescaler;
}

uint32_t adjustedMicros() {
  return micros() * clock_prescaler;
}

////////////////////////////////////////////////////////////////////////////ECU text///////////////////////////////////////////////////////////////////////////

void ascon_enc(CANMessage *frame,CANMessage *frame_cipher,CANMessage *frame_mac){
  uint8_t key[] = {0x11,0x11,0x12,0x13,0x18,0x19,0x1a,0x1b,0x10,0x11,0x12,0x13,0x18,0x19,0x1a,0x1b};
  uint8_t IV[] = {0x13,0x18,0x19,0x1a,0x1c,0x10,0x11,0x12,0x13,0x18,0x19,0x1a,0x1b,0x12,0x11,0x12};
  
  //  encry_begin  
  timecnt = adjustedMicros();
  Ascon128 te;
  te.clear();
  te.setKey(key,16); //around 100us
  te.setIV(IV,16);
  te.encrypt(frame_cipher->data,frame->data,frame->len);
  timecnt = adjustedMicros()-timecnt;
  Serial.print("encry_timecnt: ");
  Serial.println(timecnt);

   //  auth_begin
//  timecnt = adjustedMicros();
//  te.addAuthData(frame->data,frame->len);
//  te.computeTag(frame_mac->data,frame->len);
//  timecnt = adjustedMicros()-timecnt;
//  Serial.print("mac_timecnt: ");
//  Serial.println(timecnt);
} 

void ascon_dec(CANMessage *frame,CANMessage *frame_cipher,CANMessage *frame_mac){
  uint8_t key[] = {0x11,0x11,0x12,0x13,0x18,0x19,0x1a,0x1b,0x10,0x11,0x12,0x13,0x18,0x19,0x1a,0x1b};
  uint8_t IV[] = {0x13,0x18,0x19,0x1a,0x1c,0x10,0x11,0x12,0x13,0x18,0x19,0x1a,0x1b,0x12,0x11,0x12};
 
  //  decry_begin  
  timecnt = adjustedMicros();
  Ascon128 te;
  te.clear();
  te.setKey(key,16); //around 100us
  te.setIV(IV,16);
  te.decrypt(frame->data,frame_cipher->data,frame_cipher->len);
  timecnt = adjustedMicros()-timecnt;
  Serial.print("decry_timecnt: ");
  Serial.println(timecnt);

  //auth_check_begin
  timecnt = adjustedMicros();
  same_mac=te.checkTag(frame_mac->data,frame->len);
  timecnt = adjustedMicros()-timecnt;
  Serial.print("mac_check_timecnt: ");
  Serial.println(timecnt);
}  
//static const byte MCP2515_SCK = 8 ; // SCK input of MCP2515
//static const byte MCP2515_SI  = 10 ; // SI input of MCP2515
//static const byte MCP2515_SO  = 7 ; // SO output of MCP2515

static const byte MCP2515_CS  = 10 ; // CS input of MCP2515
static const byte MCP2515_INT =  3; // INT output of MCP2515
ACAN2515 can (MCP2515_CS, SPI, MCP2515_INT) ;
static const uint32_t QUARTZ_FREQUENCY = 8UL * 1000UL * 1000UL ; // 16 MHz

//——————————————————————————————————————————————————————————————————————————————
//   SETUP
//——————————————————————————————————————————————————————————————————————————————
static uint32_t gBlinkLedDate = 0 ;
static uint32_t gReceivedFrameCount = 0 ;
static uint32_t gSentFrameCount = 0 ;

static void receive0 (const CANMessage & inMessage) {
    Serial.println();
    Serial.println ("Function_Receive0 ") ;
}
static void receive1 (const CANMessage & inMessage) {
  Serial.println();
 Serial.println ("Function_Receive1") ;
}
static void receive2 (const CANMessage & inMessage) {
    Serial.println();
    Serial.println ("Function_Receive2") ;
}


void setup() {
  Serial.begin(38400);
  
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

  Serial.print("Running at ");
  Serial.print((16 / clock_prescaler));
  Serial.println(" MHz");
  
  pinMode (LED_BUILTIN, OUTPUT) ;
  digitalWrite (LED_BUILTIN, HIGH) ;
//--- Start serial
//  Serial.begin (38400) ;
//--- Wait for serial (blink led at 10 Hz during waiting)
  while (!Serial) {
    delay (50) ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
  }
//--- Begin SPI
  SPI.begin () ;
//--- Configure ACAN2515
//  Serial.println ("Configure ACAN2515") ;
  ACAN2515Settings settings (QUARTZ_FREQUENCY, 125UL * 1000UL) ; // CAN bit rate 125 kb/s
  settings.mRequestedMode = ACAN2515Settings::LoopBackMode ; // Select loopback mode

  const ACAN2515Mask rxm0 = extended2515Mask (0x1ff800ff);
  const ACAN2515AcceptanceFilter filters [] = {
    {extended2515Filter (0x8200121), receive0},
    {extended2515Filter (0x4400121), receive0},
  }; 
  const uint32_t errorCode = can.begin (settings, [] { can.isr () ; }, rxm0, filters, 2);
  if (errorCode != 0) {
    Serial.print ("Configuration error 0x") ;
    Serial.println (errorCode, HEX) ;
  }
}

void loop() {
  const uint16_t numRuns = 1;

  // Total execution time
  uint32_t totalExecutionTime = 0;

  // Run myFunction() numRuns times and add up the execution times
  for (uint16_t i = 0; i < numRuns; i++) {
    totalExecutionTime += measureTime(myFunction);
  }

  // Calculate the average execution time
  uint32_t avgExecutionTime = totalExecutionTime / numRuns;

  // Output the average execution time
  Serial.print("Average execution time: ");
  Serial.print(avgExecutionTime);
  Serial.println(" us");

//  delay(5000); // Add a delay between measurements to avoid flooding the Serial Monitor
}

uint32_t myFunction() {
  // Your main loop code
  uint32_t timecnt = adjustedMicros();
  CANMessage frame,frame_mac,frame_cipher;

  frame.id = 0x8200121 ;   
  frame.len = 8;   
  frame.ext = true ;
  frame_cipher.id = frame.id;
  frame_cipher.len = frame.len;
  frame_cipher.ext = true;
  frame_mac.ext = true;
  frame_mac.id = frame.id;
  frame_mac.len = frame.len;
  frame.data[0]=0X12;
  frame.data[1]=0X12;
  frame.data[2]=0X12;
  frame.data[3]=0x12;
  frame.data[4] = 0X12;
  frame.data[5] = 0X12;
  frame.data[6] = 0X12;
  frame.data[7] = 0X12;

  // Encrypt the data in the frame
  ascon_enc(&frame,&frame_cipher,&frame_mac);

  // Compute the MAC
//  computeMAC(frame_cipher.data, frame.len, frame_mac.data, macKey);
    const bool ok_cipher = can.tryToSend (frame_cipher) ; 
    const bool ok_mac = 1 ;
//   const bool  ok_mac = can.tryToSend (frame_mac) ;      
    if(ok_cipher&&ok_mac){
  // Send the encrypted data and MAC
  can.tryToSend(frame_cipher);
//  can.tryToSend(frame_mac);
    }
  // Add any other required functionality here

  // Update timecnt with the time taken for the function to execute
  timecnt = adjustedMicros() - timecnt;
  return timecnt;
}
