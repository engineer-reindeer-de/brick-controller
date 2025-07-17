#pragma once
#include <stdarg.h>

class Logger
{
public:
    Logger();
    void log(const char *format, ...);
};