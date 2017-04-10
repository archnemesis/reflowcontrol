#include <Arduino.h>

#include <ST7565.h>
#include <SPI.h>
#include <MAX31855.h>
#include <PID_v1.h>

#include "protocol.h"
#include "profile.h"

Protocol protocol;

//
// Chip select pins for thermocouple A and B
//
#define MAX31855_CS_A 8
#define MAX31855_CS_B 9

#define PID_HEATER_P  2
#define PID_HEATER_I  5
#define PID_HEATER_D  1

#define DEBOUNCE_THRESHOLD  50

#define HEATER_PIN      A5
#define FAN_PIN         A6
#define HEATER_LED_PIN  10
#define FAN_LED_PIN     A7
#define BUTTON1_PIN     A0
#define BUTTON2_PIN     A1
#define BUTTON3_PIN     A2
#define BUTTON4_PIN     A3
#define BUTTON5_PIN     A4

MAX31855 max31855_a(MAX31855_CS_A);
MAX31855 max31855_b(MAX31855_CS_B);

struct {
  uint8_t mode;
  uint8_t status;
  uint32_t start_time;
  float temperature_a;
  float temperature_b;
  float start_temperature;
  double pid_heater_setpoint;
  double pid_heater_input;
  double pid_heater_output;
  uint8_t button1_state;
  uint8_t button1_state_prev;
  unsigned long button1_debounce;
} SystemStatus;

PID PID_Heater(
  &SystemStatus.pid_heater_input,
  &SystemStatus.pid_heater_output,
  &SystemStatus.pid_heater_setpoint,
  PID_HEATER_P, PID_HEATER_I, PID_HEATER_D, DIRECT
);

Profile ActiveProfile;

//
// System modes
//
#define MODE_STANDBY  1
#define MODE_RUNNING  2

#define STATUS_IDLE               1
#define STATUS_RAMPING_TO_SOAK    2
#define STATUS_SOAKING            3
#define STATUS_RAMPING_TO_PEAK    4
#define STATUS_COOLING            5
#define STATUS_WAITING            6

void init_spi();
void init_thermocouples();
void init_display();
void init_switches();
void init_leds();
void init_buttons();

void update_display();
void write_status();
void process_buttons();
void process_serial();
void process_command_message(CommandMessage *msg);
void process_store_profile_message(StoreProfileMessage *msg);
void serial_putchar(unsigned char c);
void draw_profile();
void run_program();
void halt_debug(const char *msg);

#define ST7565_REVERSE
ST7565 glcd(3, 2, 4, 5, 6);

#ifndef UNIT_TEST
void setup() {
  Serial.begin(57600);
  Serial.println("Starting up...");

  SystemStatus.mode = MODE_STANDBY;
  SystemStatus.status = STATUS_IDLE;
  SystemStatus.start_time = 0;
  SystemStatus.temperature_a = 0;
  SystemStatus.temperature_b = 0;
  SystemStatus.start_temperature = 0;
  SystemStatus.pid_heater_input = 0;
  SystemStatus.pid_heater_output = 0;
  SystemStatus.pid_heater_setpoint = 0;
  SystemStatus.button1_state = 0;
  SystemStatus.button1_state_prev = 0;
  SystemStatus.button1_debounce = 0;

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

  init_switches();
  init_buttons();
  init_spi();
  init_thermocouples();
  init_display();
  init_leds();

  protocol.setWriteCharCallback(&serial_putchar);
  protocol.setCommandCallback(&process_command_message);
  protocol.setStoreProfileCallback(&process_store_profile_message);

  delay(1000);
}

void loop() {
  process_serial();
  process_buttons();
  write_status();
  update_display();
  run_program();
}
#endif

void halt_debug(const char *msg)
{
  glcd.clear();
  glcd.drawstring(0, 0, msg);
  glcd.display();
  while (true) {
    ;;
  }
}

void init_switches()
{
  // HEATER
  pinMode(HEATER_PIN, OUTPUT);
  digitalWrite(HEATER_PIN, LOW);

  // FAN
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);
}

