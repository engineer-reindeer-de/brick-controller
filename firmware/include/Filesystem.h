#pragma once
#include "Logger.h"
#include "Arduino.h"
#include "FS.h"

class Filesystem
{
    public:
        Filesystem(Logger logger);
        bool exists(const char* path);
        bool readFile(const char* path, String& out);
        bool writeFile(const char* path, const String& content);
        fs::FS& getFS();
};