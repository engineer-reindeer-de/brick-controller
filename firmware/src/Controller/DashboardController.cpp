#include "controller/SettingsController.h"
#include <ArduinoJson.h>
#include "Repository/DashboardRepository.h"

class DashboardController
{
private:
  DashboardRepository &dashboardRepository;

public:
  DashboardController(DashboardRepository &dashboardRepository) : dashboardRepository(dashboardRepository){}

  void get(AsyncWebServerRequest *req) {
    String dashboardJson = this->dashboardRepository.load();
    req->send(200, "application/json", dashboardJson);
  };

  void post(AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total)
  {
    String body = String((const char*)data).substring(0, len);
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, body);
    if (err) {
      req->send(400, "text/plain", err.c_str());
      return;
    }

    if (!doc["widgets"].is<JsonArray>()) {
      req->send(400, "text/plain", "widgets[] fehlt oder ist kein Array");
      return;
    }
    //JsonArray widgets = doc["widgets"].as<JsonArray>();

    // Persist the validated JSON via the repository
    if (!this->dashboardRepository.save(body)) {
      req->send(500, "text/plain", "Fehler beim Speichern der Datei");
      return;
    }

    req->send(200, "application/json", "{\"status\":\"ok\"}");
  };
};