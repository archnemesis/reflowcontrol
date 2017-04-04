#include "MAX31855.h"

#include <Arduino.h>
#include <SPI.h>

MAX31855::MAX31855(uint8_t cs)
{
  m_cs = cs;
}

void MAX31855::begin()
{
  pinMode(m_cs, OUTPUT);
  digitalWrite(m_cs, HIGH);
}

uint32_t MAX31855::rawData()
{
  uint32_t ret = 0;

  digitalWrite(m_cs, LOW);
  delay(1);

  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  ret = SPI.transfer(0);
  ret <<= 8;
  ret |= SPI.transfer(0);
  ret <<= 8;
  ret |= SPI.transfer(0);
  ret <<= 8;
  ret |= SPI.transfer(0);
  SPI.endTransaction();

  digitalWrite(m_cs, HIGH);
  return ret;
}

float MAX31855::internalTemperatureC()
{
  float temperature;
  uint32_t data = rawData();

  data >>= 4;
  temperature = data & 0x7FF;

  if (data & 0x800) {
    data = ~data;
    temperature = data & 0x7FF;
    temperature += 1;
    temperature *= -1;
  }

  temperature *= 0.0625;

  return temperature;
}

float MAX31855::junctionTemperatureC()
{
  float temperature = 0;
  uint32_t data = rawData();

  if (data & 0x00010000) {
    switch (data & 0x7) {
      case 0x01:
        temperature = THERMO_FAULT_OPEN;
        break;
      case 0x02:
        temperature = THERMO_FAULT_SHORT_GND;
        break;
      case 0x04:
        temperature = THERMO_FAULT_SHORT_VCC;
        break;
    }
  }
  else {
    data = data >> 18;
    temperature = (data & 0x1FFF);

    if (data & 0x2000) {
      data = ~data;
      temperature = data & 0x1FFF;
      temperature += 1;
      temperature *= -1;
    }

    temperature *= 0.25;
  }

  return temperature;
}
