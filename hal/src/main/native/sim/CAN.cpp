// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/CAN.h"

#include "mockdata/CanDataInternal.h"
#include "ctre/phoenix/platform/Platform.h"
#include <cstring>

using namespace hal;

const char* CAN_BUS = "can0";

namespace hal::init {
void InitializeCAN() {
  //ctre::phoenix::platform::can::RegisterCANbus(CAN_BUS);
}
}  // namespace hal::init

extern "C" {

void HAL_CAN_SendMessage(uint32_t messageID, const uint8_t* data,
                         uint8_t dataSize, int32_t periodMs, int32_t* status) {
  ctre::phoenix::platform::can::CANComm_SendMessage(messageID, data, dataSize, periodMs, status, CAN_BUS);
}
void HAL_CAN_ReceiveMessage(uint32_t* messageID, uint32_t messageIDMask,
                            uint8_t* data, uint8_t* dataSize,
                            uint32_t* timeStamp, int32_t* status) {
  // Use a data size of 42 as call check. Difficult to add check to invoke
  // handler
  *dataSize = 42;
  auto tmpStatus = *status;
  ctre::phoenix::platform::can::CANComm_ReceiveMessage(messageID, messageIDMask, data, dataSize,
                             timeStamp, status, CAN_BUS);
  // If no handler invoked, return message not found
  if (*dataSize == 42 && *status == tmpStatus) {
    *status = HAL_ERR_CANSessionMux_MessageNotFound;
  }
}
void HAL_CAN_OpenStreamSession(uint32_t* sessionHandle, uint32_t messageID,
                               uint32_t messageIDMask, uint32_t maxMessages,
                               int32_t* status) {
  ctre::phoenix::platform::can::CANComm_OpenStreamSession(sessionHandle, messageID, messageIDMask,
                                maxMessages, status, CAN_BUS);
}
void HAL_CAN_CloseStreamSession(uint32_t sessionHandle) {
  ctre::phoenix::platform::can::CANComm_CloseStreamSession(sessionHandle, CAN_BUS);
}
void HAL_CAN_ReadStreamSession(uint32_t sessionHandle,
                               struct HAL_CANStreamMessage* messages,
                               uint32_t messagesToRead, uint32_t* messagesRead,
                               int32_t* status) {
  static struct HAL_CANStreamMessage messages_buf[128];
  static ctre::phoenix::platform::can::canframe_t canframe_messages[128];
  ctre::phoenix::platform::can::CANComm_ReadStreamSession(sessionHandle, canframe_messages, 
      messagesToRead, messagesRead, status, CAN_BUS);
  for(size_t i=0; i < *messagesRead; i++)
  {
    messages_buf[i].messageID = canframe_messages[i].arbID;
    messages_buf[i].timeStamp = canframe_messages[i].timeStampUs;
    int toCopy = canframe_messages[i].len > 8 ? 8 : canframe_messages[i].len;
    memcpy(messages_buf[i].data, canframe_messages[i].data, toCopy);
    messages_buf[i].dataSize = toCopy;
  }
}
void HAL_CAN_GetCANStatus(float* percentBusUtilization, uint32_t* busOffCount,
                          uint32_t* txFullCount, uint32_t* receiveErrorCount,
                          uint32_t* transmitErrorCount, int32_t* status) {
  ctre::phoenix::platform::can::CANbus_GetStatus(*percentBusUtilization, *busOffCount, *txFullCount,
                           *receiveErrorCount, *transmitErrorCount, *status, CAN_BUS);
}

}  // extern "C"
