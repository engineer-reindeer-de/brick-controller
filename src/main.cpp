#include <cstring>
#include "config.h"
#include "wifisetup.h"
#include "webserver.h"
#include "servo.h"
#include "logging.h"
#ifdef HAS_CAMERA
#include "camera.h"
#endif


#ifdef ESP32
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#endif
Config cfg;

/***************** setup & loop *****************************/
void setup()
{
  #ifdef ESP32
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  #endif
  Serial.begin(115200);

  Serial.println("🚀 START");
  cfg.load();
  cfg.i2c.sda = 14;
  cfg.i2c.scl = 15;
  cfg.save();  
  cfg.load();
  setupLogging();
  cfg.print();

  logInfo("⚙ Konfiguration geladen.");
  for (int i = 0; i < CONTROLLER_GPIO_COUNT; ++i)
  {
    uint8_t pin = cfg.controllers[i].pin;
    if (pin > -1 && pin < 16)
    {
      initServo(pin);
    }
  }

  logInfo("🏁 Servos initialisiert.");
  startWiFi();
  logInfo("📶 Wifi initialisiert.");
  #ifdef HAS_CAMERA
    initCamera();
  #endif


  setupServer();
  logInfo("✅ Webserver initialisiert.");
  delay(1000);
}
void loop()
{
}