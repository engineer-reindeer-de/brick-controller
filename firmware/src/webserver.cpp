
#include "webserver.h"
extern Config cfg;

void initPwmPinsFromMapping(const Config& cfg);

#ifdef BOARD_ESP32CAM
#include <AsyncTCP.h>
#include "Preferences.h"
#endif

#ifdef BOARD_ESP01
#include <WString.h>
#endif

#ifdef HAS_CAMERA
#include "camera.h"
#include <esp_camera.h>
#endif

AsyncWebSocket ws("/ws");


AsyncWebServer server(80);


#ifdef HAS_CAMERA
#include "esp_camera.h"
// -------- Asynchrones MJPEG‑Streaming ohne Watchdog‑Probleme --------
#define PART_BOUNDARY "123456789000000000000987654321"
static const char* STREAM_CONTENT_TYPE = "multipart/x-mixed-replace; boundary=" PART_BOUNDARY;
static const char* STREAM_BOUNDARY     = "\r\n--" PART_BOUNDARY "\r\n";
static const char* STREAM_PART         = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

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

class AsyncJpegStreamResponse : public AsyncAbstractResponse {
  private:
    camera_fb_t * _fb   = nullptr;
    size_t        _idx  = 0;
    size_t        _len  = 0;
    uint64_t      _last = 0;

  public:
    AsyncJpegStreamResponse() {
      _code             = 200;
      _contentType      = STREAM_CONTENT_TYPE;
      _sendContentLength = false;
      _chunked          = true;
    }

    ~AsyncJpegStreamResponse() override {
      if (_fb) esp_camera_fb_return(_fb);
    }

    bool _sourceValid() const override { return true; }

    size_t _fillBuffer(uint8_t *buf, size_t maxLen) override {
      size_t written = 0;

      // Neues Frame holen, wenn keines aktiv ist
      if (!_fb) {
        if (maxLen < strlen(STREAM_BOUNDARY) + 64) return RESPONSE_TRY_AGAIN;

        _fb = esp_camera_fb_get();
        if (!_fb) return RESPONSE_TRY_AGAIN;

        _idx = 0;
        _len = _fb->len;

        // Boundary + Header ausgeben
        written += snprintf((char*)(buf + written),    // schreibe direkt in Puffer
                            maxLen - written,          // verbleibende Größe
                            "%s"
                            "Content-Type: image/jpeg\r\n"
                            "Content-Length: %u\r\n\r\n",
                            STREAM_BOUNDARY, _len);
      }

      // Bilddaten kopieren
      size_t toCopy = std::min(maxLen - written, _len - _idx);
      memcpy(buf + written, _fb->buf + _idx, toCopy);
      written += toCopy;
      _idx    += toCopy;

      // Frame komplett? → zurückgeben und Stats ausgeben
      if (_idx == _len) {
        size_t thisLen = _len;
        esp_camera_fb_return(_fb);
        _fb  = nullptr;
        _idx = _len = 0;
      }

      return written;
    }
};

// Neuer Handler ohne Endlosschleife
void streamJPG(AsyncWebServerRequest *request) {
  auto *response = new AsyncJpegStreamResponse();
  if (!response) {
    request->send(503, "text/plain", "Speicherfehler");
    return;
  }
  response->addHeader("Access-Control-Allow-Origin", "*");
  request->send(response);
}
#endif // HAS_CAMERA

