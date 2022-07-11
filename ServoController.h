#include <Servo.h>

#define SERVO_PIN 5
#define SERVO_MAX 180
#define SERVO_MIN 0
#define SERVO_DURATION 700
#define SERVO_SPEED 6
#define SERVO_SPEED_NIGHT 1

Servo servo;

int32_t servoPos = SERVO_MIN;
uint8_t servoSpeed = SERVO_SPEED;

bool showing = false;
bool hiding = false;

class ServoController {
  public:
    ServoController();

    void init() {
      servo.attach(SERVO_PIN, 480, 2400);
      servo.write(SERVO_MIN);
    }

    void tick() {
      uint8_t servoStartPos = servoPos;
      
      if (showing) {
        servoPos += servoSpeed;
      } else if (hiding) {
        servoPos -= servoSpeed;
      }

      if (servoPos >= SERVO_DURATION) {
        servoPos = SERVO_DURATION;
        showing = false;
      } else if (servoPos <= SERVO_MIN) {
        servoPos = SERVO_MIN;
        hiding = false;
      }

      if (servoStartPos != servoPos) {
        servo.write((cos(PI / SERVO_DURATION * servoPos - PI) + 1) / 2 * SERVO_MAX);
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

    void activateNightMode() {
      servoSpeed = SERVO_SPEED_NIGHT;
    }

    void disableNightMode() {
      servoSpeed = SERVO_SPEED;
    }
};


extern ServoController ServoController;
