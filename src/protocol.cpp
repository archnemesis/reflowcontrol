#include "protocol.h"

void _defaultCommandCallback(CommandMessage*) {}
void _defaultStoreProfileCallback(StoreProfileMessage*) {}

Protocol::Protocol()
{
  m_bytesReceived = 0;
  m_bytesExpected = 0;
  m_packetTypeId = 0;
  m_inPacket = false;
  m_commandCallback = &_defaultCommandCallback;
  m_storeProfileCallback = &_defaultStoreProfileCallback;
}

void Protocol::setWriteCharCallback(void (*callback)(unsigned char))
{
  m_writeCharCallback = callback;
}

void Protocol::setCommandCallback(void (*callback)(CommandMessage*))
{
  m_commandCallback = callback;
}

void Protocol::setStoreProfileCallback(void (*callback)(StoreProfileMessage*))
{
  m_storeProfileCallback = callback;
}

void Protocol::inputBytes(const unsigned char *bytes, size_t size)
{
  unsigned int i;

  for (i = 0; i < size; i++) {
    inputChar(bytes[i]);
  }
}

void Protocol::inputChar(const unsigned char byte)
{
  if (!m_startOfPacketReceived) {
    if (byte == PROTOCOL_MSG_START) {
      m_startOfPacketReceived = true;
    }
  }
  else {
    if (!m_inPacket) {
      uint8_t packet_id = byte;

      switch (packet_id) {
        case PROTOCOL_MSG_ID_COMMAND:
        {
          m_inPacket = true;
          m_packetTypeId = PROTOCOL_MSG_ID_COMMAND;
          m_bytesExpected = sizeof(CommandMessage);
          m_bytesReceived = 0;
          break;
        }
        case PROTOCOL_MSG_ID_STORE_PROFILE:
        {
          m_inPacket = true;
          m_packetTypeId = PROTOCOL_MSG_ID_STORE_PROFILE;
          m_bytesExpected = sizeof(StoreProfileMessage);
          m_bytesReceived = 0;
          break;
        }
        default:
        {
          m_startOfPacketReceived = false;
          m_inPacket = false;
          m_packetTypeId = 0;
          m_bytesExpected = 0;
          m_bytesReceived = 0;
          break;
        }
      }
    }
    else {
      m_packetBuffer[m_bytesReceived++] = byte;

      if (m_bytesReceived == m_bytesExpected) {
        switch (m_packetTypeId) {
          case PROTOCOL_MSG_ID_COMMAND:
          {
            CommandMessage *msg = (CommandMessage *)m_packetBuffer;
            m_commandCallback(msg);
            break;
          }
          case PROTOCOL_MSG_ID_STORE_PROFILE:
          {
            StoreProfileMessage *msg = (StoreProfileMessage *)m_packetBuffer;
            m_storeProfileCallback(msg);
            break;
          }
        }

        m_startOfPacketReceived = false;
        m_inPacket = false;
        m_packetTypeId = 0;
        m_bytesExpected = 0;
        m_bytesReceived = 0;
      }
    }
  }
}

void Protocol::sendStatusMessage(uint8_t mode, uint8_t status, float temperature)
{
  StatusMessage msg;
  msg.mode = mode;
  msg.status = status;
  msg.temperature = temperature;

  m_writeCharCallback(PROTOCOL_MSG_START);
  m_writeCharCallback(PROTOCOL_MSG_ID_STATUS);

  for (size_t i = 0; i < sizeof(StatusMessage); i++) {
    unsigned char c = ((unsigned char *)&msg)[i];
    m_writeCharCallback(c);
  }
}
