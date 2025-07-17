#include "Wifi/WifiCredentials.h"
#include "Wifi/WifiConnection.h"
#include "Arduino.h"
#include "Logger.h"
#ifdef BOARD_ESP32
#include <WiFi.h>
#include <Preferences.h>
#endif

#ifdef BOARD_ESP01
#include <ESP8266WiFi.h>
#endif

WifiConnection::WifiConnection(Logger &logger): logger(logger){};

bool WifiConnection::connect(WifiCredentials wifiCredentials)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiCredentials.ssid, wifiCredentials.password);

    logger.log("Versuche Verbindung mit SSID: '%s'\n", wifiCredentials.ssid);
    logger.log("mit dem Passwort: '%s'\n", wifiCredentials.password);

    int8_t n = WiFi.scanNetworks();
    logger.log("Gefundene Netzwerke: %d\n", n);
    for (int i = 0; i < n; ++i)
    {
        logger.log("  - %s (%d dBm)%s\n",
                   WiFi.SSID(i).c_str(),
                   WiFi.RSSI(i),
                   WiFi.SSID(i) == String(wifiCredentials.ssid) ? "  <-- Ziel" : "");
    }

    unsigned long timeout = millis() + 20000;
    while (WiFi.status() != WL_CONNECTED && millis() < timeout)
    {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        logger.log("\n✅ WLAN verbunden!");
        int rssi = WiFi.RSSI();
        int signalStrength = map(rssi, -90, -30, 0, 100);
        logger.log("Signal: %d dBm| %d%\nIP: %s\n", rssi, signalStrength, WiFi.localIP().toString());
        return true;
    }

    return false;
};

String WifiConnection::getIp()
{
    return WiFi.softAPIP().toString();
};

void WifiConnection::startAP(WifiCredentials wifiCredentials)
{
    logger.log("⚠️ Starte stattdessen Access-Point...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(wifiCredentials.ssid, wifiCredentials.password);
    logger.log("AP IP-Adresse: ");
    logger.log("%s\n", WiFi.softAPIP().toString());
};