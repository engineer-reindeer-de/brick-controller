#include "Filesystem.h"
#include "Arduino.h"
#include "LittleFS.h"

Filesystem::Filesystem(Logger &logger)
{
    if (!LittleFS.begin())
    {
        logger.log("❌ LittleFS konnte nicht gestartet werden – versuche Format …");
        if (LittleFS.format() && LittleFS.begin())
        {
            logger.log("✅ LittleFS formatiert und gemountet.");
        }
        else
        {
            logger.log("🛑 LittleFS endgültig fehlgeschlagen – stop.");
            esp_restart();
        }
    }
    else
    {
        logger.log("📁 LittleFS bereit.");
        for (int i = 0; i < 10; ++i)
        {
            delay(10);
            logger.log("⏳ LittleFS check %d", i);
        }
    }
}

bool Filesystem::exists(const char* path)
{
    return LittleFS.exists(path);
}

bool Filesystem::readFile(const char* path, String& out)
{
    if (!LittleFS.exists(path))
    {
        return false;
    }
    File f = LittleFS.open(path, "r");
    if (!f)
    {
        return false;
    }
    out = f.readString();
    f.close();
    return true;
}

bool Filesystem::writeFile(const char* path, const String& content)
{
    File f = LittleFS.open(path, "w");
    if (!f)
    {
        return false;
    }
    f.print(content);
    f.close();
    return true;
}

fs::FS& Filesystem::getFS()
{
    return LittleFS;
}