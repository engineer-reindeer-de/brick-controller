#include "ESPAsyncWebServer.h"
#include "Repository/DashboardRepository.h"

class DashboardController{
    private:
    DashboardRepository dashboardRepository;

    public:
        DashboardController(DashboardRepository dashboardRepository);
        static void get(AsyncWebServerRequest *req);     
        static void post(AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total);
};