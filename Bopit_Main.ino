#include <Adafruit_LSM6DS3TRC.h>
#include <Wire.h>

// Pin Constants
constexpr uint8_t Button = 9;
constexpr uint8_t LED1 = 8;

// I2C Config
constexpr uint8_t IMU_1_ADDR = 0x6A;
//constexpr uint8_t IMU_2_ADDR = 0x6B;

Adafruit_LSM6DS3TRC player1;
//Adafruit_LSM6DS3TRC player2;

void setup() {
  // Set Pin Modes
  pinMode(Button, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);

  // Setup I2C and Serial
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  Serial.println("TEST");

  // Initialize Sensors
  if (!player1.begin_I2C(IMU_1_ADDR)) {
    Serial.println("Couldn't find IMU 1 at 0x6A");
  }

  // if (!player2.begin_I2C(IMU_2_ADDR)) {
  //   Serial.println("Couldn't find IMU 2 at 0x6B");
  // }

}

void loop() {
  sensors_event_t accel1, gyro1, temp1;
  //sensors_event_t accel2, gyro2, temp2;
  delay(500);

  // while(digitalRead(Button) == 1){
  //   digitalWrite(LED1, HIGH);
  //   delay(500);
  //   digitalWrite(LED1, LOW);
  //   delay(500);
  // }
  
}
