#include "filesystem.h"

void initializeFilesystem(){
    if (!LittleFS.begin())
    {
      logInfo("❌ LittleFS konnte nicht gestartet werden – versuche Format …");
      if (LittleFS.format() && LittleFS.begin())
      {
        logInfo("✅ LittleFS formatiert und gemountet.");
      }
      else
      {
        logInfo("🛑 LittleFS endgültig fehlgeschlagen – stop.");
        esp_restart();
      }
    }
    else
    {
        logInfo("📁 LittleFS bereit.");
        for (int i = 0; i < 10; ++i) {
            delay(10);
            logInfo("⏳ LittleFS check %d", i);
        }
    }
}