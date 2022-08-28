// -----------------------------------------------------------------------------------
// Step/Dir TMC SPI motor driver
#pragma once

#include <Arduino.h>
#include "../../../../../Common.h"

#if !defined(DRIVER_TMC_STEPPER) && defined(STEP_DIR_TMC_SPI_PRESENT)

#include "../../../../softSpi/SoftSpi.h"

#ifndef TMC5160_DRIVER_RSENSE
  #define TMC5160_DRIVER_RSENSE (0.075)
#endif

#ifndef TMC2130_DRIVER_RSENSE
  #define TMC2130_DRIVER_RSENSE (0.11 + 0.02)
#endif

class TmcSPI {
  public:
    // setup SoftSpi or UART and driver model/Rsense
    bool init(int model, int16_t mosi, int16_t sck, int16_t cs, int16_t miso, int16_t axisNumber = 0);

    // TMC setup most common settings
    // 256x interpolation:   intpol
    // decay mode:           decay_mode (STEALTHCHOP or SPREADCYCLE)
    // microstepping mode:   micro_step_code (0=256x, 1=128x, 2=64x, 3=32x, 4=16x, 5=8x, 6=4x, 7=2x, 8=1x)
    // irun, ihold, rsense:  current in mA and sense resistor value
    bool mode(bool intpol, int decay_mode, byte micro_step_code, int irun, int ihold);

    // Check for TMC error from DRVSTATUS register
    bool error();
    int refresh_DRVSTATUS();
    inline bool get_DRVSTATUS_stst()             { return ds_stst;       }
    inline bool get_DRVSTATUS_olA()              { return ds_ola;        }
    inline bool get_DRVSTATUS_olB()              { return ds_olb;        }
    inline bool get_DRVSTATUS_s2gA()             { return ds_s2ga;       }
    inline bool get_DRVSTATUS_s2gB()             { return ds_s2gb;       }
    inline bool get_DRVSTATUS_otpw()             { return ds_otpw;       }
    inline bool get_DRVSTATUS_ot()               { return ds_ot;         }
    inline bool get_DRVSTATUS_stallguard()       { return ds_stallguard; }
    inline int  get_DRVSTATUS_cs_actual()        { return ds_cs_actual;  }
    inline bool get_DRVSTATUS_active()           { return ds_fs_active;  }
    inline int  get_DRVSTATUS_result()           { return ds_result;     }

    // Chopper configuration
    bool refresh_CHOPCONF(byte micro_step_code);
    uint32_t read_CHOPCONF();
    inline bool set_CHOPCONF_toff(int v)         { if (v >= 2 && v <= 15)      { cc_toff         = v; return true; } return false; }
    inline bool set_CHOPCONF_hstart(int v)       { if (v >= 0 && v <= 7)       { cc_hstart       = v; return true; } return false; }
    inline bool set_CHOPCONF_hend(int v)         { if (v >= 0 && v <= 15)      { cc_hend         = v; return true; } return false; }
    inline bool set_CHOPCONF_rndtf(int v)        { if (v >= 0 && v <= 1)       { cc_rndtf        = v; return true; } return false; }
    inline bool set_CHOPCONF_tbl(int v)          { if (v >= 0 && v <= 3)       { cc_tbl          = v; return true; } return false; }
    inline bool set_CHOPCONF_vsense(int v)       { if (v >= 0 && v <= 1)       { cc_vsense       = v; return true; } return false; }
    inline bool set_CHOPCONF_vhighfs(int v)      { if (v >= 0 && v <= 1)       { cc_vhighfs      = v; return true; } return false; }
    inline bool set_CHOPCONF_vhighchm(int v)     { if (v >= 0 && v <= 1)       { cc_vhighchm     = v; return true; } return false; }
    inline bool set_CHOPCONF_intpol(int v)       { if (v >= 0 && v <= 1)       { cc_intpol       = v; return true; } return false; }
    // TMC5160 specific
    inline bool set_CHOPCONF_tpfd(int v)         { if (v >= 0 && v <= 15)      { cc_tpfd         = v; return true; } return false; }
    volatile uint32_t last_chop_config;

    // Misc. configuration
    inline bool set_TPOWERDOWN_value(int v)      { if (v >= 0 && v <= 255)     { tpd_value       = v; return true; } return false; }
    inline bool set_TPWMTHRS_value(long v)       { if (v >= 0 && v <= 1048575) { tpt_value       = v; return true; } return false; }
    inline bool set_THIGH_value(long v)          { if (v >= 0 && v <= 1048575) { thigh_value     = v; return true; } return false; }

