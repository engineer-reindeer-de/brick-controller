#include <stdint.h>
#include <cstring>
#include "Arduino.h"
#include "I2c.h"
#include "Wifi/WifiCredentials.h"
#include "Logger.h"
#include "LittleFS.h"
#include "Entity/Settings.h"
#include <Wire.h>

Settings::Settings(): i2c(I2c(-1, -1, Wire)), wifiCredentials() {};
    
char *Settings::widgetsAsJson() {
        static char buffer[4096];
        File f = LittleFS.open("/widgets.json", "r");
        if (!f) {
            snprintf(buffer, sizeof(buffer), "{\"widgets\":[]}");
            return buffer;
        }
        size_t len = f.readBytes(buffer, sizeof(buffer) - 1);
        buffer[len] = '\0';
        f.close();
        return buffer;
    }