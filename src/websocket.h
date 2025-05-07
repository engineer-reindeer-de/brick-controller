#pragma once

#include <Adafruit_PWMServoDriver.h>

void onWsEvent(AsyncWebSocket *server,AsyncWebSocketClient *client,AwsEventType type,void *arg,uint8_t *data,size_t len);
extern Adafruit_PWMServoDriver pwm;
extern int servoPositions[6];