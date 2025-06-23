#include "config.h"

extern Config cfg;

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h> 
#include "servo.h"

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  if (type != WS_EVT_DATA) return;

  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->opcode != WS_TEXT) return;

  String msg;
  msg.reserve(len);
  for (size_t i = 0; i < len; ++i) {
    msg += (char)data[i];
  }

  // JSON-Format: { "target": { "type": "pwm", "chip": "esp32", "pin": 12 }, "value": 123, "offset": 23  }
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, msg);
  if (err) {
    Serial.printf("⚠️ JSON-Fehler: %s\n", err.c_str());
    return;
  }

  JsonObject target = doc["target"];
  const char* targetType = target["type"] | "";
  const char* chip = target["chip"] | "";
  int pin = target["pin"] | -1;
  int value = doc["value"] | -1;
  int offset = doc["offset"] | 0;
  int drift = doc["drift"] | 0;

  if (strcmp(targetType, "pwm") != 0 || value < 0 || value > 255 || pin < 0) {
    Serial.println("⚠️ Ungültige Steuerdaten");
    return;
  }

  value = value + drift;

  int angle = map(value, offset, 255-offset, 0, 180);
  Serial.printf("[WebSocket] %s.%s pin %d = %d (Winkel: %d)\n", targetType, chip, pin, value, angle);

  if (pin < 0) {
    Serial.printf("⚠️ Ungültiger oder fehlender Pin in Nachricht: %s.%s\n", targetType, chip);
    return;
  }
  writeServo(pin, angle);
}