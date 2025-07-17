#pragma once
#include "Wifi/WifiCredentials.h"
#include "Logger.h"
#ifdef BOARD_ESP32
#include <WiFi.h>
#include <Preferences.h>
#endif

#ifdef BOARD_ESP01
#include <ESP8266WiFi.h>
#endif

class WifiConnection
{
private:
    Logger &logger;

public:
    WifiConnection(Logger &logger);
    bool connect(WifiCredentials wifiCredentials);
    String getIp();
    void startAP(WifiCredentials wifiCredentials);
};