#include "ESPAsyncWebServer.h"
#include "Repository/SettingsRepository.h"
#include "Wifi/WifiConnection.h"

class SettingsController{
    private:
        SettingsRepository &settingsRepository;
        WifiConnection &wifiConnection;
    public:
        SettingsController(SettingsRepository &settingsRepository, WifiConnection &wifiConnection); 
         void get(AsyncWebServerRequest *req);     
         void post(AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total);
};