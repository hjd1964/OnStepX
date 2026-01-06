// -----------------------------------------------------------------------------------
// CAN transport - Server

#include "CanTransportServer.h"

#if defined(CAN_PLUS) && CAN_PLUS != OFF

CanTransportServer* CanTransportServer::s_instances[TRANSPORT_CAN_MAX_INSTANCES] = {nullptr};

CanTransportServer::CanTransportServer(uint16_t requestId, uint16_t responseId)
  : reqId(requestId), rspId(responseId) {}

bool CanTransportServer::init(bool startTask, uint16_t processPeriodMs) {
  if (!canPlus.ready) return false;

  if (slot < 0) slot = allocSlot(this);
  if (slot < 0) return false;

  if (!canPlus.callbackRegisterId((int)reqId, thunkForSlot(slot))) return false;

  if (startTask) {
    taskHandle = tasks.add(processPeriodMs, 0, true, 5, &CanTransportServer::taskThunk, "CanTrS");
    if (!taskHandle) return false;
  }

  return true;
}

void CanTransportServer::taskThunk() {
  // Run all instances (cheap); instances with empty queues return quickly.
  canPlus.rxBrust();
  for (int i = 0; i < (int)TRANSPORT_CAN_MAX_INSTANCES; i++) {
    if (s_instances[i]) s_instances[i]->process();
  }
}

void CanTransportServer::process() {
  Frame f;
  while (dequeue(f)) {
    processCommand(f.data, f.len);
  }
}

void CanTransportServer::sendResponse(uint8_t tidop, uint8_t status,
                                      const uint8_t *payload, uint8_t payloadLen) {
  // response = [tidop, status, payload...]
  uint8_t out[8] = {0};
  uint8_t outLen = 0;

  out[outLen++] = tidop;
  out[outLen++] = status;

  if (payload && payloadLen) {
    if (payloadLen > 6) payloadLen = 6;
    for (uint8_t i = 0; i < payloadLen; i++) out[outLen++] = payload[i];
  }

  canPlus.txWait();
  (void)canPlus.writePacket((int)rspId, out, outLen);
}

void CanTransportServer::enqueueFromCallback(const uint8_t data[8], uint8_t len) {
  if (len > 8) len = 8;
  if (len < 1) return;

  uint8_t next = (uint8_t)((qHead + 1) % TRANSPORT_CAN_SERVER_RXQ);
  if (next == qTail) return; // full, drop

  q[qHead].len = len;
  for (uint8_t i = 0; i < len; i++) q[qHead].data[i] = data[i];
  qHead = next;
}

bool CanTransportServer::dequeue(Frame &out) {
  if (qTail == qHead) return false;
  out = q[qTail];
  qTail = (uint8_t)((qTail + 1) % TRANSPORT_CAN_SERVER_RXQ);
  return true;
}

int8_t CanTransportServer::allocSlot(CanTransportServer *p) {
  for (int8_t i = 0; i < (int8_t)TRANSPORT_CAN_MAX_INSTANCES; i++) {
    if (s_instances[i] == nullptr) { s_instances[i] = p; return i; }
  }
  return -1;
}

CanTransportServer::ThunkFn CanTransportServer::thunkForSlot(int8_t s) {
  switch (s) {
    default: return &CanTransportServer::onThunk0;
    case 0:  return &CanTransportServer::onThunk0;
    case 1:  return &CanTransportServer::onThunk1;
    case 2:  return &CanTransportServer::onThunk2;
    case 3:  return &CanTransportServer::onThunk3;
    case 4:  return &CanTransportServer::onThunk4;
    case 5:  return &CanTransportServer::onThunk5;
    case 6:  return &CanTransportServer::onThunk6;
    case 7:  return &CanTransportServer::onThunk7;
  }
}

#define TRANSPORT_CAN_SERVER_THUNK(N) \
  void CanTransportServer::onThunk##N(uint8_t data[8], uint8_t len) { \
    CanTransportServer *p = s_instances[N]; \
    if (p) p->enqueueFromCallback((const uint8_t*)data, len); \
  }

TRANSPORT_CAN_SERVER_THUNK(0)
TRANSPORT_CAN_SERVER_THUNK(1)
TRANSPORT_CAN_SERVER_THUNK(2)
TRANSPORT_CAN_SERVER_THUNK(3)
TRANSPORT_CAN_SERVER_THUNK(4)
TRANSPORT_CAN_SERVER_THUNK(5)
TRANSPORT_CAN_SERVER_THUNK(6)
TRANSPORT_CAN_SERVER_THUNK(7)

#undef TRANSPORT_CAN_SERVER_THUNK

#endif
