#include "logging.h"
#include <stdint.h>
#include "servo.h"
#include "config.h"
#include <string.h>

static int8_t esp32PwmChannels[CONTROLLER_GPIO_COUNT];   // GPIO → channel; -1 = unused

void writeServo(uint8_t pin, int angle)
{
  if (pin >= CONTROLLER_GPIO_COUNT) {
    logInfo("⚠️ Ungültiger Pin: %d", pin);
    return;
  }
  int8_t channel = esp32PwmChannels[pin];
  if (channel < 0) {
    logInfo("⚠️ Kein LEDC‑Kanal für Pin %d", pin);
    return;
  }
#if defined(LED_PWM_RESOLUTION)
  angle = constrain(angle, 0, 180);
  uint32_t pulseWidth = map(angle, 0, 180, 1000, 2000);      // 1–2 ms
  uint32_t duty = (pulseWidth * ((1 << LED_PWM_RESOLUTION) - 1)) / 20000;
  ledcWrite(channel, duty);                                  // legacy API
  logInfo("LEDC Servo → pin:%d ch:%d angle:%d° duty:%lu", pin, channel, angle, duty);
#endif
}

int mapJS(int val)
{
  return constrain(map(val, 0, 100, 0, 180), 0, 180);
}

void initPwmPinsFromMapping(const Config &cfg)
{
  memset(esp32PwmChannels, -1, sizeof(esp32PwmChannels));
  uint8_t channel = 0;

  for (int i = 0; i < CONTROLLER_GPIO_COUNT && channel < 16; ++i)
  {
    uint8_t pin = allowedControllerPins[i];

    // Skip invalid markers, non‑GPIOs and the I²C pins
    if (pin == 255 || pin > 39 || pin == cfg.i2c.scl || pin == cfg.i2c.sda)
      continue;

#if defined(LED_PWM_RESOLUTION)
    ledcSetup(channel, 50, LED_PWM_RESOLUTION);   // 50 Hz servo PWM
    ledcAttachPin(pin, channel);
    delay(2000);
    logInfo("LEDC Servo init → pin:%d → channel:%d OK", pin, channel);

    break;
    //esp32PwmChannels[channel] = pin;
    
    //++channel;
#else
    logInfo("⚠️ LED_PWM_RESOLUTION nicht definiert – PWM nicht initialisiert");
#endif
  }
}