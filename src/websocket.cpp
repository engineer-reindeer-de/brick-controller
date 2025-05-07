#include "config.h"

extern Config cfg;

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "servo.h"

extern int lastAngleX;
extern int lastAngleY;

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_DATA)
  {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->opcode != WS_TEXT)
      return;

    String msg;
    msg.reserve(len);
    for (size_t i = 0; i < len; ++i)
    {
      msg += (char)data[i];
    }

    int sep = msg.indexOf(':');
    if (sep <= 0)
      return;

    char axis = msg[0];
    Serial.println(axis);
    int value = msg.substring(sep + 1).toInt();

    int index = axis - '1'; // '1' → 0, '2' → 1, ..., '8' → 7
    if (index < 0 || index >= CONTROLLER_GPIO_COUNT) {
      Serial.printf("⚠️ Ungültiger Controller-Index: %c\n", axis);
      return;
    }

    Config::ControllerConfig* ctrl = &cfg.controllers[index];
    int angle = mapJS(value + ctrl->drift);

    Serial.printf("[WebSocket] Index: %d, Pin: %d, Wert: %d → Winkel: %d\n", index, ctrl->pin, value, angle);
    writeServo(ctrl->pin, angle);

    // driftKorrektur fehlt noch!
  }
}