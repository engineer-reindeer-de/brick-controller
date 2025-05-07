#include <Arduino.h>

#ifdef BOARD_ESP32
#include <Preferences.h>
#include <esp_system.h>
#endif

#ifdef BOARD_ESP01
#include <EEPROM.h>
#endif

#include "config.h"
#include <cstring>

int Config::findControllerIndexById(const char* id) const {
  for (int i = 0; i < CONTROLLER_GPIO_COUNT; ++i) {
        if (strcmp(controllers[i].name, id) == 0)
            return i;
    }
    return -1;
}

void Config::print() const {
    logInfo("📦 Aktuelle Konfiguration:");

    logInfo("├ SSID: %s", ssid);
    logInfo("├ PASS: %s", password);
    logInfo("├ i2c_sda: %d", i2c.sda);
    logInfo("├ i2c_scl: %d", i2c.scl);
    logInfo(" controllerConfiguration: ");

    for (int i = 0; i < CONTROLLER_GPIO_COUNT; ++i) {
        const auto& c = controllers[i];
        logInfo("  ▶ [%d] %s → Port %d, Snapback: %s, Drift: %d",
                      i, c.name, c.pin, c.snapback ? "true" : "false", c.drift);
    }
}

Config::ControllerConfig* Config::getById(const char* id) {
    int idx = findControllerIndexById(id);
    return idx >= 0 ? &controllers[idx] : nullptr;
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