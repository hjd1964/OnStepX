// -----------------------------------------------------------------------------------
// Local command broker
#pragma once

#include "../../Common.h"

#if defined(SERIAL_LOCAL_MODE) && SERIAL_LOCAL_MODE == ON

#ifndef COMMAND_BROKER_SLOTS
  #define COMMAND_BROKER_SLOTS 8
#endif

#ifndef COMMAND_BROKER_COMMAND_LEN
  #define COMMAND_BROKER_COMMAND_LEN 40
#endif

#ifndef COMMAND_BROKER_REPLY_LEN
  #define COMMAND_BROKER_REPLY_LEN 80
#endif

#ifndef COMMAND_BROKER_TIMEOUT_MS
  #define COMMAND_BROKER_TIMEOUT_MS 100
#endif

#if COMMAND_BROKER_SLOTS > 15
  #error "COMMAND_BROKER_SLOTS must be 15 or less."
#endif

enum CommandBrokerStatus : uint8_t {
  CB_FREE,
  CB_PENDING,
  CB_SENT,
  CB_DONE,
  CB_TIMEOUT
};

class CommandBroker {
  public:
    bool init();

    // Queue commands here when a caller expects a reply. Do not mix brokered
    // request/reply traffic with direct SERIAL_LOCAL.receive() calls.
    uint8_t request(const char *command, unsigned long timeoutMs = COMMAND_BROKER_TIMEOUT_MS);
    // Queue commands here only when no reply will be consumed.
    bool send(const char *command);

    CommandBrokerStatus status(uint8_t handle);
    inline bool ready(uint8_t handle) { return status(handle) == CB_DONE; }
    inline bool timeout(uint8_t handle) { return status(handle) == CB_TIMEOUT; }
    CommandBrokerStatus result(uint8_t &handle, char *reply, size_t replySize);
    void release(uint8_t handle);

    void poll();

  private:
    struct Request {
      CommandBrokerStatus state = CB_FREE;
      char command[COMMAND_BROKER_COMMAND_LEN] = "";
      char reply[COMMAND_BROKER_REPLY_LEN] = "";
      unsigned long timeoutMs = COMMAND_BROKER_TIMEOUT_MS;
      unsigned long deadline = 0;
      bool replyExpected = true;
      uint8_t generation = 0;
    };

    Request requests[COMMAND_BROKER_SLOTS];
    int8_t active = -1;
    bool initialized = false;

    uint8_t enqueue(const char *command, unsigned long timeoutMs, bool replyExpected);
    uint8_t makeHandle(uint8_t slot);
    int8_t handleToSlot(uint8_t handle);
    bool handleValid(uint8_t handle, uint8_t slot);
    void lock();
    void unlock();

    #ifdef ESP32
      SemaphoreHandle_t mutex = NULL;
    #endif
};

extern CommandBroker commandBroker;

#endif
