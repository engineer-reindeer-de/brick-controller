#pragma once
#include <Adafruit_SSD1306.h>
#include <I2c.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
extern bool isConfigured;

class Display
{
public:
    Display(I2c i2c, const uint8_t addr);
    void show(const char *line1, const char *line2 = "", const char *line3 = "");

private:
    Adafruit_SSD1306 display;
};