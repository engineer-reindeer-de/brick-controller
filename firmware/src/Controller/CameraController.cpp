#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include "Camera/AsyncJpegStreamResponse.cpp"
#include "Controller/CameraController.h"

void CameraController::streamJpg(AsyncWebServerRequest *request)
{
    auto *response = new AsyncJpegStreamResponse();
    if (!response)
    {
        request->send(503, "text/plain", "Speicherfehler");
        return;
    }
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
};