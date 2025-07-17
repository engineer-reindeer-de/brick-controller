#pragma once
#include "Arduino.h"
#include "Filesystem.h"

class DashboardRepository
{
public:
    explicit DashboardRepository(Filesystem &filesystem);
    String load();
    bool save(const String &json);

private:
    Filesystem &filesystem;
};