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

#include <cstring>
#include "Repository/SettingsRepository.h"


SettingsRepository::SettingsRepository(Logger logger, Filesystem filesystem) : logger(logger), filesystem(filesystem) {};

  void SettingsRepository::save(Settings settings)
  {
#ifdef BOARD_ESP32
    Preferences prefs;
    prefs.begin("settings", false);
    prefs.putBytes("settings", this, sizeof(Settings));
    prefs.end();
#endif

#ifdef BOARD_ESP01
    EEPROM.begin(sizeof(Config));
    EEPROM.put(0, *this);
    EEPROM.commit();
    EEPROM.end();
#endif
  };

  Settings SettingsRepository::load()
  {
    this->logger.log("🎚️ Lade Konfiguration");
    Settings settings;   // uses the new default constructor

#ifdef BOARD_ESP32
    // implemetiere das Persistieren der settings mit dem Filesystem!
#endif

#ifdef BOARD_ESP01
    EEPROM.begin(sizeof(Config));
    EEPROM.get(0, *this);
    EEPROM.end();
#endif

    logger.log("⚙ Konfiguration geladen.");

    return settings;
  };

  bool SettingsRepository::reset()
  {
#ifdef BOARD_ESP01
    EEPROM.begin(sizeof(Config));
    for (unsigned int i = 0; i < sizeof(Config); i++)
    {
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
    return true;
  };
