#include "Controller/SettingsController.h"
#include "Controller/CameraController.h"
#include "Controller/DashboardController.h"
#include "Controller/Websocket.h"

class Webserver {
    private:
  SettingsController &settingsController;
  CameraController &cameraController;
  DashboardController &dashboardController;
  Filesystem &filesystem;
  Websocket &websocket;
    public:
        Webserver(
            SettingsRepository &settingsRepository,
            SettingsController &settingsController,
            CameraController &cameraController,
            DashboardController &dashboardController,
            Filesystem &filesystem,
            Websocket &websocket);
        void cleanUp();
};