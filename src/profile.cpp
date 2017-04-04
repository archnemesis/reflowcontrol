#include "profile.h"
#include <stdint.h>
#include <string.h>

Profile::Profile()
{

}

void Profile::setName(const char *name)
{
  strncpy(m_name, name, 16);
}

void Profile::setRampToSoakRate(int rate)
{
  m_rampToSoakRate = rate;
}

void Profile::setSoakTemperature(int temp)
{
  m_soakTemperature = temp;
}

void Profile::setSoakTime(int time)
{
  m_soakTime = time;
}

void Profile::setRampToPeakRate(int rate)
{
  m_rampToPeakRate = rate;
}

void Profile::setPeakTemperature(int temp)
{
  m_peakTemperature = temp;
}

void Profile::setRampCoolingRate(int rate)
{
  m_rampCoolingRate = rate;
}

const char *Profile::name()
{
  return (const char *)m_name;
}

int Profile::rampToSoakRate()
{
  return m_rampToSoakRate;
}

int Profile::soakTemperature()
{
  return m_soakTemperature;
}

int Profile::soakTime()
{
  return m_soakTime;
}

int Profile::rampToPeakRate()
{
  return m_rampToPeakRate;
}

int Profile::peakTemperature()
{
  return m_peakTemperature;
}

int Profile::rampCoolingRate()
{
  return m_rampCoolingRate;
}
