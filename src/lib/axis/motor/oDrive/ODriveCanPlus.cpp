// -----------------------------------------------------------------------------------
// axis oDrive servo motor CanPlus interface

#include "ODriveCanPlus.h"

#ifdef ODRIVE_NEW_MOTOR_PRESENT
#ifdef ODRIVE_MOTOR_PRESENT

#include "../../../canPlus/CanPlus.h"

static void odHb0(uint8_t d[8])  { odriveCan.onHeartbeat(0, d); }
static void odEnc0(uint8_t d[8]) { odriveCan.onFeedbackPosVel(0, d); }
static void odHb1(uint8_t d[8])  { odriveCan.onHeartbeat(1, d); }
static void odEnc1(uint8_t d[8]) { odriveCan.onFeedbackPosVel(1, d); }
static void odVer0(uint8_t d[8]) { odriveCan.onGetVersion(0, d); }
static void odVer1(uint8_t d[8]) { odriveCan.onGetVersion(1, d); }

bool ODriveCanPlus::init(uint8_t node) {
  if (!canPlus.ready) return false;
  switch (node) {
    case 0:
      canPlus.callbackRegisterId(makeId(0, CMD_HEARTBEAT), odHb0);
      canPlus.callbackRegisterId(makeId(0, CMD_GET_ENCODER_EST), odEnc0);
      canPlus.callbackRegisterId(makeId(0, CMD_GET_VERSION), odVer0);
      return true;
    case 1:
      canPlus.callbackRegisterId(makeId(1, CMD_HEARTBEAT), odHb1);
      canPlus.callbackRegisterId(makeId(1, CMD_GET_ENCODER_EST), odEnc1);
      canPlus.callbackRegisterId(makeId(1, CMD_GET_VERSION), odVer1);
      return true;
    default:
      return false;
  }
}

bool ODriveCanPlus::getVersionSnapshot(uint8_t node,
                                       uint8_t raw[8],
                                       uint16_t& proto,
                                       uint8_t& fw_major,
                                       uint8_t& fw_minor,
                                       uint8_t& fw_rev,
                                       uint8_t& hw_variant,
                                       uint8_t& hw_version) const {
  if (node >= MaxNodes || raw == nullptr) return false;

  uint8_t seen;
  noInterrupts();
  seen = cache_[node].seen;
  if (!(seen & SEEN_VER)) { interrupts(); return false; }

  proto      = cache_[node].proto_ver;
  fw_major   = cache_[node].fw_major;
  fw_minor   = cache_[node].fw_minor;
  fw_rev     = cache_[node].fw_rev;
  hw_variant = cache_[node].hw_variant;
  hw_version = cache_[node].hw_version;
  for (int i = 0; i < 8; i++) raw[i] = cache_[node].ver_raw[i];
  interrupts();

  return true;
}

bool ODriveCanPlus::hasFreshVersion(uint8_t node, uint32_t max_age_ms) const {
  if (node >= MaxNodes) return false;
  uint8_t seen;
  uint32_t t;
  noInterrupts();
  seen = cache_[node].seen;
  t = cache_[node].last_ver_ms;
  interrupts();
  if (!(seen & SEEN_VER)) return false;
  return (uint32_t)(millis() - t) <= max_age_ms;
}

bool ODriveCanPlus::setAxisState(uint8_t node, uint32_t requested_state) {
  if (node >= MaxNodes) return false;
  // A state change invalidates any queued position for this node.
  inputPos_[node].pending = false;
  if (requested_state == STATE_CLOSED_LOOP_CONTROL) inputPos_[node].failed = false;
  if (!canPlus.ready) return false;

  uint8_t d[8] = {0};
  put_le32(&d[0], requested_state);

  // Infrequent state commands bypass position pacing; never wait or yield here.
  return canPlus.writePacket(makeId(node, CMD_SET_AXIS_STATE), d, 8) == 1;
}

uint8_t ODriveCanPlus::axisState(uint8_t node) const {
  if (node >= MaxNodes) return 0;
  noInterrupts();
  uint8_t s = cache_[node].axis_state;
  interrupts();
  return s;
}

void ODriveCanPlus::setInputPos(uint8_t node, float pos_turns, float vel_ff_turns_s , float tq_ff_nm) {
  if (node >= MaxNodes || inputPos_[node].failed) return;
  InputPos &position = inputPos_[node];
  uint8_t *d = position.data;

  const int32_t vel_i = lroundf(vel_ff_turns_s  * 1000.0F);
  const int32_t tq_i  = lroundf(tq_ff_nm        * 1000.0F);

  put_f32(&d[0], pos_turns);
  put_le16(&d[4], clamp_i16(vel_i));
  put_le16(&d[6], clamp_i16(tq_i));
  
  // Replace the pending value without extending its delivery deadline.
  if (!position.pending) position.pendingSince = millis();
  position.pending = true;
}

