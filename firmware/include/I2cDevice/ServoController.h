#pragma once
#include <I2c.h>
#include <Adafruit_PWMServoDriver.h>

class ServoController
{
public:
    ServoController(I2c i2c, const uint8_t addr = 0x40);
    void set(uint8_t pin, int value);

private:
    Adafruit_PWMServoDriver controller;
};