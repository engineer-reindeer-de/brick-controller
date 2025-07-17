#include "Repository/DashboardRepository.h"

DashboardRepository::DashboardRepository(Filesystem &filesystem): filesystem(filesystem) {}

String DashboardRepository::load()
{
    String json;
    if (!filesystem.readFile("/dashboard.json", json))
    {
        return "{}";
    }
    return json;
}

bool DashboardRepository::save(const String &json)
{
    return filesystem.writeFile("/dashboard.json", json);
}