void ODriveCanPlus::pollInputPos() {
  // Both motors service this queue; give each pending node its turn.
  for (uint8_t i = 0; i < MaxNodes; i++) {
    const uint8_t node = (nextInputPos + i) % MaxNodes;
    InputPos &position = inputPos_[node];
    if (!position.pending) continue;
    if (!canPlus.ready || (uint32_t)(millis() - position.pendingSince) >= 250U) {
      position.pending = false;
      position.failed = true;
      continue;
    }
    if (!canPlus.txTryLock()) return;
    nextInputPos = (node + 1) % MaxNodes;
    if (canPlus.writePacket(makeId(node, CMD_SET_INPUT_POS), position.data, 8) == 1)
      position.pending = false;
    // A failed write keeps the latest position pending for a bounded retry.
    return;
  }
}

float ODriveCanPlus::getPosTurns(uint8_t node) const {
  if (node >= MaxNodes) return 0.0F;
  noInterrupts();
  float p = cache_[node].pos_turns;
  interrupts();
  return p;
}

bool ODriveCanPlus::hasFreshTurns(uint8_t node, uint32_t max_age_ms) const {
  if (node >= MaxNodes) return false;
  uint8_t seen;
  uint32_t t;
  noInterrupts();
  seen = cache_[node].seen;
  t = cache_[node].last_enc_ms;
  interrupts();
  if (!(seen & SEEN_ENC)) return false;
  return (uint32_t)(millis() - t) <= max_age_ms;
}

bool ODriveCanPlus::hasHeartbeat(uint8_t node, uint32_t max_age_ms) const {
  if (node >= MaxNodes) return false;
  uint8_t seen;
  uint32_t t;
  noInterrupts();
  seen = cache_[node].seen;
  t = cache_[node].last_hb_ms;
  interrupts();
  if (!(seen & SEEN_HB)) return false;
  return (uint32_t)(millis() - t) <= max_age_ms;
}

uint32_t ODriveCanPlus::lastHeartbeatMs(uint8_t node) const {
  if (node >= MaxNodes) return 0;
  noInterrupts();
  uint32_t t = cache_[node].last_hb_ms;
  interrupts();
  return t;
}

uint32_t ODriveCanPlus::lastError(uint8_t node) const {
  if (node >= MaxNodes) return 0;
  noInterrupts();
  uint32_t e = cache_[node].last_err;
  interrupts();
  return e;
}

void ODriveCanPlus::onHeartbeat(uint8_t node, const uint8_t* d, uint8_t len) {
  if (node >= MaxNodes || len < 7) return;

  cache_[node].last_hb_ms = millis();
  cache_[node].last_err =
      (uint32_t(d[0])      ) |
      (uint32_t(d[1]) <<  8) |
      (uint32_t(d[2]) << 16) |
      (uint32_t(d[3]) << 24);
  cache_[node].axis_state = d[4];

  ATOMIC_OR(cache_[node].seen, SEEN_HB);

  // optionally store:
  // cache_[node].procedure_result = d[5];
  // cache_[node].traj_done = d[6];
}

bool ODriveCanPlus::requestVersion(uint8_t node) {
  if (!canPlus.ready || node >= MaxNodes) return false;
  return canPlus.writePacketRtr(makeId(node, CMD_GET_VERSION), 8) == 1;
}

bool ODriveCanPlus::requestTurns(uint8_t node) {
  if (!canPlus.ready || node >= MaxNodes) return false;
  return canPlus.writePacketRtr(makeId(node, CMD_GET_ENCODER_EST), 8) == 1;
}

void ODriveCanPlus::onGetVersion(uint8_t node, const uint8_t* d, uint8_t len) {
  if (node >= MaxNodes || len < 8) return;

  const uint32_t now = millis();

  for (int i = 0; i < 8; i++) cache_[node].ver_raw[i] = d[i];

  cache_[node].proto_ver  = get_u16_le(&d[0]);
  cache_[node].fw_major   = d[2];
  cache_[node].fw_minor   = d[3];
  cache_[node].fw_rev     = d[4];
  cache_[node].hw_variant = d[5];
  cache_[node].hw_version = d[6];

  cache_[node].last_ver_ms = now;        // write time first

  ATOMIC_OR(cache_[node].seen, SEEN_VER);
}

void ODriveCanPlus::onFeedbackPosVel(uint8_t node, const uint8_t* d, uint8_t len) {
  if (node >= MaxNodes || len < 8) return;

  // An unknown absolute position can be reported as NaN.
  const float position = get_f32(d + 0);
  if (isnan(position) || isinf(position)) {
    ATOMIC_AND(cache_[node].seen, (uint8_t)~SEEN_ENC);
    return;
  }

  cache_[node].pos_turns   = position;
  cache_[node].vel_turns_s = get_f32(d + 4);
  cache_[node].last_enc_ms = millis();

  ATOMIC_OR(cache_[node].seen, SEEN_ENC);
}

ODriveCanPlus odriveCan;

#endif
#endif
