#ifndef MAX31855_H
#define MAX31855_H

#include <stdint.h>

#define THERMO_FAULT_OPEN       10000
#define THERMO_FAULT_SHORT_GND  10001
#define THERMO_FAULT_SHORT_VCC  10002

class MAX31855 {
public:
  MAX31855(uint8_t cs);
  void begin();
  uint32_t rawData();
  float junctionTemperatureC();
  float internalTemperatureC();
protected:
  uint8_t m_cs;
};

#endif
