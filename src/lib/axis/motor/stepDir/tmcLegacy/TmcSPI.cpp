// -----------------------------------------------------------------------------------
// Step/Dir TMC SPI motor driver

#include "TmcSPI.h"

#if !defined(DRIVER_TMC_STEPPER) && defined(STEP_DIR_TMC_SPI_PRESENT)

bool TmcSPI::init(int model, int16_t mosi, int16_t sck, int16_t cs, int16_t miso, int16_t axisNumber) {
  active = false;
  this->model = model;

  if (model == TMC2130 || model == TMC5160) {
    if (model == TMC5160) rsense = TMC5160_DRIVER_RSENSE; else
    if (model == TMC2130) rsense = TMC2130_DRIVER_RSENSE; else rsense = TMC2130_DRIVER_RSENSE;
    VF("MSG: TmcSPI, init RSENSE="); VL(rsense);

    active = softSpi.init(mosi, sck, cs, miso);
    return true;
  } else

  return false;
}

bool TmcSPI::mode(bool intpol, int decay_mode, byte micro_step_code, int irun, int ihold) {
  if (!active) return false;

  if (model == TMC2130 || model == TMC5160) {
    softSpi.begin();
    uint32_t data_out = 0;

    // *** My notes are limited, see the TMC2130 datasheet for more info. ***

    // IHOLDDELAY=0x00, IRUN=0x1F, IHOLD=0x1F (  0,   31,   31   ) or 50% (0,16,16)
    //                                         0b0000 11111 11111
    // IHOLD,      default=16, range 0 to 31 (Standstill current 0=1/32... 31=32/32)
    // IRUN,       default=31, range 0 to 31 (Run current 0=1/32... 31=32/32)
    // IHOLDDELAY, default=4,  range 0 to 15 (Delay per current reduction step in x 2^18 clocks)
    float Ifs = 0.325/rsense;
    long IHOLD = round(( (ihold/1000.0)/Ifs)*32.0)-1;
    long IRUN  = round(( (irun/1000.0)/Ifs)*32.0)-1;
    if (IHOLD < 0) IHOLD = 0;
    if (IHOLD > 31) IHOLD = 31;
    if (IRUN < 0) IRUN = 0;
    if (IRUN > 31) IRUN = 31;
    if (IHOLD == OFF) IHOLD = IRUN/2;

    //          IHOLD       +  IRUN       +  IHOLDDELAY
    data_out = (IHOLD << 0) + (IRUN << 8) + (4UL << 16);
    if (last_IHOLD_IRUN != data_out) {
      last_IHOLD_IRUN = data_out;
      write(REG_IHOLD_IRUN, data_out);
      softSpi.pause();
    }

    // TPOWERDOWN, default=127, range 0 to 255 (Delay after standstill for motor current power down, about 0 to 4 seconds)
    data_out = (tpd_value << 0);
    if (last_TPOWERDOWN != data_out) {
      last_TPOWERDOWN = data_out;
      write(REG_TPOWERDOWN, data_out);
      softSpi.pause();
    }

    // TPWMTHRS, default=0, range 0 to 2^20 (switchover upper velocity for stealthChop voltage PWM mode)
    data_out = (tpt_value << 0);
    if (last_TPWMTHRS != data_out) {
      last_TPWMTHRS = data_out;
      write(REG_TPWMTHRS, data_out);
      softSpi.pause();
    }

    // THIGH, default=0, range 0 to 2^20 (switchover rate for vhighfs/vhighchm)
    data_out = (thigh_value << 0);
    if (last_THIGH != data_out) {
      last_THIGH = data_out;
      write(REG_THIGH, data_out);
      softSpi.pause();
    }

    // PWMCONF
    if (model == TMC2130) {
      // default=0x00050480UL
      data_out = (pc_pwm_ampl<<0)+(pc_pwm_grad<<8)+(pc_pwm_freq<<16)+(pc_pwm_auto<<18)+(pc_pwm_sym<<19)+(pc_pwm_freewheel<<20);
      if (last_PWMCONF != data_out) {
        last_PWMCONF = data_out;
        write(REG_PWMCONF, data_out);
        softSpi.pause();
      }
    } else
    if (model == TMC5160) {
      // default=0xC40C001EUL
      data_out = (pc_pwm_ofs<<0)+(pc_pwm_grad<<8)+(pc_pwm_freq<<16)+(pc_pwm_auto<<18)+(pc_pwm_autograd<<19)+(pc_pwm_freewheel<<20)+(pc_pwm_reg<<24)+(pc_pwm_lim<<28);
      if (last_PWMCONF != data_out) {
        last_PWMCONF = data_out;
        write(REG_PWMCONF, data_out);
        softSpi.pause();
      }
    }

    // CHOPCONF
    if (intpol) cc_intpol=1; else cc_intpol=0; // set interpolation bit
    // default=0x00008008UL
    if (model == TMC2130) last_chop_config = (cc_toff<<0)+(cc_hstart<<4)+(cc_hend<<7)+(cc_rndtf<<13)+(cc_tbl<<15)+(cc_vsense<<17)+(cc_vhighfs<<18)+(cc_vhighchm<<19)+(cc_intpol<<28);
    // default=0x10410150UL
    if (model == TMC5160) last_chop_config = (cc_toff<<0)+(cc_hstart<<4)+(cc_hend<<7)+(cc_tbl<<15)+(cc_vhighfs<<18)+(cc_vhighchm<<19)+(cc_tpfd<<20)+(cc_intpol<<28);
    if (micro_step_code != 255) {
      data_out = last_chop_config + (((uint32_t)micro_step_code)<<24);
      write(REG_CHOPCONF, data_out);
      softSpi.pause();
    }

    // GCONF
    // voltage on AIN is current reference
    data_out = 0x00000001UL;
    // set stealthChop bit
    if (decay_mode == STEALTHCHOP) data_out |= 0x00000004UL;
    if (last_GCONF != data_out) {
      last_GCONF = data_out;
      write(REG_GCONF, data_out);
    }

    softSpi.end();
    return true;
  } else

  return false;
}

