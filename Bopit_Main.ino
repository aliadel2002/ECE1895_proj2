#include <Adafruit_LSM6DS3TRC.h>

#include <Wire.h>

// Pin Constants
constexpr uint8_t Button = 9;
constexpr uint8_t LED1 = 8;

// I2C Config
constexpr uint8_t P1_I2C_ADR = 0x00;
constexpr uint8_t P2_I2C_ADR = 0x00;

void setup() {
  // Setup I2C and Serial
  Wire.begin();
  Serial.begin(115200);
  Serial.println("test");

  // Set Pin Modes
  pinMode(Button, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
}

void loop() {

  while(digitalRead(Button) == 1){
    digitalWrite(LED1, HIGH);
    delay(500);
    digitalWrite(LED1, LOW);
    delay(500);
  }
  
}
