#pragma once
#include "logging.h"
#include "Arduino.h"
#include "config.h"

void writeServo(uint8_t pin, int angle);
void initPwmPinsFromMapping(const Config& cfg);
int getChannelByPin(int pin);