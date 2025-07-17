#include "Logger.h"
#include <Arduino.h>
#include <stdarg.h>

#ifdef HAS_DISPLAY
#include "I2cDevice/Display.h"
// Optional: assign this pointer from your main code once the Display is available
static Display* g_display = nullptr;
#endif

Logger::Logger()
{
    Serial.begin(115200);
    delay(1000);
}

void Logger::log(const char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    Serial.println(buffer);

#ifdef HAS_DISPLAY
    if (g_display) {
        g_display->show(buffer);
    }
#endif
}