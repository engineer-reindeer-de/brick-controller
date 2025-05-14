#pragma once
#include <stdint.h>
#include <cstring>
#include "logging.h"

#if defined(BOARD_ESP01)
#include "include/controllerConfigEsp01.h"
#elif defined(BOARD_ESP32CAM)
#include "include/controllerConfigEsp32Cam.h"
#elif defined(BOARD_ESP32C3)
#include "include/controllerConfigEsp32C3.h"
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

    struct Option
    {
        char name[16];
        char label[16];
        char type[16];
        char defaultValue[16];
    };

    struct Widget
    {
        char type[16];
        char label[16];
        Option* options;  
    };
    
    struct I2C
    {
        uint8_t sda = -1;
        uint8_t scl = -1; 
    };

    struct ControllerMapping {
        char chip[12];
        char type[8];
        uint8_t channel;
        uint8_t pin;
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
    I2C i2c;

    char ssid[64];     // WLAN-SSID
    char password[64]; // WLAN-Passwort

    void load();
    void save();
    void reset();
    void print() const;
    char* widgetsAsJson();
};