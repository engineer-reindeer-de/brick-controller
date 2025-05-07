#include <Wire.h>
#include "display.h"
#include "config.h"

extern Config cfg;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initDisplay() {
  Wire.begin(cfg.i2c.sda, cfg.i2c.scl);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("⚠️ OLED nicht gefunden!");
    return;
  }
  display.clearDisplay();
  display.setRotation(0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("ESP32 LEGO-Controller");
  display.display();
}

void showStatus(const char* line1, const char* line2, const char* line3) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(line1);
  if (line2) display.println(line2);
  if (line3) display.println(line3);
  display.display();
}