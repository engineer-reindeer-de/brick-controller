#pragma once
#include "logging.h"
#include "Arduino.h"
#include "config.h"

void writeServo(uint8_t pin, int angle);
int mapJS(int val);
void initPwmPinsFromMapping(const Config& cfg);