void init_leds()
{
  pinMode(HEATER_LED_PIN, OUTPUT);
  digitalWrite(HEATER_LED_PIN, LOW);

  pinMode(FAN_LED_PIN, OUTPUT);
  digitalWrite(FAN_LED_PIN, LOW);
}

void run_program()
{
  float temp_avg = (SystemStatus.temperature_a + SystemStatus.temperature_b) / 2.0;

  if (SystemStatus.mode == MODE_RUNNING) {
    switch (SystemStatus.status) {
      case STATUS_WAITING:
        if (SystemStatus.button1_state == LOW) {
          SystemStatus.status = STATUS_IDLE;
        }
        break;
      case STATUS_IDLE:
        SystemStatus.status = STATUS_RAMPING_TO_SOAK;
        SystemStatus.start_time = millis();
        SystemStatus.start_temperature = temp_avg;

        digitalWrite(FAN_LED_PIN, HIGH);
        //
        // set PID to set point...
        //
        break;
      case STATUS_RAMPING_TO_SOAK:
      {
        if ((millis() - SystemStatus.start_time) >= ((unsigned long)ActiveProfile.rampToSoakTime(SystemStatus.start_temperature) * 1000)) {
          SystemStatus.status = STATUS_SOAKING;
        }
        else {
          //
          // find the temp we should be at for this particular
          // moment in time and set the PID setpoint to that
          //
        }
        break;
      }
      case STATUS_SOAKING:
      {
        unsigned long soak_time = (unsigned long)(ActiveProfile.rampToSoakTime(
          SystemStatus.start_temperature) + ActiveProfile.soakTime()) * 1000;

        if ((millis() - SystemStatus.start_time) >= soak_time) {
          SystemStatus.status = STATUS_RAMPING_TO_PEAK;
        }
        else {
          //
          // maintain the temperature at the soak temp for the
          // duration of the soak cycle
          //
        }
        break;
      }
      case STATUS_RAMPING_TO_PEAK:
      {
        unsigned long peak_time = (unsigned long)(ActiveProfile.rampToSoakTime(
          SystemStatus.start_temperature) +
          ActiveProfile.soakTime() +
          ActiveProfile.rampToPeakTime()) * 1000;

        if ((millis() - SystemStatus.start_time) >= peak_time) {
          SystemStatus.status = STATUS_COOLING;
        }
        else {
          //
          // find the temp we should be at for this particular
          // moment in time and set the PID setpoint to that
          //
        }
        break;
      }
      case STATUS_COOLING:
      {
        unsigned long cooling_time = (unsigned long)(ActiveProfile.rampToSoakTime(
          SystemStatus.start_temperature) +
          ActiveProfile.soakTime() +
          ActiveProfile.rampToPeakTime() +
          ActiveProfile.coolingTime(SystemStatus.start_temperature)) * 1000;

        if ((millis() - SystemStatus.start_time) >= cooling_time) {
          SystemStatus.status = STATUS_IDLE;
          SystemStatus.mode = MODE_STANDBY;
        }
        else {
          //
          // control fan to maintain cooling rate
          //
        }
        break;
      }
    }
  }
}

void init_display()
{
  glcd.begin(0x03);
  glcd.clear();
  glcd.drawstring(0, 0, "T962-A Reflow 1.0");
  glcd.display();
}

void write_status()
{
  static unsigned long t = 0;

  if ((millis() - t) > 100) {
    float junction = max31855_a.junctionTemperatureC();

    protocol.sendStatusMessage(
      SystemStatus.mode,
      SystemStatus.status,
      millis() - SystemStatus.start_time,
      junction);

      t = millis();
  }
}

void process_serial()
{
  while (Serial.available()) {
    protocol.inputChar(Serial.read());
  }
}

void init_buttons()
{
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  pinMode(BUTTON4_PIN, INPUT_PULLUP);
  pinMode(BUTTON5_PIN, INPUT_PULLUP);
}

