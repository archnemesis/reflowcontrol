#ifndef PROFILE_H
#define PROFILE_H

class Profile {
public:
  Profile();
  void setName(const char *name);
  void setRampToSoakRate(unsigned int rate);
  void setSoakTemperature(unsigned int temp);
  void setSoakTime(unsigned int time);
  void setRampToPeakRate(unsigned int rate);
  void setPeakTemperature(unsigned int temp);
  void setRampCoolingRate(unsigned int rate);

  const char *name();
  unsigned int rampToSoakRate();
  unsigned int soakTemperature();
  unsigned int soakTime();
  unsigned int rampToPeakRate();
  unsigned int peakTemperature();
  unsigned int rampCoolingRate();

  unsigned int rampToSoakTime(float ambient);
  unsigned int rampToPeakTime();
  unsigned int coolingTime(float ambient);

  unsigned int totalTime(float ambient);
protected:
  char m_name[16];
  unsigned int m_rampToSoakRate;
  unsigned int m_soakTemperature;
  unsigned int m_soakTime;
  unsigned int m_rampToPeakRate;
  unsigned int m_peakTemperature;
  unsigned int m_rampCoolingRate;
};

#endif
