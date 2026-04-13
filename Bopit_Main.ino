#include <Adafruit_LSM6DS3TRC.h>
#include <Wire.h>
#include <U8g2lib.h>

// Pins
constexpr uint8_t LED = 0x08;

// IMU addresses
constexpr uint8_t IMU_1_ADDR = 0x6A;
constexpr uint8_t IMU_2_ADDR = 0x6B;

// Devices
Adafruit_LSM6DS3TRC imu1;
Adafruit_LSM6DS3TRC imu2;

// Display
U8G2_SSD1306_128X64_NONAME_1_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

// Thresholds
constexpr int SHAKE_T = 250;
constexpr int TILT_T  = 70;
constexpr int TWIST_T = 25;
constexpr int PUNCH_T = 200;

constexpr unsigned long COOLDOWN_MS = 600;

// Sensor data
static sensors_event_t accel1, gyro1, temp1;
static sensors_event_t accel2, gyro2, temp2;

// Timing state
static unsigned long lastDetect1 = 0;
static unsigned long lastDetect2 = 0;

// Helper
static inline int absScaled(float v)
{
  if (v < 0) {
      return (int)(-v * 10);
  } else {
      return (int)(v * 10);
  }
}

// Gesture detection
static const char* detectGesture(
  sensors_event_t& accel,
  sensors_event_t& gyro,
  unsigned long& lastDetect,
  unsigned long now)
{
  if (now - lastDetect < COOLDOWN_MS)
    return nullptr;

  int ax = absScaled(accel.acceleration.x);
  int ay = absScaled(accel.acceleration.y);
  int az = absScaled(accel.acceleration.z);

  int gx = absScaled(gyro.gyro.x);
  int gy = absScaled(gyro.gyro.y);
  int gz = absScaled(gyro.gyro.z);

  int shake = ax + ay + az;

  int punch = ax;
  if (ay > punch) punch = ay;
  if (az > punch) punch = az;

  int tilt = ax + ay;
  int twist = gx + gy + gz;

  const char* gesture = nullptr;

  if (punch > PUNCH_T) gesture = "PUNCH";
  else if (shake > SHAKE_T) gesture = "SHAKE";
  else if (twist > TWIST_T) gesture = "TWIST";
  else if (tilt > TILT_T) gesture = "TILT";

  if (gesture)
    lastDetect = now;

  return gesture;
}

// Display rendering
static void render(const char* gesture1, const char* gesture2)
{
  display.firstPage();
  do {
    display.setFont(u8g2_font_ncenB08_tr);

    display.drawStr(0, 12, "BOPIT");

    if (gesture1) {
      display.drawStr(0, 32, "P1:");
      display.drawStr(30, 32, gesture1);
    }

    if (gesture2) {
      display.drawStr(0, 52, "P2:");
      display.drawStr(30, 52, gesture2);
    }

  } while (display.nextPage());
}

// Setup
void setup()
{
  Wire.begin();

  pinMode(LED, OUTPUT);

  imu1.begin_I2C(IMU_1_ADDR);
  imu2.begin_I2C(IMU_2_ADDR);

  display.begin();

  render("READY", "READY");
}

// Main loop
void loop()
{
  digitalWrite(LED, LOW);
  imu1.getEvent(&accel1, &gyro1, &temp1);
  imu2.getEvent(&accel2, &gyro2, &temp2);

  unsigned long now = millis();

  const char* gesture1 = detectGesture(accel1, gyro1, lastDetect1, now);
  const char* gesture2 = detectGesture(accel2, gyro2, lastDetect2, now);

  render(gesture1, gesture2);

  if (gesture1 || gesture2) {
    digitalWrite(LED, HIGH);
    delay(1000);
  } else {
    delay(20);
  }

}
