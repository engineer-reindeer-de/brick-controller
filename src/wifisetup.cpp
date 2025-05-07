#include "config.h"
#include "logging.h"

extern Config cfg;

#ifdef BOARD_ESP32
#include <WiFi.h>
#include <Preferences.h>
#endif

#ifdef BOARD_ESP01
#include <ESP8266WiFi.h>
#endif

const char *AP_SSID = "ESP32CamAP";
const char *AP_PASS = "12345678";

void startWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(cfg.ssid, cfg.password);
#ifdef BUILD_IN_LED_PIN
  if (BUILD_IN_LED_PIN >= 0)
  {
    pinMode(BUILD_IN_LED_PIN, OUTPUT);
  }
#endif

  logInfo("Versuche Verbindung mit SSID: '%s'\n", cfg.ssid);
  logInfo("mit dem Passwort: '%s'\n", cfg.password);

  int8_t n = WiFi.scanNetworks();
  logInfo("Gefundene Netzwerke: %d\n", n);
  for (int i = 0; i < n; ++i)
  {
    logInfo("  - %s (%d dBm)%s\n",
            WiFi.SSID(i).c_str(),
            WiFi.RSSI(i),
            WiFi.SSID(i) == cfg.ssid ? "  <-- Ziel" : "");
  }

  unsigned long timeout = millis() + 20000;
  while (WiFi.status() != WL_CONNECTED && millis() < timeout)
  {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    logInfo("\n✅ WLAN verbunden!");
    int rssi = WiFi.RSSI();                       // z. B. -30 (gut) bis -90 (schlecht)
    int brightness = map(rssi, -90, -30, 0, 255); // in 0–255 mappen
    brightness = constrain(brightness, 0, 255);
#ifdef BUILD_IN_LED_PIN
    if (BUILD_IN_LED_PIN >= 0)
    {
      analogWrite(BUILD_IN_LED_PIN, brightness); // für ESP8266/ESP32
    }
#endif

    int signalStrength = map(brightness, 0, 255, 0, 100);
    logInfo("RSSI: %d dBm,\n Signal: %d%\nIP: %s\n", rssi, signalStrength, WiFi.localIP().toString());
  }
  else
  {
    logInfo("⚠️ Starte stattdessen Access-Point...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASS);
#ifdef BUILD_IN_LED_PIN
    if (BUILD_IN_LED_PIN >= 0)
    {
      analogWrite(BUILD_IN_LED_PIN, 50);
    }
#endif
    logInfo("AP IP-Adresse: ");
    logInfo("%s\n", WiFi.softAPIP().toString());
  }

#ifdef ESP32
  xTaskCreate(
      [](void *parameter)
      {
        while (true)
        {
          int brightness = 0;

          if (WiFi.getMode() == WIFI_AP)
          {
            int rssi = WiFi.RSSI();
            brightness = map(rssi, -90, -30, 0, 255);
            brightness = constrain(brightness, 0, 255);
            int signalStrength = map(brightness, 0, 255, 0, 100);
            logInfo(" AP-Modus aktiv\n RSSI: %d dBm\n Signal: %d%\n IP: %s\n", rssi, signalStrength, WiFi.localIP().toString());
          }
          else if (WiFi.status() == WL_CONNECTED)
          {
            int rssi = WiFi.RSSI();
            brightness = map(rssi, -90, -30, 0, 255);
            brightness = constrain(brightness, 0, 255);
            int signalStrength = map(brightness, 0, 255, 0, 100);
            logInfo(" STA-Modus aktiv\n RSSI: %d dBm\n Signal: %d%\n IP: %s\n", rssi, signalStrength, WiFi.localIP().toString());
          }
          else
          {
            brightness = 10;

            int signalStrength = map(brightness, 0, 255, 0, 100);
            logInfo("WLAN nicht verbunden");
          }
#ifdef BUILD_IN_LED_PIN
          if (BUILD_IN_LED_PIN >= 0)
          {
            analogWrite(BUILD_IN_LED_PIN, brightness);
          }
#endif
          vTaskDelay(pdMS_TO_TICKS(2000));
        }
      },
      "WifiSignalLEDTask",
      2048,
      NULL,
      1,
      NULL);
#endif
}

String getIp()
{
  return WiFi.softAPIP().toString();
}