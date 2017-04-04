#ifndef PROFILE_H
#define PROFILE_H

class Profile {
public:
  Profile();
  void setName(const char *name);
  void setRampToSoakRate(int rate);
  void setSoakTemperature(int temp);
  void setSoakTime(int time);
  void setRampToPeakRate(int rate);
  void setPeakTemperature(int temp);
  void setRampCoolingRate(int rate);

  const char *name();
  int rampToSoakRate();
  int soakTemperature();
  int soakTime();
  int rampToPeakRate();
  int peakTemperature();
  int rampCoolingRate();
protected:
  char m_name[16];
  int m_rampToSoakRate;
  int m_soakTemperature;
  int m_soakTime;
  int m_rampToPeakRate;
  int m_peakTemperature;
  int m_rampCoolingRate;
};

#endif
