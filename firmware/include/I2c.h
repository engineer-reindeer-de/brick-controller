#pragma once
#include "Arduino.h"
#include <Adafruit_I2CDevice.h>

class I2c {
public:
    I2c(int sda, int scl, TwoWire& twoWire);
    int sda;
    int scl;
    TwoWire& wiring;
};