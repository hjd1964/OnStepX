// -----------------------------------------------------------------------------------
// axis oDrive servo motor CanPlus interface
#pragma once
#include "../../../../Common.h"

#ifdef ODRIVE_NEW_MOTOR_PRESENT
#ifdef ODRIVE_MOTOR_PRESENT

class ODriveCanPlus {
public:
  bool init(uint8_t node);
  
  bool getVersionSnapshot(uint8_t node,
                          uint8_t raw[8],
                          uint16_t& proto,
                          uint8_t& fw_major,
                          uint8_t& fw_minor,
                          uint8_t& fw_rev,
                          uint8_t& hw_variant,
                          uint8_t& hw_version) const;
  
  bool setAxisState(uint8_t node, uint32_t requested_state);
  uint8_t axisState(uint8_t node) const;
 
  void setInputPos(uint8_t node, float pos_turns, float vel_ff_turns_s = 0.0F, float tq_ff_nm = 0.0F);
  float getPosTurns(uint8_t node) const; // cached feedback
  bool hasFreshTurns(uint8_t node, uint32_t max_age_ms = 250U) const;

  void onGetVersion(uint8_t node, const uint8_t* d, uint8_t len = 8);
  bool hasFreshVersion(uint8_t node, uint32_t max_age_ms = 1000U) const;

  bool hasHeartbeat(uint8_t node, uint32_t max_age_ms = 250U) const;
  uint32_t lastHeartbeatMs(uint8_t node) const;
  uint32_t lastError(uint8_t node) const;

  void onHeartbeat(uint8_t node, const uint8_t* d, uint8_t len = 8);
  void onFeedbackPosVel(uint8_t node, const uint8_t* d, uint8_t len = 8);

  bool requestVersion(uint8_t node);
  bool requestTurns(uint8_t node);

private:

  // helpers
  constexpr uint8_t CmdBits = 5;
  static inline uint16_t makeId(uint8_t node, uint8_t cmd) { return (uint16_t(node) << CmdBits) | (cmd & 0x1F); }
  
  static inline void put_le16(uint8_t* p, int16_t v) { p[0] = uint8_t(v & 0xFF); p[1] = uint8_t((v >> 8) & 0xFF); }
  static inline void put_le32(uint8_t* p, uint32_t v) {
    p[0]=(uint8_t)v;
    p[1]=(uint8_t)(v>>8);
    p[2]=(uint8_t)(v>>16);
    p[3]=(uint8_t)(v>>24);
  }
  static inline void put_f32(uint8_t* p, float f) {
    static_assert(sizeof(float) == 4, "float must be 32-bit");
    uint32_t u;
    memcpy(&u, &f, 4);
    put_le32(p, u);
  }

  static inline float get_f32(const uint8_t* p) {
    uint32_t u = (uint32_t(p[0])) |
                 (uint32_t(p[1]) <<  8) |
                 (uint32_t(p[2]) << 16) |
                 (uint32_t(p[3]) << 24);
    float f;
    memcpy(&f, &u, 4);
    return f;
  }
  static inline uint16_t get_u16_le(const uint8_t* p) { return (uint16_t)p[0] | ((uint16_t)p[1] << 8); }

  static inline int16_t clamp_i16(int32_t x) {
    if (x > 32767) return  32767;
    if (x < -32768) return -32768;
    return (int16_t)x;
  }

  // ODrive CMD IDs
  static constexpr uint8_t CMD_GET_VERSION         = 0x00;
  static constexpr uint8_t CMD_HEARTBEAT           = 0x01;
  static constexpr uint8_t CMD_SET_AXIS_STATE      = 0x07;
  static constexpr uint8_t CMD_SET_INPUT_POS       = 0x0C;
  static constexpr uint8_t CMD_GET_ENCODER_EST     = 0x09;

  // startup state
  static constexpr uint8_t SEEN_HB  = 0x01;
  static constexpr uint8_t SEEN_ENC = 0x02;
  static constexpr uint8_t SEEN_VER = 0x04;

  struct AxisCache {
    volatile float pos_turns = 0.0F;
    volatile float vel_turns_s = 0.0F;
    volatile uint32_t last_hb_ms = 0;
    volatile uint32_t last_err = 0;
    volatile uint8_t axis_state = 0;
    volatile uint32_t last_enc_ms = 0;
    volatile uint32_t last_ver_ms = 0;
    volatile uint8_t  ver_raw[8] = {0};
    volatile uint16_t proto_ver = 0;
    volatile uint8_t  fw_major = 0, fw_minor = 0, fw_rev = 0;
    volatile uint8_t  hw_variant = 0, hw_version = 0;
    volatile uint8_t seen = 0;
  };

  static constexpr uint8_t MaxNodes = 2;
  AxisCache cache_[MaxNodes];
};

extern ODriveCanPlus odriveCan;

#endif
#endif
