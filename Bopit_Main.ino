#include <Adafruit_LSM6DS3TRC.h>
#include <Wire.h>
#include <U8g2lib.h>

// Set Pins (Add pins for score and sound board)
//constexpr uint8_t PIN_NAME = 0x00;

// IMU addresses
constexpr uint8_t IMU_1_ADDR = 0x6A;
constexpr uint8_t IMU_2_ADDR = 0x6B;

// Devices
Adafruit_LSM6DS3TRC imu1;
Adafruit_LSM6DS3TRC imu2;

// Display
U8G2_SSD1306_128X64_NONAME_1_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

// Thresholds
constexpr float TILT_Z_THRESHOLD   = -5.0f;
constexpr float TWIST_GZ_THRESHOLD =  1.5f;
constexpr float PUNCH_AX_THRESHOLD =  2.0f;
constexpr float SHAKE_AZ_THRESHOLD =  1.25f;

constexpr unsigned long COOLDOWN_MS = 400;

// Game
constexpr int WIN_SCORE = 10;
const char* commands[] = {"THROW", "DRIBBLE", "SPIN", "HEADER"};
//const char* commands[] = {"PUNCH", "SHAKE", "TWIST", "TILT"};
constexpr int NUM_COMMANDS = 4;

int score1 = 0;
int score2 = 0;
const char* currentCommand = nullptr;

// all the game states
enum GameState { WAITING, ACTIVE, GAME_OVER };
GameState state = WAITING;

unsigned long roundStartTime = 0;
unsigned long waitDuration   = 0;

static sensors_event_t accel1, gyro1, temp1;
static sensors_event_t accel2, gyro2, temp2;

static unsigned long lastDetect1 = 0;
static unsigned long lastDetect2 = 0;

// Shuffled bag (for better "random")
static const char* bag[NUM_COMMANDS];
static int bagIndex = 0;

static void refillBag()
{
  for (int i = 0; i < NUM_COMMANDS; i++) bag[i] = commands[i];
  for (int i = NUM_COMMANDS - 1; i > 0; i--) {
    int j = random(i + 1);
    const char* tmp = bag[i];
    bag[i] = bag[j];
    bag[j] = tmp;
  }
  bagIndex = 0;
}

static const char* nextCommand()
{
  if (bagIndex >= NUM_COMMANDS) refillBag();
  return bag[bagIndex++];
}

static const char* detectGesture(
  sensors_event_t& accel,
  sensors_event_t& gyro,
  unsigned long& lastDetect,
  unsigned long now)
{
  if (now - lastDetect < COOLDOWN_MS) return nullptr;

  float ax = accel.acceleration.x;
  float ay = accel.acceleration.y;
  float az = accel.acceleration.z;
  float gz = gyro.gyro.z;

// HEADER: flip upside down — az goes negative
  if (az < TILT_Z_THRESHOLD) {
    lastDetect = now;
    return "HEADER";
  }

  // SPIN: gz dominant
  if (fabsf(gz) > TWIST_GZ_THRESHOLD) {
    lastDetect = now;
    return "SPIN";
  }

  // THROW: spike on X axis
  if (fabsf(ax) > PUNCH_AX_THRESHOLD) {
    lastDetect = now;
    return "THROW";
  }

  // DRIBBLE: spike on Z axis above gravity baseline
  float azDeviation = fabsf(fabsf(az) - 9.8f);
  if (azDeviation > SHAKE_AZ_THRESHOLD) {
    lastDetect = now;
    return "DRIBBLE";
  }

  return nullptr;
}

static void renderGame()
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
      display.drawStr(0, 60, score1 >= WIN_SCORE ? "P1 WINS" : "P2 WINS");
    }

  } while (display.nextPage());
}

void setup()
{
  Wire.begin();
  imu1.begin_I2C(IMU_1_ADDR);
  imu2.begin_I2C(IMU_2_ADDR);

  display.begin();
  randomSeed(analogRead(0));

  refillBag();
  waitDuration = random(3000, 5000);
  roundStartTime = millis();
}

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
      if (now - roundStartTime >= waitDuration) {
        currentCommand = nextCommand();
        state = ACTIVE;
        // =========================
        // PLAY SOUND
        // =========================
      }
      break;

    // =========================
    // after score++ add to 7 seg for p1 or p2
    // =========================
    case ACTIVE:
      if (g1 && strcmp(g1, currentCommand) == 0) {
        score1++;
        state = WAITING;
      }
      else if (g2 && strcmp(g2, currentCommand) == 0) {
        score2++;
        state = WAITING;
      }

      if (score1 >= WIN_SCORE || score2 >= WIN_SCORE) {
        state = GAME_OVER;
      }

      if (state == WAITING) {
        waitDuration   = random(3000, 5000);
        roundStartTime = now;
        delay(500);
      }
      break;

    case GAME_OVER:
      break;
  }

  renderGame();
  delay(20);
}
