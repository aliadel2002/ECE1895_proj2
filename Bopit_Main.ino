#include <Adafruit_LSM6DS3TRC.h>
#include <Wire.h>
#include <U8g2lib.h>

// Set Pins (Add pins for score and sound board)
//constexpr uint8_t PIN_NAME = 0x00;

// IMU addresses (CS needs to be tied to VCC on IMU 2)
constexpr uint8_t IMU_1_ADDR = 0x6A;
constexpr uint8_t IMU_2_ADDR = 0x6B;

// Devices
Adafruit_LSM6DS3TRC imu1;
Adafruit_LSM6DS3TRC imu2;

// Display
U8G2_SSD1306_128X64_NONAME_1_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

// Thresholds (Needs to be tuned)
constexpr int SHAKE_T = 250;
constexpr int TILT_T  = 100;
constexpr int TWIST_T = 70;
constexpr int PUNCH_T = 200;

constexpr unsigned long COOLDOWN_MS = 600;

// Game
constexpr int WIN_SCORE = 10;
const char* commands[] = {"PUNCH", "SHAKE", "TWIST", "TILT"};
constexpr int NUM_COMMANDS = 4;

int score1 = 0;
int score2 = 0;
const char* currentCommand = nullptr;

enum GameState { WAITING, ACTIVE, GAME_OVER };
GameState state = WAITING;

unsigned long roundStartTime = 0;
unsigned long waitDuration = 0;

// Sensor data
static sensors_event_t accel1, gyro1, temp1;
static sensors_event_t accel2, gyro2, temp2;

// Timing
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
  if (now - lastDetect < COOLDOWN_MS) return nullptr;

  int ax = absScaled(accel.acceleration.x);
  int ay = absScaled(accel.acceleration.y);
  int az = absScaled(accel.acceleration.z);

  int gx = absScaled(gyro.gyro.x);
  int gy = absScaled(gyro.gyro.y);
  int gz = absScaled(gyro.gyro.z);

  int shake = ax + ay + az;
  int punch = max(ax, max(ay, az));
  int tilt = ax + ay;
  int twist = gx + gy + gz;

  const char* gesture = nullptr;

  if (punch > PUNCH_T) gesture = "PUNCH";
  else if (shake > SHAKE_T) gesture = "SHAKE";
  else if (twist > TWIST_T) gesture = "TWIST";
  else if (tilt > TILT_T) gesture = "TILT";

  if (gesture) lastDetect = now;

  return gesture;
}

// Display
static void render()
{
  display.firstPage();
  do {
    display.setFont(u8g2_font_ncenB08_tr);

    display.drawStr(0, 10, "BOPIT");

    char buf[32];

    sprintf(buf, "P1:%d  P2:%d", score1, score2);
    display.drawStr(0, 25, buf);

    if (state == ACTIVE && currentCommand) {
      display.drawStr(0, 45, currentCommand);
    }

    if (state == GAME_OVER) {
      if (score1 >= WIN_SCORE)
        display.drawStr(0, 60, "P1 WINS");
      else
        display.drawStr(0, 60, "P2 WINS");
    }

  } while (display.nextPage());
}

// Setup
void setup()
{
  Wire.begin();
  //pinMode(LED, OUTPUT);
  // SET PINMODES HERE

  imu1.begin_I2C(IMU_1_ADDR);
  imu2.begin_I2C(IMU_2_ADDR);

  display.begin();
  randomSeed(analogRead(0));

  waitDuration = random(3000, 5000);
  roundStartTime = millis();
}

// Loop
void loop()
{
  unsigned long now = millis();

  imu1.getEvent(&accel1, &gyro1, &temp1);
  imu2.getEvent(&accel2, &gyro2, &temp2);

  const char* g1 = detectGesture(accel1, gyro1, lastDetect1, now);
  const char* g2 = detectGesture(accel2, gyro2, lastDetect2, now);

  switch (state)
  {
    case WAITING:
      // Start Round after random wait is over
      if (now - roundStartTime >= waitDuration) {
        currentCommand = commands[random(NUM_COMMANDS)];
        state = ACTIVE;
      }
      break;

    case ACTIVE:
      // Comp p1 and p2 command to current command
      if (g1 && strcmp(g1, currentCommand) == 0) {
        score1++;
        state = WAITING;
      }
      else if (g2 && strcmp(g2, currentCommand) == 0) {
        score2++;
        state = WAITING;
      }

      // End game condition
      if (score1 >= WIN_SCORE || score2 >= WIN_SCORE) {
        state = GAME_OVER;
      }

      if (state == WAITING) {
        waitDuration = random(3000, 5000);
        roundStartTime = now;
        delay(500);
      }
      break;

    case GAME_OVER:
      break;
  }

  render();
  delay(20);
}
