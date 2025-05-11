#include "config.h"
#include "wifisetup.h"
#include "webserver.h"
#include "servo.h"
#include <LittleFS.h>
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
  if (!LittleFS.begin())
  {
    Serial.println("❌ LittleFS konnte nicht gestartet werden – versuche Format …");
    if (LittleFS.format() && LittleFS.begin())
    {
      Serial.println("✅ LittleFS formatiert und gemountet.");
    }
    else
    {
      Serial.println("🛑 LittleFS endgültig fehlgeschlagen – stop.");
      esp_restart();
    }
  }
  else
  {
    Serial.println("📁 LittleFS bereit.");
  }

  cfg.load();
  initPwmPinsFromMapping(cfg);
  cfg.i2c.sda = 14;
  cfg.i2c.scl = 15;
  strncpy(cfg.ssid, "WLAN-SCHWARZ-2.4G", sizeof(cfg.ssid));
  strncpy(cfg.password, "Willkommen bei Erika und Rudolf", sizeof(cfg.password));
  cfg.save();
  cfg.load();
  setupLogging();
  cfg.print();

  logInfo("⚙ Konfiguration geladen.");
  for (int i = 0; i < CONTROLLER_GPIO_COUNT; ++i)
  {
    uint8_t pin = allowedControllerPins[i];
    if (pin == 255 || pin > 16)
    {
      continue;
    }

    if (pin == cfg.i2c.sda || pin == cfg.i2c.scl)
    {
      continue;
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