#pragma once
#include <stdint.h>
#include <cstring>
#include "Arduino.h"
#include "I2c.h"
#include "Wifi/WifiCredentials.h"
#include "Logger.h"

class Settings
{
public:
    Settings();
    I2c i2c;
    WifiCredentials wifiCredentials;
    void print(Logger logger);
    char *widgetsAsJson();
};