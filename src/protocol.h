#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stdlib.h>

#define PROTOCOL_MSG_START            0xAE
#define PROTOCOL_MSG_ID_COMMAND       0x01
#define PROTOCOL_MSG_ID_STORE_PROFILE 0x02
#define PROTOCOL_MSG_ID_STATUS        0x03

#define PROTOCOL_COMMAND_START        0x01
#define PROTOCOL_COMMAND_STOP         0x02
#define PROTOCOL_COMMAND_CLEAR_MEM    0x03

typedef struct {
  uint8_t mode;
  uint8_t status;
  float temperature;
} StatusMessage;

typedef struct {
  uint8_t command_type;
  uint8_t command_arg1;
  uint8_t command_arg2;
  uint8_t command_arg3;
  uint8_t command_arg4;
} CommandMessage;

typedef struct {
  uint8_t error_no;
  uint8_t error_code;
} ErrorMessage;

typedef struct {
  uint8_t slot;
  char name[16];
  uint16_t ramp_to_soak_rate;
  uint16_t soak_temp;
  uint16_t soak_time;
  uint16_t ramp_to_peak_rate;
  uint16_t peak_temp;
  uint16_t ramp_cooling_rate;
} StoreProfileMessage;

class Protocol {
public:
  Protocol();
  void inputChar(const unsigned char byte);
  void inputBytes(const unsigned char *bytes, size_t size);
  void setWriteCharCallback(void (*callback)(unsigned char));
  void setCommandCallback(void (*callback)(CommandMessage*));
  void setStoreProfileCallback(void (*callback)(StoreProfileMessage*));
  // void sendErrorMessage(uint8_t error_no, uint8_t error_code);
  void sendStatusMessage(uint8_t mode, uint8_t status, float temperature);
protected:
  unsigned int m_bytesReceived;
  unsigned int m_bytesExpected;
  unsigned int m_packetTypeId;
  bool m_startOfPacketReceived;
  bool m_inPacket;
  char m_packetBuffer[255];

  void (*m_writeCharCallback)(unsigned char);
  void (*m_commandCallback)(CommandMessage*);
  void (*m_storeProfileCallback)(StoreProfileMessage*);
};

#endif