bool TmcSPI::error() {
  if (!active) return false;

  if (model == TMC2130 || model == TMC5160) {
    softSpi.begin();

    // get global status register, look for driver error bit
    uint32_t data_out = 0;
    //uint8_t result=read(REG_GSTAT,&data_out);
    uint8_t result = read(REG_DRVSTATUS,&data_out);
    
    softSpi.end();
    if ((result & 2) != 0 || (result == 0 && data_out == 0)) return true; else return false;
  } else

  return true;
}

bool TmcSPI::refresh_CHOPCONF(byte micro_step_code) {
  if (!active) return false;

  if (model == TMC2130 || model == TMC5160) {
    softSpi.begin();

    // default=0x00008008UL
    if (model == TMC2130) last_chop_config = (cc_toff<<0)+(cc_hstart<<4)+(cc_hend<<7)+(cc_rndtf<<13)+(cc_tbl<<15)+(cc_vsense<<17)+(cc_vhighfs<<18)+(cc_vhighchm<<19)+(cc_intpol<<28);
    // default=0x10410150UL
    if (model == TMC5160) last_chop_config = (cc_toff<<0)+(cc_hstart<<4)+(cc_hend<<7)+(cc_tbl<<15)+(cc_vhighfs<<18)+(cc_vhighchm<<19)+(cc_tpfd<<20)+(cc_intpol<<28);

    write(REG_CHOPCONF, last_chop_config + (((uint32_t)micro_step_code)<<24));
    softSpi.end();
    return true;
  } else

  return false;
}

uint32_t TmcSPI::read_CHOPCONF() {
  if (!active) return false;

  if (model == TMC2130 || model == TMC5160) {
    softSpi.begin();

    uint32_t data_out = 0;
    read(REG_CHOPCONF, &data_out);

    // first write returns nothing, second the data
    data_out = 0;
    read(REG_DRVSTATUS, &data_out);
    
    softSpi.end();
    return data_out;
  } else

  return 0;
}  

int TmcSPI::refresh_DRVSTATUS() {
  if (!active) return false;

  if (model == TMC2130 || model == TMC5160) {
    softSpi.begin();

    // get global status register, look for driver error bit
    uint32_t data_out=0;
    uint8_t  result = read(REG_DRVSTATUS, &data_out);

    softSpi.pause();
    
    // first write returns nothing, second the status data
    data_out=0;
    read(REG_DRVSTATUS, &data_out);

    // get the extended status info.
    if (data_out != 0 || result != 0) {
      ds_stst       = (bool)bitRead(data_out,31); // DRV_STATUS 31 Standstill
      ds_olb        = (bool)bitRead(data_out,30); // DRV_STATUS 30 Open Load B
      ds_ola        = (bool)bitRead(data_out,29); // DRV_STATUS 29 Open Load A
      ds_s2ga       = (bool)bitRead(data_out,28); // DRV_STATUS 28 Short to Ground B
      ds_s2gb       = (bool)bitRead(data_out,27); // DRV_STATUS 27 Short to Ground A
      ds_otpw       = (bool)bitRead(data_out,26); // DRV_STATUS 26 Overtemperature Pre-warning 120C
      ds_ot         = (bool)bitRead(data_out,25); // DRV_STATUS 25 Overtemperature Shutdown 150C
      ds_stallguard = (bool)bitRead(data_out,24); // DRV_STATUS 24 stallGuard2 status
      ds_cs_actual  = (data_out>>16) & 0b011111;  // DRV_STATUS 16 Actual current control scaling
      ds_fs_active  = (bool)bitRead(data_out,15); // DRV_STATUS 15 Full step active indicator
      ds_result     = data_out & 0b1111111111;    // DRV_STATUS  0 stallGuard2 result
    } else {
      ds_stst = true; ds_olb = true; ds_ola = true; ds_s2ga = true; ds_s2gb = true; ds_otpw = true;
      ds_ot = true; ds_stallguard = false; ds_cs_actual = 0; ds_fs_active = false; ds_result = 0;       
    }

    softSpi.end();
    return data_out != 0;
  } else

  return 0;
}

bool TmcSPI::refresh_COOLCONF() {
  if (!active) return false;

  if (model == TMC2130 || model == TMC5160) {
    softSpi.begin();
    
    uint32_t data_out = (cl_semin<<0)+(cl_seup<<5)+(cl_semax<<8)+(cl_sedn<<13)+(cl_seimin<<15)+(cl_sgt<<16)+(cl_sfilt<<24);
    write(REG_COOLCONF, data_out);
    
    softSpi.end();
    return true;
  } else

  return false;
}

uint8_t TmcSPI::write(byte Address, uint32_t data_out) {
  Address = Address | 0x80;
  uint8_t status_byte = softSpi.transfer(Address);
  softSpi.transfer32(data_out);
  return status_byte;
}

uint8_t TmcSPI::read(byte Address, uint32_t* data_out) {
  Address = Address & ~0x80;
  uint8_t status_byte = softSpi.transfer(Address);
  *data_out = softSpi.transfer32(*data_out);
  return status_byte;
}

#endif
