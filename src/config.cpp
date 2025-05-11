#include <LittleFS.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#ifdef BOARD_ESP32
#include <Preferences.h>
#include <esp_system.h>
#endif

#ifdef BOARD_ESP01
#include <EEPROM.h>
#endif

#include "config.h"
#include <cstring>

void Config::print() const {
    logInfo("📦 Aktuelle Konfiguration:");
    logInfo("├ SSID: %s", ssid);
    logInfo("├ PASS: %s", password);
    logInfo("├ i2c_sda: %d", i2c.sda);
    logInfo("├ i2c_scl: %d", i2c.scl);
}

char* Config::widgetsAsJson() {
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

void Config::load() {
  #ifdef BOARD_ESP32
    Preferences prefs;
    prefs.begin("cfg", true);
    prefs.getBytes("data", this, sizeof(Config));
    prefs.end();
  #endif

  #ifdef BOARD_ESP01
    EEPROM.begin(sizeof(Config));
    EEPROM.get(0, *this);
    EEPROM.end();
  #endif
}

void Config::save() {
#ifdef BOARD_ESP32
  Preferences prefs;
  prefs.begin("cfg", false);
  prefs.putBytes("data", this, sizeof(Config));
  prefs.end();
#endif

#ifdef BOARD_ESP01
  EEPROM.begin(sizeof(Config));
  EEPROM.put(0, *this);
  EEPROM.commit();
  EEPROM.end();
#endif
}

void Config::reset() {
#ifdef BOARD_ESP01
  EEPROM.begin(sizeof(Config));
  for (unsigned int i = 0; i < sizeof(Config); i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  EEPROM.end();
#endif

#ifdef BOARD_ESP32CAM
  Preferences prefs;
  prefs.begin("cfg", false);
  prefs.clear();
  prefs.end();
  ESP.restart();
#endif
}