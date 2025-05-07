#include "config.h"

extern Config cfg;

#include <ESPAsyncWebServer.h>
#include "websocket.h"
#include "index_html.h"
#include <ArduinoJson.h>
#include <algorithm>   // für std::min

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

  /* Root ---------------------------------------------------------------- */
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *req)
            { req->send(200, "text/html", index_html, index_html_len); });

  /* WebSocket ----------------------------------------------------------- */
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  /* POST  /save_config ---------------------------------------------------- */
  server.on("/save_config", HTTP_POST,
            [](AsyncWebServerRequest *req)
            {
              if (!req->hasParam("plain", true))
              {
                req->send(400, "text/plain", "Kein JSON-Body");
                return;
              }

              const AsyncWebParameter *p = req->getParam("plain", true); // 'true' = POST body

              JsonDocument doc;
              DeserializationError err = deserializeJson(doc, p->value());
              if (err)
              {
                req->send(400, "text/plain", err.c_str());
                return;
              }
              const char *ssid = doc["ssid"] | nullptr;
              if (ssid && *ssid)
                strncpy(cfg.ssid, ssid, sizeof(cfg.ssid));

              const char *password = doc["password"] | nullptr;
              if (password && *password)
                strncpy(cfg.password, password, sizeof(cfg.password));

              cfg.i2c.sda = doc["i2c_sda"] | cfg.i2c.sda;
              cfg.i2c.scl = doc["i2c_scl"] | cfg.i2c.scl;

              cfg.save();
              cfg.print();

              String out;
              serializeJson(doc, out);
              req->send(200, "application/json", out);
            });

  /* GET /get_config ----------------------------------------------------- */
  server.on("/get_config", HTTP_GET, [](AsyncWebServerRequest *req)
            {
      JsonDocument doc;
      // WLAN-Daten
      JsonObject wifi = doc["wifi"].to<JsonObject>();
      wifi["ssid"] = cfg.ssid;
      wifi["password"] = cfg.password; // Passwort nie ausgeben

      // Controller-Daten
      JsonObject controllers = doc["controllers"].to<JsonObject>();
      const char* ids[8] = {
        "joystick1_x", "joystick1_y", "joystick2_x", "joystick2_y",
        "slider1", "slider2", "slider3", "slider4"
      };
      for (int i = 0; i < CONTROLLER_GPIO_COUNT && i < 8; ++i) {
        const auto& c = cfg.controllers[i];
        JsonObject ctrl = controllers[ids[i]].to<JsonObject>();
        ctrl["name"] = (c.name && c.name[0]) ? c.name : ids[i];
        ctrl["port"] = c.pin;
        ctrl["snapback"] = c.snapback;
        ctrl["drift"] = c.drift;
      }
      JsonArray allowed = doc["allowed_gpio_ports"].to<JsonArray>();
      for (int i = 0; i < CONTROLLER_GPIO_COUNT; ++i) {
          allowed.add(allowedControllerPins[i]);
      }
      doc["i2c_sda"] = cfg.i2c.sda;
      doc["i2c_scl"] = cfg.i2c.scl;

      String out;
      serializeJson(doc, out);
      req->send(200, "application/json", out); });

  // Neuer Endpoint: /save_json (ersetzt)
  server.on("/save_json", HTTP_POST, [](AsyncWebServerRequest *req) {}, NULL, [](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total)
            {
        String body = String((const char*)data).substring(0, len);
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, body);
        if (err) {
          req->send(400, "text/plain", err.c_str());
          return;
        }

        // WLAN-Daten
        const char* ssid = doc["wifi"]["ssid"] | "";
        const char* password = doc["wifi"]["password"] | "";
        strncpy(cfg.ssid, ssid, sizeof(cfg.ssid));
        strncpy(cfg.password, password, sizeof(cfg.password));

        // Controller-Daten
        JsonObject controllers = doc["controllers"];
        int i = 0;
        for (JsonPair kv : controllers) {
          if (i >= CONTROLLER_GPIO_COUNT) break;
          JsonObject ctrl = kv.value().as<JsonObject>();
          strncpy(cfg.controllers[i].name, ctrl["name"] | "", sizeof(cfg.controllers[i].name));
          cfg.controllers[i].pin = ctrl["port"] | 0;
          cfg.controllers[i].snapback = ctrl["snapback"] | false;
          cfg.controllers[i].drift = ctrl["drift"] | 0;
          i++;
        }

        cfg.i2c.sda = doc["i2c_sda"] | cfg.i2c.sda;
        cfg.i2c.scl = doc["i2c_scl"] | cfg.i2c.scl;

        cfg.save();
        cfg.print();
        req->send(200, "text/plain", "OK");
        delay(1000);
        ESP.restart(); });

  // Ergänzung: /restart
  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest *req)
            {
      req->send(200, "text/plain", "Neustart...");
      delay(500);
      ESP.restart(); });

  // Ergänzung: /reset_config
  server.on("/reset_config", HTTP_GET, [](AsyncWebServerRequest *req)
            {
      req->send(200, "text/plain", "Zurückgesetzt – Neustart...");
      delay(500);
      cfg.reset(); });


#ifdef HAS_CAMERA
  server.on("/stream", HTTP_GET, streamJPG);
#endif
  server.begin();
}

void cleanUpClients()
{
  ws.cleanupClients();
}