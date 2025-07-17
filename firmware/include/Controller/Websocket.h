#pragma once
#include <Adafruit_PWMServoDriver.h>
#include "I2cDevice/ServoController.h"
#include <ESPAsyncWebServer.h>

class Websocket
{
private:
    ServoController &servoController;

public:
    explicit Websocket(ServoController &servoController);

    void onWsEvent(
        AsyncWebSocket *server,
        AsyncWebSocketClient *client,
        AwsEventType type,
        void *arg,
        uint8_t *data, 
        size_t len
    );
};