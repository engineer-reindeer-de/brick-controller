#include <Arduino.h>
#include <ArduinoJson.h>
#ifdef BOARD_ESP32
#include <Preferences.h>
#include <esp_system.h>
#endif

#ifdef BOARD_ESP01
#include <EEPROM.h>
#endif

#include <cstring>
#include "Filesystem.h"
#include "Repository/SettingsRepository.h"


SettingsRepository::SettingsRepository(Logger &logger, Filesystem &filesystem)
    : logger(logger), filesystem(filesystem) {}

void SettingsRepository::save(Settings settings)
{
    logger.log("💾 Speichere Konfiguration");
    fs::FS &fs = filesystem.getFS();
    File file = fs.open("/settings.bin", "w");
    if (!file)
    {
        logger.log("❌ Konnte /settings.bin nicht öffnen (write)");
        return;
    }
    file.write(reinterpret_cast<const uint8_t *>(&settings), sizeof(Settings));
    file.close();
    logger.log("✅ Konfiguration gespeichert.");
}

Settings SettingsRepository::load()
{
    logger.log("🎚️ Lade Konfiguration");
    Settings settings; // default

    fs::FS &fs = filesystem.getFS();
    if (!fs.exists("/settings.bin"))
    {
        logger.log("⚠️ /settings.bin nicht gefunden – verwende Standardwerte");
        return settings;
    }

    File file = fs.open("/settings.bin", "r");
    if (!file)
    {
        logger.log("❌ Konnte /settings.bin nicht öffnen (read)");
        return settings;
    }

    if (file.size() == sizeof(Settings))
    {
        file.readBytes(reinterpret_cast<char *>(&settings), sizeof(Settings));
        logger.log("✅ Konfiguration geladen.");
    }
    else
    {
        logger.log("⚠️ Größe von /settings.bin passt nicht – verwende Standardwerte");
    }
    file.close();
    return settings;
}

bool SettingsRepository::reset()
{
    logger.log("🗑️ Lösche Konfiguration");
    fs::FS &fs = filesystem.getFS();
    if (fs.exists("/settings.bin"))
    {
        fs.remove("/settings.bin");
        logger.log("✅ /settings.bin gelöscht");
    }
    return true;
}