    // PWM/stealthChop configuration
    inline bool set_PWMCONF_PWM_GRAD(int v)      { if (v >= 0 && v <= 255)     { pc_pwm_grad     = v; return true; } return false; }
    inline bool set_PWMCONF_pwm_freq(int v)      { if (v >= 0 && v <= 3)       { pc_pwm_freq     = v; return true; } return false; }
    inline bool set_PWMCONF_pwm_auto(int v)      { if (v >= 0 && v <= 1)       { pc_pwm_auto     = v; return true; } return false; }
    inline bool set_PWMCONF_pwm_freewheel(int v) { if (v >= 0 && v <= 1)       { pc_pwm_freewheel= v; return true; } return false; }
    // TMC2130 specific
    inline bool set_PWMCONF_PWM_AMPL(int v)      { if (v >= 0 && v <= 255)     { pc_pwm_ampl     = v; return true; } return false; }
    inline bool set_PWMCONF_pwm_sym(int v)       { if (v >= 0 && v <= 1)       { pc_pwm_sym      = v; return true; } return false; }
    // TMC5160/5161 specific
    inline bool set_PWMCONF_PWM_OFS(int v)       { if (v >= 0 && v <= 255)     { pc_pwm_ofs      = v; return true; } return false; }
    inline bool set_PWMCONF_pwm_autograd(int v)  { if (v >= 0 && v <= 1)       { pc_pwm_autograd = v; return true; } return false; }
    inline bool set_PWMCONF_PWM_REG(int v)       { if (v >= 0 && v <= 15)      { pc_pwm_reg      = v; return true; } return false; }
    inline bool set_PWMCONF_PWM_LIM(int v)       { if (v >= 0 && v <= 15)      { pc_pwm_lim      = v; return true; } return false; }

    // Coolstep and stallguard configurarion
    bool refresh_COOLCONF();
    inline bool set_COOLCONF_semin(int v)        { if (v >= 0 && v <= 15)      { cl_semin = v; return true; } return false; }
    inline bool set_COOLCONF_seup(int v)         { if (v >= 0 && v <= 3)       { cl_seup  = v; return true; } return false; }
    inline bool set_COOLCONF_semax(int v)        { if (v >= 0 && v <= 15)      { cl_semax = v; return true; } return false; }
    inline bool set_COOLCONF_sedn(int v)         { if (v >= 0 && v <= 3)       { cl_sedn  = v; return true; } return false; }
    inline bool set_COOLCONF_seimin(int v)       { if (v >= 0 && v <= 1)       { cl_seimin= v; return true; } return false; }
    inline bool set_COOLCONF_sgt(int v)          { if (v >= -64 && v <= 63)    { cl_sgt= v+64; return true; } return false; }
    inline bool set_COOLCONF_sfilt(int v)        { if (v >= 0 && v <= 1)       { cl_sfilt = v; return true; } return false; }

  private:
    uint8_t write(byte Address, uint32_t data_out);
    uint8_t read(byte Address, uint32_t* data_out);

    // the write flag and various TMC SPI registers
    static const uint8_t WRITE          = 0x80;
    static const uint8_t REG_GCONF      = 0x00;
    static const uint8_t REG_GSTAT      = 0x01;
    static const uint8_t REG_IHOLD_IRUN = 0x10;
    static const uint8_t REG_TPOWERDOWN = 0x11;
    static const uint8_t REG_TPWMTHRS   = 0x13;
    static const uint8_t REG_THIGH      = 0x15;
    static const uint8_t REG_CHOPCONF   = 0x6C;
    static const uint8_t REG_COOLCONF   = 0x6D;
    static const uint8_t REG_DCCTRL     = 0x6E;
    static const uint8_t REG_DRVSTATUS  = 0x6F;
    static const uint8_t REG_PWMCONF    = 0x70;

    // keep track of what commands need to be sent
    int last_microStepCode = -1;
    int last_irun = -1;
    int last_ihold = -1;

    // keep track of what registers need to be updated
    uint32_t last_GCONF       = 0;
    uint32_t last_IHOLD_IRUN  = 0;
    uint32_t last_TPOWERDOWN  = 0;
    uint32_t last_TPWMTHRS    = 0;
    uint32_t last_THIGH       = 0;
    uint32_t last_PWMCONF     = 0;

