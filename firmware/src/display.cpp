#include <Wire.h>
#include "display.h"
#include "config.h"

extern Config cfg;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
bool isConfigured = false;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initDisplay() {
  if(cfg.i2c.sda == -1 || cfg.i2c.scl == -1){
    Serial.println("⚠️ I2C pins sind nicht konfiguriert!");
    return;
  }
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
  isConfigured = true;
}

void showStatus(const char* line1, const char* line2, const char* line3) {
  if(!isConfigured){
    return;
  }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(line1);
  if (line2) display.println(line2);
  if (line3) display.println(line3);
  display.display();
}