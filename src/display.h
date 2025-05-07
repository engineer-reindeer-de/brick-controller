

#pragma once
#include <Adafruit_SSD1306.h>

void initDisplay();
void showStatus(const char* line1, const char* line2 = "", const char* line3 = "");

extern Adafruit_SSD1306 display;