    // CHOPCONF settings
    uint32_t cc_toff          = 4;    // default=4,   range 2 to 15 (Off time setting, slow decay phase)
    uint32_t cc_hstart        = 0;    // default=0,   range 0 to 7  (Hysteresis start 1, 2, ..., 8)
    uint32_t cc_hend          = 0;    // default=0,   range 0 to 15 (Hysteresis -3, -2, -1, 0, 1 ..., 12)
    uint32_t cc_rndtf         = 0;    // default=0,   range 0 to 1  (Enables small random value to be added to TOFF)
    uint32_t cc_tbl           = 1;    // default=1,   range 0 to 3  (for 6, 24, 36 or 54 clocks)
    uint32_t cc_vsense        = 0;    // default=0,   range 0 to 1  (0 for high sensitivity, 1 for low sensitivity @ 50% current setting)
    uint32_t cc_vhighfs       = 0;    // default=0,   range 0 to 1  (Enables switch to full-step when VHIGH (THIGH?) is exceeded)
    uint32_t cc_vhighchm      = 0;    // default=0,   range 0 to 1  (Enables switch to fast-decay mode VHIGH (THIGH?) is exceeded)
    uint32_t cc_intpol        = 1;    // default=1,   range 0 to 1  (Enables 256x interpolation)
    
    // TMC5160 specific
    uint32_t cc_tpfd          = 4UL;  // default=4,   range 0 to 15 (Passive fast decay time mid-range resonance dampening)

    // TPOWERDOWN settings
    uint32_t tpd_value        = 128;  // default=127, range 0 to 255  (Delay after standstill for motor current power down, about 0 to 4 seconds)

    // TPWMTHRS settings
    uint32_t tpt_value        = 0;    // default=0,   range 0 to 2^20 (Switchover upper velocity for stealthChop voltage PWM mode)

    // THIGH settings
    uint32_t thigh_value      = 0;    // default=0,   range 0 to 2^20 (Switchover rate for vhighfs/vhighchm)

    // PWMCONF settings
    uint32_t pc_pwm_grad      = 0x04; // default=4,   range 0 to 14  (PWM gradient scale using automatic current control)
    uint32_t pc_pwm_freq      = 0x01; // default=1,   range 0 to 3   (PWM frequency 0: fpwm=2/1024 fclk, 1: fpwm=2/683 fclk, 2: fpwm=2/512 fclk, 3: fpwm=2/410 fclk)
    uint32_t pc_pwm_auto      = 0x01; // default=1,   range 0 to 1   (PWM automatic current control 0: off, 1: on)
    uint32_t pc_pwm_freewheel = 0x01; // default=1,   range 0 to 3   (PWM freewheel 0: normal, 1: freewheel, 2:LS short, 3: HS short)
    // TMC2130 specific
    uint32_t pc_pwm_ampl      = 0x80; // default=128, range 0 to 255 (PWM amplitude or switch back amplitude if pwm_auto=1)
    uint32_t pc_pwm_sym       = 0x00; // default=0,   range 0 to 1   (PWM symmetric 0: value may change during cycle, 1: enforce; 0: disable autograd on TMC5160/5161)
    // TMC2209/TMC5160 specific
    uint32_t pc_pwm_ofs       = 0x1e; // default=30,  range 0 to 255 (PWM user defined amplitude offset related to full motor current)
    uint32_t pc_pwm_autograd  = 0x00; // default=0,   range 0 to 1   (PWM automatic grad control 0: off, 1: on)
    uint32_t pc_pwm_reg       = 0x04; // default=4,   range 0 to 15  (PWM maximum amplitude change per half-wave when using pwm autoscale)
    uint32_t pc_pwm_lim       = 0x0c; // default=12,  range 0 to 15  (PWM limit for PWM_SCALE_AUTO when switching back from spreadCycle to stealthChop)

    // DRVSTATUS settings
    bool     ds_stst          = false;
    bool     ds_olb           = false;
    bool     ds_ola           = false;
    bool     ds_s2ga          = false;
    bool     ds_s2gb          = false;
    bool     ds_ot            = false;
    bool     ds_otpw          = false;
    bool     ds_stallguard    = false;
    uint16_t ds_cs_actual     = 0;
    bool     ds_fs_active     = false;
    uint16_t ds_result        = 0;

    // COOLCONF settings
    uint32_t cl_semin         = 0;    // default=0, range 0 to 15  (Minimum stallGuard2 value for smart current control and smart current enable)
    uint32_t cl_seup          = 0;    // default=0, range 0 to 3   (Current up step width)
    uint32_t cl_semax         = 0;    // default=0, range 0 to 15  (stallGuard2 hysteresis value for smart current control)
    uint32_t cl_sedn          = 0;    // default=0, range 0 to 3   (Current down step speed)
    uint32_t cl_seimin        = 0;    // default=0, range 0 to 1   (Minimum current for scc)
    uint32_t cl_sgt           = 0;    // default=0, range 0 to 127 (stallGuard2 hysteresis value -64 to 63)
    uint32_t cl_sfilt         = 0;    // default=0, range 0 to 1   (stallGuard2 filter enable)

    bool  active              = false;
    int   model;
    float rsense              = 0.11 + 0.02; // default for TMC2130

    SoftSpi softSpi;
};

#endif
