#include <Arduino.h>

#include <ST7565.h>
#include <SPI.h>
#include <MAX31855.h>

#include "protocol.h"
#include "profile.h"

Protocol protocol;

//
// Chip select pins for thermocouple A and B
//
#define MAX31855_CS_A 8
#define MAX31855_CS_B 6

MAX31855 max31855_a(MAX31855_CS_A);

struct {
  uint8_t mode;
  uint8_t status;
} SystemStatus;

Profile ActiveProfile;

//
// System modes
//
#define MODE_STANDBY  1
#define MODE_RUNNING  2

#define STATUS_OK     1
#define STATUS_ERROR  2

void init_spi();
void init_thermocouples();
void init_display();

void update_display();
void write_status();
void process_serial();
void process_command_message(CommandMessage *msg);
void process_store_profile_message(StoreProfileMessage *msg);
void serial_putchar(unsigned char c);

#define ST7565_REVERSE
ST7565 glcd(3, 2, 4, 5, 6);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting up...");

  SystemStatus.mode = MODE_STANDBY;
  SystemStatus.status = STATUS_OK;

  //
  // Set a default active profile
  //
  ActiveProfile.setName("Default");
  ActiveProfile.setRampToSoakRate(2);
  ActiveProfile.setSoakTemperature(125);
  ActiveProfile.setSoakTime(60);
  ActiveProfile.setRampToPeakRate(2);
  ActiveProfile.setPeakTemperature(225);
  ActiveProfile.setRampCoolingRate(2);

  init_spi();
  init_thermocouples();
  init_display();

  protocol.setWriteCharCallback(&serial_putchar);
  protocol.setCommandCallback(&process_command_message);
  protocol.setStoreProfileCallback(&process_store_profile_message);

  delay(1000);
}

void loop() {
  write_status();
  update_display();
  process_serial();
  delay(100);
}

void init_display()
{
  glcd.begin(0x03);
  glcd.clear();
  glcd.drawstring(0, 0, "Testing...");
  glcd.display();
}

void write_status()
{
  float junction = max31855_a.junctionTemperatureC();

  protocol.sendStatusMessage(
    SystemStatus.mode,
    SystemStatus.status,
    junction);
}

void process_serial()
{
  while (Serial.available()) {
    protocol.inputChar(Serial.read());
  }
}

void serial_putchar(unsigned char c)
{
  Serial.write(c);
}

void process_command_message(CommandMessage *msg)
{
  switch (msg->command_type) {
    case PROTOCOL_COMMAND_START:
      SystemStatus.mode = MODE_RUNNING;
      break;
    case PROTOCOL_COMMAND_STOP:
      SystemStatus.mode = MODE_STANDBY;
      break;
  }
}

void process_store_profile_message(StoreProfileMessage *msg)
{
  ActiveProfile.setName((const char*)msg->name);
  ActiveProfile.setRampToSoakRate(msg->ramp_to_soak_rate);
  ActiveProfile.setSoakTemperature(msg->soak_temp);
  ActiveProfile.setSoakTime(msg->soak_time);
  ActiveProfile.setRampToPeakRate(msg->ramp_to_peak_rate);
  ActiveProfile.setPeakTemperature(msg->peak_temp);
  ActiveProfile.setRampCoolingRate(msg->ramp_cooling_rate);
}

void init_spi()
{
  SPI.begin();
}

void init_thermocouples()
{
  max31855_a.begin();
}

void update_display()
{
  char buf[16];

  glcd.clear();

  switch (SystemStatus.mode) {
    case MODE_STANDBY:
      glcd.drawstring(0, 0, "Standby");
      break;
    case MODE_RUNNING:
      glcd.drawstring(0, 0, "Running");
      break;
  }

  glcd.drawstring(0, 1, "Active Profile:");
  glcd.drawstring(16, 1, (char *)ActiveProfile.name());

  float junction = max31855_a.junctionTemperatureC();
  int whole = (int)junction;
  int fractional = (junction - whole) * 100;
  sprintf(buf, "%03d.%02d C", whole, fractional);
  glcd.drawstring(0, 7, (const char *)buf);
  glcd.display();
}
