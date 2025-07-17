#include "I2c.h"
#include <Wire.h>

I2c::I2c(int sda, int scl, TwoWire& twoWire)
    : sda(sda), scl(scl), wiring(twoWire) {}