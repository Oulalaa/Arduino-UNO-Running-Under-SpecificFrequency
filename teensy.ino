#include <Arduino.h>
#include<SHA256.h>
#include <Crypto.h>
// 定义默认时钟频率（Teensy 2.0 默认为 16MHz）
#define DEFAULT_CLOCK_FREQUENCY 16000000UL

// 声明一个变量以存储当前的时钟频率
uint32_t currentClockFrequency = DEFAULT_CLOCK_FREQUENCY;

// 声明测量函数原型
uint32_t measureExecutionTime(void (*functionToMeasure)());
uint32_t measureExecutionTime(void (*functionToMeasure)()) {
  // 获取当前时钟频率下的微秒计数速率
  float microsScaler = (float)DEFAULT_CLOCK_FREQUENCY / (float)currentClockFrequency;

  // 获取执行前的微秒计数
  uint32_t startMicros = micros();

  // 执行要测量的函数
  functionToMeasure();

  // 获取执行后的微秒计数
  uint32_t endMicros = micros();

  // 根据微秒计数速率计算实际执行时间
  uint32_t executionTime = (uint32_t)((float)(endMicros - startMicros) * microsScaler);

  return executionTime;}
  
  void myFunction() {
  // 你的程序段代码
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
//      Serial.println("   start  ");
//  timecnt = micros();
  sha256.finalizeHMAC(key, sizeof(key), hmac, sizeof(hmac));
//    timecnt = micros() - timecnt;
//      Serial.println("timecnt: ");
//  Serial.println(timecnt);
//      Serial.println("  end   ");
//    Serial.println("MAC: ");
//        for (int i = 0; i < sizeof(hmac); i++) 
//            Serial.print(hmac[i]);
//    Serial.println("     ");

}
void my() {
  // 你的主循环代码
delay(1000);
}
void setup() {
  // 初始化串行通信
  Serial.begin(9600);

  // 在这里设置当前时钟频率，以反映Teensy 2.0的实际时钟频率
//   currentClockFrequency = 8000000UL;



}
void loop() {
  // Number of times to run myFunction()
  const uint16_t numRuns = 1;

  // Total execution time
  uint32_t totalExecutionTime = 0;

  // Run myFunction() numRuns times and add up the execution times
  for (uint16_t i = 0; i < numRuns; i++) {
    totalExecutionTime += measureExecutionTime(myFunction);
  }

  // Calculate the average execution time
  uint32_t avgExecutionTime = totalExecutionTime / numRuns;

  // Output the average execution time
  Serial.print("Average execution time: ");
  Serial.print(avgExecutionTime);
  Serial.println(" us");

  // Delay before running the loop again
  delay(1000);
}
