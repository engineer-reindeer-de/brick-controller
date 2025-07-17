#include "Controller/SettingsController.h"
#include <ArduinoJson.h>
#include "Repository/SettingsRepository.h"
#include "Entity/Settings.h"

SettingsController::SettingsController(SettingsRepository settingsRepository, WifiConnection wifiConnection) : settingsRepository(settingsRepository),
                                                                                                               wifiConnection(wifiConnection) {};
void SettingsController::get(AsyncWebServerRequest *req)
{
  Settings settings = this->settingsRepository.load();
  JsonDocument doc;
  doc["sda"] = settings.i2c.sda;
  doc["scl"] = settings.i2c.scl;
  String json;
  serializeJson(doc, json);
  req->send(200, "application/json", json);
};

void SettingsController::post(AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total)
{
  String body = String((const char *)data).substring(0, len);
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, body);
  if (err)
  {
    req->send(400, "text/plain", err.c_str());
    return;
  }
  Settings settings = this->settingsRepository.load();
  const char *ssid = doc["ssid"] | "";
  const char *password = doc["password"] | "";
  settings.i2c.sda = doc["sda"] | -1;
  settings.i2c.scl = doc["scl"] | -1;

  strncpy(settings.wifiCredentials.ssid, ssid, sizeof(settings.wifiCredentials.ssid));
  strncpy(settings.wifiCredentials.password, password, sizeof(settings.wifiCredentials.password));

  this->settingsRepository.save(settings);

  if (err)
  {
    req->send(400, "text/plain", err.c_str());
    return;
  }

  req->send(200, "application/json", "{\"status\":\"ok\"}");

  this->wifiConnection.connect(settings.wifiCredentials);
};