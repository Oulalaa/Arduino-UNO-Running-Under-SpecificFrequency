#include <Arduino.h>

#include <Crypto.h>
#include <AES.h>
#include <string.h>
// 定义默认时钟频率（Teensy 2.0 默认为 16MHz）
#include <Crypto.h>
#include <RNG.h>
//#include <TransistorNoiseSource.h>
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
//key[16] cotain 16 byte key(128 bit) for encryption
byte key[16]={0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
//plaintext[16] contain the text we need to encrypt
byte plaintext[]=
{0x00, 0x11, 0x22, 0x33, 
0x44, 0x55, 0x66, 0x77,
0x88,0x99, 0xAA, 0xBB, 
0xCC, 0xDD, 0xEE, 0xFF,
0x00, 0x11, 0x22, 0x33, 
0x44, 0x55, 0x66, 0x77,
0x88,0x99, 0xAA, 0xBB, 
0xCC, 0xDD, 0xEE, 0xFF
};
//cypher[16] stores the encrypted text
byte cypher[32];
//decryptedtext[16] stores decrypted text after decryption
byte decryptedtext[16];
//creating an object of AES128 class
AES128 aes128;
     aes128.setKey(key,16);// Setting Key for AES
//  
//  Serial.print("Before Encryption:");
//  for(int i=0; i<sizeof(plaintext); i++){
//    Serial.write(plaintext[i]);
//   }
   
  aes128.encryptBlock(cypher,plaintext);
}
void my() {
//  byte key[32];
byte iv[16];
 

//    RNG.rand(key, sizeof(key));
    RNG.rand(iv, sizeof(iv));

  
delay(1000);
}
void setup() {
  // 初始化串行通信
  Serial.begin(9600);

  // 在这里设置当前时钟频率，以反映Teensy 2.0的实际时钟频率
//   currentClockFrequency = 8000000UL;

Serial.begin(9600);
//  aes128.setKey(key,16);// Setting Key for AES
////  
////  Serial.print("Before Encryption:");
////  for(int i=0; i<sizeof(plaintext); i++){
////    Serial.write(plaintext[i]);
////   }
//   
//  aes128.encryptBlock(cypher,plaintext);//cypher->output block and plaintext->input block
//  Serial.println();
//  Serial.print("After Encryption:");
//  for(int j=0;j<sizeof(cypher);j++){
//      Serial.write(cypher[j]);
//    }
    
//   aes128.decryptBlock(decryptedtext,cypher);
   
//  Serial.println();
//  Serial.print("After Dencryption:");
//  for(int i=0; i<sizeof(decryptedtext); i++){
//    Serial.write(decryptedtext[i]);
//   }

}
void loop() {
  // Number of times to run myFunction()
  const uint16_t numRuns = 1;

  // Total execution time
  uint32_t totalExecutionTime = 0;

  // Run myFunction() numRuns times and add up the execution times
  for (uint16_t i = 0; i < numRuns; i++) {
    totalExecutionTime += measureExecutionTime(my);
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
