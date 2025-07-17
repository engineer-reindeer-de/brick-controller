#include "Arduino.h"

class WidgetConfiguration
{
public:

    struct Option
    {
        char name[16];
        char label[16];
        char type[16];
        char defaultValue[16];
    };

    struct Widget
    {
        char type[16];
        char label[16];
        Option* options;  
    };

    struct ControllerMapping {
        char chip[12];
        char type[8];
        uint8_t channel;
        uint8_t pin;
    };
}