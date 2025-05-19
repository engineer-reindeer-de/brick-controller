#pragma once
#include <stdint.h>
#include <cstring>
#ifdef BOARD_ESP32CAM
#include <Preferences.h>
#endif

#ifdef BOARD_ESP01
#include <EEPROM.h>
#endif

struct ControllerConfig {
  char name[16] = "";
  uint8_t port = 0;
  bool snapback = false;
  int16_t drift = 0;
};

void load();
void save();
void reset();
int findControllerIndexById(const char* id) const;
ControllerConfig* getById(const char* id);