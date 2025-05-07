#include <logging.h>

#ifdef HAS_DISPLAY
#include "display.h"
#endif

void setupLogging()
{
  Serial.begin(115200);
  delay(1000);
#ifdef HAS_DISPLAY
  initDisplay();
#endif
}

void logInfo(const char *format, ...)
{
  char buffer[256];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  Serial.println(buffer);
#ifdef HAS_DISPLAY
showStatus(buffer);
#endif
}