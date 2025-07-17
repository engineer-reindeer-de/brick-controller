#include "Webserver.h"
#include <ESPAsyncWebServer.h>
#include "Controller/Websocket.h"
#include <ArduinoJson.h>
#include <algorithm> // für std::min
#include "Logger.h"
#include "Repository/SettingsRepository.h"
#include "Entity/Settings.h"
#include "Filesystem.h"

#ifdef BOARD_ESP32CAM
#include <AsyncTCP.h>
#include "Preferences.h"
#endif

#ifdef BOARD_ESP01
#include <WString.h>
#endif

#ifdef HAS_CAMERA
#include "Camera/Camera.h"
#include <esp_camera.h>

// Falls keine log_*‑Makros vorhanden sind, auf Serial umbiegen
#ifndef log_printf
#define log_printf Serial.printf
#endif
#ifndef log_i
#define log_i Serial.printf
#endif
#ifndef log_e
#define log_e Serial.printf
#endif
// Neuer Handler ohne Endlosschleife

#endif

AsyncWebSocket ws("/ws");

AsyncWebServer server(80);

Webserver::Webserver(
      SettingsRepository &settingsRepository,
      SettingsController &settingsController,
      CameraController &cameraController,
      DashboardController &dashboardController,
      Filesystem &filesystem,
      Websocket &websocket
      ) :
      settingsController(settingsController),
                                cameraController(cameraController),
                                dashboardController(dashboardController),
                                filesystem(filesystem),
                                websocket(websocket)
  {
    ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client,
                      AwsEventType type, void *arg, uint8_t *data, size_t len) {
      this->websocket.onWsEvent(server, client, type, arg, data, len);
    });
    server.addHandler(&ws);

    server.on(
        "/api/dashboard",
        HTTP_POST,
        [](AsyncWebServerRequest *req) {},
        NULL,
        [this](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total)
        {
          this->dashboardController.post(req, data, len, index, total);
        });

    server.on(
        "/api/dashboard",
        HTTP_GET,
        [this](AsyncWebServerRequest *req)
        {
          this->dashboardController.get(req);
        });

    server.on(
        "/api/settings",
        HTTP_POST,
        [](AsyncWebServerRequest *req) {},
        NULL,
        [this](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total)
        {
          this->settingsController.post(req, data, len, index, total);
        });

    server.on(
        "/api/settings",
        HTTP_GET,
        [this](AsyncWebServerRequest *req)
        {
          this->settingsController.get(req);
        });

    server.on(
        "/api/restart",
        HTTP_GET,
        [](AsyncWebServerRequest *req)
        {
          req->send(200, "text/plain", "Neustart...");
          delay(500);
          ESP.restart();
        });

#ifdef HAS_CAMERA
    server.on(
        "/stream",
        HTTP_GET,
        [this](AsyncWebServerRequest *req)
        {
          this->cameraController.streamJpg(req);
        });
#endif

    if (!this->filesystem.exists("/index.html"))
    {
      Serial.println("❌ Kein Dateisystem verfügbar.");
      return;
    }

    Serial.println("📁 LittleFS bereit.");
    server.serveStatic("/", this->filesystem.getFS(), "/")
        .setDefaultFile("index.html")
        .setFilter([](AsyncWebServerRequest *req)
                   {
              // React‑SPA nur ausliefern, wenn die URL NICHT mit "/api/" beginnt
              return !req->url().startsWith("/api/"); });

    server.begin();
  };
  void Webserver::cleanUp()
  {
    ws.cleanupClients();
  };