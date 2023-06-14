#include <Arduino.h>
#include<Ascon128.h>
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
  uint8_t key[] = {0x00,0x01,0x02,0x03,0x08,0x09,0x0a,0x0b,0x10,0x11,0x12,0x13,0x18,0x19,0x1a,0x1b};
  uint8_t IV[] = {0x02,0x01,0x02,0x03,0x08,0x09,0x0a,0x0b,0x10,0x11,0x12,0x13,0x18,0x19,0x1a,0x1b};
  uint8_t plaintext[] = 
  {0x12,0x34,0x56,0x78,
  0x90,0x12,0x34,0x56,
  0x78,0x90,0x12,0x34,
   0x78,0x90,0x12,0x34,
   0x90,0x12,0x34,0x56,
  0x78,0x90,0x12,0x34,
   0x78,0x90,0x12,0x34,
  0x56,0x78,0x90,0x99
  };
  uint8_t ciphertext[32];

  uint8_t authtext[] = {0x12,0x34,0x56,0x78,
  0x90,0x12,0x34,0x56,
  0x78,0x90,0x12,0x34,
   0x78,0x90,0x12,0x34,
   0x90,0x12,0x34,0x56,
  0x78,0x90,0x12,0x34,
   0x78,0x90,0x12,0x34,
  0x56,0x78,0x90,0x99
  };
  uint8_t authtag[32];
  Ascon128 te;
  te.clear();
  te.setKey(key,16); //around 100us
  te.setIV(IV,16);
te.addAuthData (authtext, 32);
te.computeTag(authtag,32);
//  te.encrypt(ciphertext,plaintext,32);
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
  const uint16_t numRuns = 50;

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