void setupServer()
{

  /* WebSocket ----------------------------------------------------------- */
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // API: /api/sensors (Sensorquellen für Dropdown im Frontend)
  server.on("/api/sensors", HTTP_GET, [](AsyncWebServerRequest *req) {
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    // Beispielhafte Signalpfade – später dynamisch erweiterbar
    arr.add("wifi.rssi");
    arr.add("battery.voltage");
    arr.add("environment.temperature");
    arr.add("environment.humidity");

    String json;
    serializeJson(doc, json);
    req->send(200, "application/json", json);
  });

  server.on("/api/widgets", HTTP_POST, [](AsyncWebServerRequest *req) {}, NULL,
  [](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total) {
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

    File f = LittleFS.open("/widgets.json", "w");
    if (!f) {
      req->send(500, "text/plain", "Fehler beim Öffnen der Datei");
      return;
    }
    serializeJson(doc, f);
    f.close();
    cfg.save(); // optional: z. B. WiFi mit sichern
    req->send(200, "application/json", "{\"status\":\"ok\"}");
  });

  server.on("/api/widgets", HTTP_GET, [](AsyncWebServerRequest *req) {
    req->send(200, "application/json", cfg.widgetsAsJson());
  });

  server.on("/api/wifi", HTTP_POST, [](AsyncWebServerRequest *req) {}, NULL,
    [](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total) {
      String body = String((const char*)data).substring(0, len);
      JsonDocument doc;
      DeserializationError err = deserializeJson(doc, body);
      if (err) {
        req->send(400, "text/plain", err.c_str());
        return;
      }
      cfg.load();
      const char* ssid = doc["ssid"] | "";
      const char* password = doc["password"] | "";

      strncpy(cfg.ssid, ssid, sizeof(cfg.ssid));
      strncpy(cfg.password, password, sizeof(cfg.password));

      cfg.save();
      cfg.print();

      startWiFi();

      req->send(200, "application/json", "{\"status\":\"ok\"}");
    });

  // --- I2C-Konfiguration ---
  server.on("/api/i2c", HTTP_GET, [](AsyncWebServerRequest *req) {
    JsonDocument doc;
    doc["sda"] = cfg.i2c.sda;
    doc["scl"] = cfg.i2c.scl;
    String json;
    serializeJson(doc, json);
    req->send(200, "application/json", json);
  });

  server.on("/api/i2c", HTTP_POST, [](AsyncWebServerRequest *req) {}, NULL,
    [](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total) {
      String body = String((const char*)data).substring(0, len);
      JsonDocument doc;
      DeserializationError err = deserializeJson(doc, body);
      if (err) {
        req->send(400, "text/plain", err.c_str());
        return;
      }
      cfg.load();
      cfg.i2c.sda = doc["sda"] | -1;
      cfg.i2c.scl = doc["scl"] | -1;
      cfg.save();
      cfg.print();
      req->send(200, "application/json", "{\"status\":\"ok\"}");
    });

  // Ergänzung: /api/restart
  server.on("/api/restart", HTTP_GET, [](AsyncWebServerRequest *req)
            {
      req->send(200, "text/plain", "Neustart...");
      delay(500);
      ESP.restart(); });

  // Neuer API-Endpunkt: /api/pwm_channels
server.on("/api/pins", HTTP_GET, [](AsyncWebServerRequest *req) {
  JsonDocument doc;
  JsonArray arr = doc.to<JsonArray>();

  for (int i = 0; i < CONTROLLER_GPIO_COUNT; ++i) {
    int pin = allowedControllerPins[i];
    JsonObject o = arr.add<JsonObject>();
    o["pin"] = pin;
    String label = "GPIO " + String(pin);
    o["label"] = label;
  }

  String json;
  serializeJson(doc, json);
  req->send(200, "application/json", json);
});

#ifdef HAS_CAMERA
  server.on("/stream", HTTP_GET, streamJPG);
#endif
  if(LittleFS.begin()) {
    Serial.println("📁 LittleFS bereit.");
    server.serveStatic("/", LittleFS, "/")
          .setDefaultFile("index.html")
          .setFilter([](AsyncWebServerRequest *req){
              // React‑SPA nur ausliefern, wenn die URL NICHT mit "/api/" beginnt
              return !req->url().startsWith("/api/");
          });
  } else {
    Serial.println("❌ Kein Dateisystem verfügbar.");
  }
  server.begin();
}

void cleanUpClients()
{
  ws.cleanupClients();
  initPwmPinsFromMapping(cfg);
}