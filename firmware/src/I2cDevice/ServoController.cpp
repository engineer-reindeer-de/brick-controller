#include "I2cDevice/ServoController.h"

ServoController::ServoController(I2c i2c, const uint8_t addr)
    : controller(addr, i2c.wiring)
{
    controller.begin();
}

void ServoController::set(uint8_t pin, int value)
{
    // value should be between 0‑4095 (12‑bit)
    controller.setPWM(pin, 0, value);
}