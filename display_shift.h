#ifndef DISPLAY_SHIFT_H
#define DISPLAY_SHIFT_H

#include <Arduino.h>

const int dataPin = 11;  // SER(14)
const int latchPin = 12; // RCLK(12)
const int clockPin = 13; // SRCLK(11)

// Lookup table for Common Cathode (Active HIGH)
const byte digitMap[] = {
  0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE0, 0xFE, 0xE6
};

void initDisplay() {
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
}

void displayNumbers(int n1, int n2) {
  int nums[4] = {n2%10, n2/10, n1%10, n1/10}; // data into seperated array
  
  for (uint8_t i = 0; i < 4; i++) {
    uint8_t val = digitMap[nums[i]]; // load byte

    for (uint8_t j = 0; j < 8; j++)  {
      digitalWrite(latchPin,LOW); // open latch
      digitalWrite(dataPin, val & 1); // read byte
      val >>= 1;
      digitalWrite(latchPin,HIGH); // close		
    }
    
    digitalWrite(clockPin,HIGH); // cycle clk
    digitalWrite(clockPin,LOW);
  }
}

#endif