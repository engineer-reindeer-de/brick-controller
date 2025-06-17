#pragma once
#include <stdint.h>

constexpr uint8_t CONTROLLER_GPIO_COUNT = 6;

constexpr uint8_t allowedControllerPins[CONTROLLER_GPIO_COUNT] = {
  2, 4, 12, 13, 14, 15
};
