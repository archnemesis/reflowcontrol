#include <Arduino.h>
#include <unity.h>

#include "profile.h"

#ifdef UNIT_TEST

void test_profile_rampToSoakTime()
{
  Profile givenProfile;
  givenProfile.setSoakTemperature(150);
  givenProfile.setRampToSoakRate(2);

  TEST_ASSERT_EQUAL(63, givenProfile.rampToSoakTime(25.0));
}

void test_profile_rampToPeakTime()
{
  Profile givenProfile;
  givenProfile.setSoakTemperature(150);
  givenProfile.setPeakTemperature(250);
  givenProfile.setRampToPeakRate(2);

  TEST_ASSERT_EQUAL(50, givenProfile.rampToPeakTime());
}

void test_profile_coolingTime()
{
  Profile givenProfile;
  givenProfile.setPeakTemperature(250);
  givenProfile.setRampCoolingRate(2);

  TEST_ASSERT_EQUAL(113, givenProfile.coolingTime(25.0));
}

void test_profile_soakTime()
{
  Profile givenProfile;
  givenProfile.setSoakTime(90);

  TEST_ASSERT_EQUAL(90, givenProfile.soakTime());
}

void test_profile_totalTime()
{
  Profile givenProfile;
  givenProfile.setSoakTime(90);
  givenProfile.setSoakTemperature(150);
  givenProfile.setRampToSoakRate(2);
  givenProfile.setPeakTemperature(250);
  givenProfile.setRampToPeakRate(2);
  givenProfile.setRampCoolingRate(2);

  TEST_ASSERT_EQUAL(316, givenProfile.totalTime(25.0));
}

void setup()
{
  delay(2000);

  UNITY_BEGIN();
  RUN_TEST(test_profile_rampToSoakTime);
  RUN_TEST(test_profile_rampToPeakTime);
  RUN_TEST(test_profile_coolingTime);
  RUN_TEST(test_profile_soakTime);
  RUN_TEST(test_profile_totalTime);
  UNITY_END();
}

void loop()
{

}

#endif
