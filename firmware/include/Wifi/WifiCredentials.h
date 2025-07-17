#pragma once
class WifiCredentials
{
public:
    WifiCredentials();
    char ssid[64];
    char password[64];
    int signalStrength;
};