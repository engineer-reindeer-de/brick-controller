#pragma once
#include <stdint.h>
#include <cstring>
#include "logging.h"

#if defined(BOARD_ESP32CAM)
#include "include/controllerConfigEsp32Cam.h"
#elif defined(BOARD_ESP32C3)
#include "include/controllerConfigEsp32c3.h"
#else
#error "Kein gültiges Board definiert – keine controller_config eingebunden"
#endif

#ifdef BOARD_ESP32CAM
#include <Preferences.h>
#include <esp_system.h>
#endif

#ifdef BOARD_ESP01
#include <EEPROM.h>
#endif

class Config
{
public:
    struct ControllerConfig
    {
        char name[16] = "";    // z. B. "joystick1_x", "slider3"
        uint8_t pin = -1;      // LEDC-PWM-Port oder GPIO
        bool snapback = false; // Rückstellung auf 0 bei Joystick
        int16_t drift = 0;     // Drift-Korrektur in -255 bis +255
    };

    struct I2C
    {
        uint8_t sda = -1;
        uint8_t scl = -1; // Drift-Korrektur in -255 bis +255
    };

    inline bool isValidControllerPin(uint8_t pin)
    {
        for (auto allowedPin : allowedControllerPins)
        {
            if (pin == allowedPin)
                return true;
        }
        return false;
    }

    ControllerConfig controllers[CONTROLLER_GPIO_COUNT]; // Anzahl nach CONTROLLER_GPIO_LIST

    I2C i2c;

    char ssid[64];     // WLAN-SSID
    char password[64]; // WLAN-Passwort

    void load();
    void save();
    void reset();

    int findControllerIndexById(const char *id) const;
    ControllerConfig *getById(const char *id);

    void print() const;
};