#include <Arduino.h>
#include <Wire.h>
#include "I2cDevice/Display.h"

bool isConfigured = false;

Display::Display(I2c i2c, const uint8_t addr)
    : display(SCREEN_WIDTH, SCREEN_HEIGHT, &i2c.wiring, OLED_RESET)
{
    if (i2c.sda == -1 || i2c.scl == -1) {
        Serial.println("⚠️ I2C pins sind nicht konfiguriert!");
        return;
    }

    Wire.begin(i2c.sda, i2c.scl);

    if (!display.begin(SSD1306_SWITCHCAPVCC, addr)) {
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

void Display::show(const char *line1, const char *line2, const char *line3)
{
    if (!isConfigured) {
        return;
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // Battery level (placeholder)
    display.setCursor(0, 0);
    display.print("Batt: 100%");

    // IP address centered (placeholder)
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds("IP: 192.168.4.1", 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, 0);
    display.print("IP: 192.168.4.1");

    // WiFi strength right‑aligned (placeholder)
    display.setCursor(SCREEN_WIDTH - 50, 0);
    display.print("WiFi: 78%");

    // User‑supplied lines
    display.setCursor(0, h + 2);
    if (line1) display.println(line1);
    if (line2 && line2[0] != '\0') display.println(line2);
    if (line3 && line3[0] != '\0') display.println(line3);

    display.display();
}