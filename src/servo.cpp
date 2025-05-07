#include "servo.h"

void writeServo(uint8_t pin, int angle)
{
#ifdef LED_PWM_RESOLUTION
  angle = constrain(angle, 0, 180);
  uint32_t pulseWidth = map(angle, 0, 180, 1000, 2000); // 1–2ms
  uint32_t duty = (pulseWidth * 1023) / 20000;          // 10-bit duty cycle
  ledcWrite(pin, duty);

  logInfo("LEDC Servo → pin: %d, angle: %d°, duty: %lu\n", pin, angle, duty);
#endif
}

int mapJS(int val)
{
  return constrain(map(val, 0, 100, 0, 180), 0, 180);
}

void initServo(uint8_t pin)
{

#ifdef LED_PWM_RESOLUTION
  int channel = pin;
  ledcSetup(channel, 50, LED_PWM_RESOLUTION); // 50 Hz, 10 Bit Auflösung
  ledcAttachPin(pin, channel);
  logInfo("LEDC Servo init → pin: %d, channel: %d\n", pin, channel);
#endif
}