void process_buttons()
{
  int state = digitalRead(BUTTON5_PIN);

  if (state != SystemStatus.button1_state_prev) {
    SystemStatus.button1_debounce = millis();
  }

  if ((millis() - SystemStatus.button1_debounce) > DEBOUNCE_THRESHOLD) {
    if (state != SystemStatus.button1_state) {
      SystemStatus.button1_state = state;
    }
  }

  SystemStatus.button1_state_prev = state;
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
      SystemStatus.status = STATUS_WAITING;
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

void draw_profile()
{
  int graph_x = 0;
  int graph_y = 64 - 10;
  int graph_width = 128;
  int graph_height = 40;

  int total_time = ActiveProfile.totalTime(25);
  float dots_per_sec = (float)graph_width / (float)total_time;
  int peak_temp = ActiveProfile.peakTemperature();
  int soak_temp = ActiveProfile.soakTemperature();
  int soak_time = ActiveProfile.soakTime();
  float ramp_time = (float)soak_temp / (float)ActiveProfile.rampToSoakRate();
  float peak_time = (float)(peak_temp - soak_temp) / (float)ActiveProfile.rampToPeakRate();
  float dots_per_deg = (float)graph_height / (float)peak_temp;

  glcd.drawline(graph_x, graph_y, graph_x + graph_width, graph_y, 1);

  int ramp_x = graph_x + round(ramp_time * dots_per_sec);
  int ramp_y = graph_y - round(soak_temp * dots_per_deg);
  glcd.drawline(graph_x, graph_y, ramp_x, ramp_y, 1);

  glcd.drawline(
    ramp_x,
    ramp_y,
    ramp_x + round(soak_time * dots_per_sec),
    ramp_y,
    1
  );

  ramp_x += round(soak_time * dots_per_sec);

  glcd.drawline(
    ramp_x,
    ramp_y,
    ramp_x + round(peak_time * dots_per_sec),
    ramp_y - round((peak_temp - soak_temp) * dots_per_deg),
    1
  );

  ramp_x += round(peak_time * dots_per_sec);
  ramp_y -= round((peak_temp - soak_temp) * dots_per_deg);

  glcd.drawline(
    ramp_x,
    ramp_y,
    graph_x + graph_width,
    graph_y,
    1
  );
}

void init_spi()
{
  SPI.begin();
}

void init_thermocouples()
{
  max31855_a.begin();
  max31855_b.begin();
}

void update_display()
{
  char buf[16];

  glcd.clear();
  glcd.drawstring(0, 1, (char *)ActiveProfile.name());

  draw_profile();

  switch (SystemStatus.mode) {
    case MODE_STANDBY:
      glcd.drawstring(0, 0, "Standby");
      break;
    case MODE_RUNNING:
    {
      glcd.drawstring(0, 0, "Running:");

      switch (SystemStatus.status) {
        case STATUS_WAITING:
          glcd.fillrect(10, 10, 108, 44, 1);
          glcd.fillrect(11, 11, 106, 42, 0);
          glcd.drawstring(20, 3, "Press START");
          break;
        case STATUS_RAMPING_TO_SOAK:
        case STATUS_RAMPING_TO_PEAK:
          glcd.drawstring(64, 0, "Heating");
          break;
        case STATUS_SOAKING:
          glcd.drawstring(64, 0, "Soak");
          break;
        case STATUS_COOLING:
          glcd.drawstring(64, 0, "Cooling");
          break;
      }

      int seconds = (millis() - SystemStatus.start_time) / 1000;
      sprintf(buf, "%03d", seconds);
      glcd.drawstring(64, 7, (const char *)buf);

      break;
    }
  }

  float junction_a = max31855_a.junctionTemperatureC();
  float junction_b = max31855_b.junctionTemperatureC();
  float junction = (junction_a + junction_b) / 2.0;
  SystemStatus.temperature_a = junction;
  SystemStatus.temperature_b = junction;
  int whole = (int)junction;
  int fractional = (junction - whole) * 100;
  sprintf(buf, "%03d.%02d C", whole, fractional);
  glcd.drawstring(0, 7, (const char *)buf);
  glcd.display();
}
