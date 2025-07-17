#include "Arduino.h"
#include "Filesystem.h"
#include "Repository/SettingsRepository.h"
#include "Webserver.h"
#include "Logger.h"
#include "Wifi/WifiConnection.h"
#ifdef HAS_CAMERA
#include "Camera/Camera.h"
#endif

#ifdef ESP32
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#endif

class App {
    public:
        App()
        {
    #ifdef ESP32
            WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    #endif
            Serial.begin(115200);
            Serial.println("🚀 START");
            Logger logger;
            Filesystem filesystem(logger);
            SettingsRepository settingsRepositoy(logger, filesystem);
            Settings settings = settingsRepositoy.load();
            WifiConnection wifi(logger); 

            if(!wifi.connect(settings.wifiCredentials)){
                /*
                todo:
                WifiCredentials wifiCredentials("BrickControllerAP","12345678");
                wifi.startAP(wifiCredentials);
                */
            }
    #ifdef HAS_CAMERA
            Camera camera();
    #endif
            Webserver webserver();
            delay(1000);
        }
};