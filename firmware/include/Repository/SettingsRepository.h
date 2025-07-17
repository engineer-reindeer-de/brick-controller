#pragma once
#include "Filesystem.h"
#include "Entity/Settings.h"
#include "Logger.h"

class SettingsRepository {
private:
    Logger logger;
    Filesystem filesystem;
public:
    SettingsRepository(Logger logger, Filesystem filesystem);
    void save(Settings settings);
    Settings load();
    bool reset();
};
