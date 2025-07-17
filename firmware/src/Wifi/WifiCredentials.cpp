#include "wifi/WifiCredentials.h"
#include <cstring>

WifiCredentials::WifiCredentials() {
    // Sichere Kopie der SSID
    std::strncpy(this->ssid, ssid, sizeof(this->ssid) - 1);
    this->ssid[sizeof(this->ssid) - 1] = '\0';

    // Sichere Kopie des Passworts
    std::strncpy(this->password, password, sizeof(this->password) - 1);
    this->password[sizeof(this->password) - 1] = '\0';

    signalStrength = 0;  // Standardwert
}