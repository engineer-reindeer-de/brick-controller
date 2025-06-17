#include "config.h"
#include "wifisetup.h"
#include "webserver.h"
#include "servo.h"
#include "logging.h"
#include "filesystem.h"
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
  initializeFilesystem();
  cfg.load();
  initPwmPinsFromMapping(cfg);
  logInfo("🏁 Servos initialisiert.");
  setupLogging();
  cfg.print();
  logInfo("⚙ Konfiguration geladen.");
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