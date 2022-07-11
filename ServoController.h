#include <Servo.h>

#define SERVO_PIN 5
#define SERVO_MAX 180
#define SERVO_MIN 0

Servo servo;

int16_t servoPos = SERVO_MIN;
uint8_t servoSpeed = 2;

bool showing = false;
bool hiding = false;

uint8_t servoPosMap[SERVO_MAX];

class ServoController {
  public:
    ServoController();

    void init() {
      servo.attach(SERVO_PIN, 480, 2400);
      servo.write(SERVO_MIN);
      setupServoPosMap();
    }

    void tick() {
      uint8_t servoStartPos = servoPos;
      
      if (showing) {
        servoPos += servoSpeed;
      } else if (hiding) {
        servoPos -= servoSpeed;
      }

      if (servoPos >= SERVO_MAX) {
        servoPos = SERVO_MAX;
        showing = false;
      } else if (servoPos <= SERVO_MIN) {
        servoPos = SERVO_MIN;
        hiding = false;
      }

      if (servoStartPos != servoPos) {
        servo.write(servoPosMap[servoPos - 1]);
      }
    }

    void showHeart() {
      hiding = false;
      showing = true;
    }

    void hideHeart() {
      showing = false;
      hiding = true;
    }

  private:
    void setupServoPosMap() {
      for (uint8_t i = 0; i < SERVO_MAX; i++) {
        uint8_t value = (cos(PI / SERVO_MAX * i - PI) + 1) / 2 * SERVO_MAX;
        servoPosMap[i] = value;
      }
    }
};


extern ServoController ServoController;
