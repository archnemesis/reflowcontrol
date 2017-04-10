#include "profile.h"
#include <stdint.h>
#include <string.h>
#include <math.h>

Profile::Profile()
{

}

void Profile::setName(const char *name)
{
  strncpy(m_name, name, 16);
}

void Profile::setRampToSoakRate(unsigned int rate)
{
  m_rampToSoakRate = rate;
}

void Profile::setSoakTemperature(unsigned int temp)
{
  m_soakTemperature = temp;
}

void Profile::setSoakTime(unsigned int time)
{
  m_soakTime = time;
}

void Profile::setRampToPeakRate(unsigned int rate)
{
  m_rampToPeakRate = rate;
}

void Profile::setPeakTemperature(unsigned int temp)
{
  m_peakTemperature = temp;
}

void Profile::setRampCoolingRate(unsigned int rate)
{
  m_rampCoolingRate = rate;
}

const char *Profile::name()
{
  return (const char *)m_name;
}

unsigned int Profile::rampToSoakRate()
{
  return m_rampToSoakRate;
}

unsigned int Profile::soakTemperature()
{
  return m_soakTemperature;
}

unsigned int Profile::soakTime()
{
  return m_soakTime;
}

unsigned int Profile::rampToPeakRate()
{
  return m_rampToPeakRate;
}

unsigned int Profile::peakTemperature()
{
  return m_peakTemperature;
}

unsigned int Profile::rampCoolingRate()
{
  return m_rampCoolingRate;
}

unsigned int Profile::totalTime(float ambient)
{
  unsigned int time = 0;
  time += rampToSoakTime(ambient) + m_soakTime;
  time += rampToPeakTime();
  time += coolingTime(ambient);
  return time;
}

unsigned int Profile::rampToSoakTime(float ambient)
{
  return round(((float)m_soakTemperature - ambient) / (float)m_rampToSoakRate);
}

unsigned int Profile::rampToPeakTime()
{
  return round(((float)m_peakTemperature - m_soakTemperature) / (float)m_rampToPeakRate);
}

unsigned int Profile::coolingTime(float ambient)
{
  return round(((float)m_peakTemperature - ambient) / (float)m_rampCoolingRate);
}
