#pragma once
#include <ESPAsyncWebServer.h>
#include "websocket.h"
#include <ArduinoJson.h>
#include <algorithm>   // für std::min
#include "wifisetup.h"
#include "servo.h"
#include <SD.h>
#include <LittleFS.h>
#include "config.h"

void setupServer();
void cleanUpClients();