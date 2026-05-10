// -----------------------------------------------------------------------------------
// Local command broker

#include "CommandBroker.h"

#if defined(SERIAL_LOCAL_MODE) && SERIAL_LOCAL_MODE == ON

#include "../../lib/serial/Serial_Local.h"
#include "../../lib/tasks/OnTask.h"

void commandBrokerWrapper() { commandBroker.poll(); }

bool CommandBroker::init() {
  if (initialized) return true;

  #ifdef ESP32
    mutex = xSemaphoreCreateMutex();
    if (mutex == NULL) return false;
  #endif

  for (uint8_t i = 0; i < COMMAND_BROKER_SLOTS; i++) {
    requests[i].state = CB_FREE;
    requests[i].generation = 0;
  }

  initialized = true;
  VF("MSG: System, start command broker task (rate 3ms priority 5)... ");
  if (tasks.add(3, 0, true, 5, commandBrokerWrapper, "CmdBrkr")) { VLF("success"); return true; } else { VLF("FAILED!"); return false; }
}

uint8_t CommandBroker::request(const char *command, unsigned long timeoutMs) {
  return enqueue(command, timeoutMs, true);
}

bool CommandBroker::send(const char *command) {
  return enqueue(command, 0, false) != 0;
}

uint8_t CommandBroker::enqueue(const char *command, unsigned long timeoutMs, bool replyExpected) {
  if (!initialized || command == NULL || command[0] == 0) return 0;

  lock();
  for (uint8_t i = 0; i < COMMAND_BROKER_SLOTS; i++) {
    if (requests[i].state == CB_FREE) {
      sstrcpy(requests[i].command, command);
      requests[i].reply[0] = 0;
      requests[i].timeoutMs = timeoutMs;
      requests[i].deadline = 0;
      requests[i].replyExpected = replyExpected;
      requests[i].generation++;
      if (requests[i].generation == 0) requests[i].generation = 1;
      requests[i].state = CB_PENDING;
      uint8_t handle = makeHandle(i);
      unlock();
      return handle;
    }
  }
  unlock();
  return 0;
}

CommandBrokerStatus CommandBroker::status(uint8_t handle) {
  int8_t slot = handleToSlot(handle);
  if (slot < 0) return CB_FREE;

  lock();
  CommandBrokerStatus state = handleValid(handle, slot) ? requests[slot].state : CB_FREE;
  unlock();
  return state;
}

CommandBrokerStatus CommandBroker::result(uint8_t &handle, char *reply, size_t replySize) {
  if (reply != NULL && replySize > 0) reply[0] = 0;
  if (reply == NULL || replySize == 0) { handle = 0; return CB_FREE; }

  int8_t slot = handleToSlot(handle);
  if (slot < 0) { handle = 0; return CB_FREE; }

  lock();
  CommandBrokerStatus state = handleValid(handle, slot) ? requests[slot].state : CB_FREE;
  if (state == CB_DONE) {
    sstrcpyex(reply, requests[slot].reply, replySize);
    requests[slot].state = CB_FREE;
    handle = 0;
  } else if (state == CB_TIMEOUT) {
    requests[slot].state = CB_FREE;
    handle = 0;
  } else if (state == CB_FREE) {
    handle = 0;
  }
  unlock();
  return state;
}

void CommandBroker::release(uint8_t handle) {
  int8_t slot = handleToSlot(handle);
  if (slot < 0) return;

  lock();
  if (handleValid(handle, slot) && requests[slot].state != CB_SENT) requests[slot].state = CB_FREE;
  unlock();
}

void CommandBroker::poll() {
  if (!initialized) return;

  if (active < 0) {
    lock();
    for (uint8_t i = 0; i < COMMAND_BROKER_SLOTS; i++) {
      if (requests[i].state == CB_PENDING) {
        if (requests[i].replyExpected) requests[i].deadline = millis() + requests[i].timeoutMs;
        requests[i].state = CB_SENT;
        active = i;
        break;
      }
    }
    unlock();

    if (active >= 0) {
      SERIAL_LOCAL.receive();
      SERIAL_LOCAL.transmit(requests[active].command);
      if (!requests[active].replyExpected) {
        lock();
        requests[active].state = CB_FREE;
        active = -1;
        unlock();
      }
    }
  }

  if (active < 0) return;

  if (SERIAL_LOCAL.receiveAvailable()) {
    char *reply = SERIAL_LOCAL.receive();
    lock();
    sstrcpy(requests[active].reply, reply);
    requests[active].state = CB_DONE;
    active = -1;
    unlock();
  } else {
    lock();
    bool timedOut = requests[active].timeoutMs == 0 || (long)(millis() - requests[active].deadline) >= 0;
    if (timedOut) {
      requests[active].reply[0] = 0;
      requests[active].state = CB_TIMEOUT;
      active = -1;
    }
    unlock();
  }
}

uint8_t CommandBroker::makeHandle(uint8_t slot) {
  return ((requests[slot].generation & 0x0F) << 4) | ((slot + 1) & 0x0F);
}

int8_t CommandBroker::handleToSlot(uint8_t handle) {
  if (handle == 0) return -1;

  uint8_t slot = (handle & 0x0F) - 1;
  if (slot >= COMMAND_BROKER_SLOTS) return -1;
  return slot;
}

bool CommandBroker::handleValid(uint8_t handle, uint8_t slot) {
  return (requests[slot].generation & 0x0F) == (handle >> 4) && requests[slot].state != CB_FREE;
}

void CommandBroker::lock() {
  #ifdef ESP32
    xSemaphoreTake(mutex, portMAX_DELAY);
  #else
    noInterrupts();
  #endif
}

void CommandBroker::unlock() {
  #ifdef ESP32
    xSemaphoreGive(mutex);
  #else
    interrupts();
  #endif
}

CommandBroker commandBroker;

#endif
