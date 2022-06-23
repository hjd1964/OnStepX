// -----------------------------------------------------------------------------------
// axis step/dir TMC SPI motor driver

#include "TmcDrivers.h"

#if defined(TMC_UART_DRIVER_PRESENT) || defined(TMC_SPI_DRIVER_PRESENT)

bool TmcDriver::init(int model, int16_t mosi, int16_t sck, int16_t cs, int16_t miso, int16_t axisNumber) {
  active = false;
  this->model = model;

  #ifdef TMC_UART_DRIVER_PRESENT
    if (model == TMC2209U) {
      pinModeEx(mosi, OUTPUT);
      pinModeEx(sck, OUTPUT);

      tmcUartDriver = new TMC2209();
      if (tmcUartDriver == NULL) return false; 
      int deviceAddress = 0;
      int rx = miso;
      int tx = cs;

      delay(1);
      #if SERIAL_TMC == SoftSerial
        // pull MS1 and MS2 low for device address 0
        digitalWriteEx(mosi, LOW);
        digitalWriteEx(sck, LOW);

        #ifdef SERIAL_TMC_NO_RX
          rx = OFF;
        #endif
        VF("MSG: TmcDriver, software serial UART driver pins rx="); V(rx); VF(", tx="); V(tx); VF(", baud="); V(SERIAL_TMC_BAUD); VLF("bps");
        tmcUartDriver->setup(SERIAL_TMC_BAUD, deviceAddress, rx, tx);
      #else
        // help user hard code the device addresses 0,1,2,3
        digitalWriteEx(mosi, HIGH);
        digitalWriteEx(sck, HIGH);

        rx = SERIAL_TMC_RX;
        tx = SERIAL_TMC_TX;

        #ifdef DRIVER_UART_ADDRESS_REMAP_AXIS5
          if (deviceAddress == 4) deviceAddress = 2;
        #endif
        #ifdef DRIVER_UART_ADDRESS_REMAP
          deviceAddress = DRIVER_UART_ADDRESS_REMAP(axisNumber - 1);
        #else
          deviceAddress = axisNumber - 1;
        #endif

        VF("MSG: TmcDriver, hardware serial UART driver pins rx="); V(rx); VF(", tx="); V(tx); VF(", baud="); V(SERIAL_TMC_BAUD); VLF("bps");
        #if SERIAL_TMC_INVERT == ON
          tmcUartDriver->setup(SERIAL_TMC, SERIAL_TMC_BAUD, deviceAddress, rx, tx, true);
        #else
          tmcUartDriver->setup(SERIAL_TMC, SERIAL_TMC_BAUD, deviceAddress, rx, tx);
        #endif
      #endif

      if (rx != OFF) {
        if (tmcUartDriver->isSetupAndCommunicating()) VLF("MSG: TmcDriver, UART driver found"); else VLF("WRN: TmcDriver, UART driver detection failed");
      }

      tmcUartDriver->enable();
      tmcUartDriver->moveUsingStepDirInterface();
      tmcUartDriver->setPwmOffset(pc_pwm_ofs);
      tmcUartDriver->setPwmGradient(pc_pwm_grad);
      if (pc_pwm_auto) tmcUartDriver->enableAutomaticCurrentScaling();

      active = true;
      return true;
    } else
  #endif

  #ifdef TMC_SPI_DRIVER_PRESENT
    if (model == TMC5160) rsense = TMC5160_DRIVER_RSENSE; else
    if (model == TMC2130) rsense = TMC2130_DRIVER_RSENSE; else rsense = TMC2130_DRIVER_RSENSE;
    VF("MSG: TmcDriver, init RSENSE="); VL(rsense);

    if (model == TMC2130 || model == TMC5160) {
      active = softSpi.init(mosi, sck, cs, miso);
      return true;
    } else
  #endif

  return false;
}

bool TmcDriver::mode(bool intpol, int decay_mode, byte micro_step_code, int irun, int ihold) {
  if (!active) return false;

  #ifdef TMC_UART_DRIVER_PRESENT
    if (model == TMC2209U) {
      if (decay_mode == STEALTHCHOP) tmcUartDriver->enableStealthChop(); else tmcUartDriver->disableStealthChop();

      int microStepCode = MicroStepCodeToMode[micro_step_code];
      if (last_microStepCode != microStepCode) tmcUartDriver->setMicrostepsPerStep(microStepCode);
      last_microStepCode = microStepCode;

      if (last_irun != irun) tmcUartDriver->setRunCurrent(irun/25);
      last_irun = irun;

      if (last_ihold != ihold) tmcUartDriver->setHoldCurrent(ihold/25);
      last_ihold = ihold;

      return true;
    } else
  #endif

  #ifdef TMC_SPI_DRIVER_PRESENT
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
  #endif

  return false;
}

bool TmcDriver::error() {
  if (!active) return false;

  #ifdef TMC_UART_DRIVER_PRESENT
    if (model == TMC2209U) {
      return false;
    } else
  #endif

  #ifdef TMC_SPI_DRIVER_PRESENT
    if (model == TMC2130 || model == TMC5160) {
      softSpi.begin();

      // get global status register, look for driver error bit
      uint32_t data_out = 0;
      //uint8_t result=read(REG_GSTAT,&data_out);
      uint8_t result = read(REG_DRVSTATUS,&data_out);
      
      softSpi.end();
      if ((result & 2) != 0 || (result == 0 && data_out == 0)) return true; else return false;
    } else
  #endif

  return true;
}

bool TmcDriver::refresh_CHOPCONF(byte micro_step_code) {
  if (!active) return false;

  #ifdef TMC_UART_DRIVER_PRESENT
    if (model == TMC2209U) {
      int microStepCode = MicroStepCodeToMode[micro_step_code];
      if (last_microStepCode != microStepCode) tmcUartDriver->setMicrostepsPerStep(microStepCode);
      last_microStepCode = microStepCode;

      return true;
    } else
  #endif

  #ifdef TMC_SPI_DRIVER_PRESENT
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
  #endif

  return false;
}

uint32_t TmcDriver::read_CHOPCONF() {
  if (!active) return false;

  #ifdef TMC_UART_DRIVER_PRESENT
    if (model == TMC2209U) {
      return 0;
    } else
  #endif

  #ifdef TMC_SPI_DRIVER_PRESENT
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
  #endif

  return 0;
}  

int TmcDriver::refresh_DRVSTATUS() {
  if (!active) return false;

  #ifdef TMC_UART_DRIVER_PRESENT
    if (model == TMC2209U) {
      TMC2209::Status status = tmcUartDriver->getStatus();
      ds_stst       = (bool)status.standstill;
      ds_olb        = (bool)status.open_load_a;
      ds_ola        = (bool)status.open_load_b;
      ds_s2ga       = (bool)status.short_to_ground_a || (bool)status.low_side_short_a;
      ds_s2gb       = (bool)status.short_to_ground_b || (bool)status.low_side_short_b;
      ds_otpw       = (bool)status.over_temperature_warning;
      ds_ot         = (bool)status.over_temperature_shutdown;
      ds_stallguard = (bool)false;
      ds_cs_actual  = status.current_scaling;
      ds_fs_active  = (bool)false;
      ds_result     = 0;
      // status.over_temperature_120c
      // status.over_temperature_143c
      // status.over_temperature_150c
      // status.over_temperature_157c
      // status.stealth_chop_mode
      return 1;
    } else
  #endif

  #ifdef TMC_SPI_DRIVER_PRESENT
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
  #endif

  return 0;
}

bool TmcDriver::refresh_COOLCONF() {
  if (!active) return false;

  #ifdef TMC_UART_DRIVER_PRESENT
    if (model == TMC2209U) {
      return false;
    } else
  #endif

  #ifdef TMC_SPI_DRIVER_PRESENT
    if (model == TMC2130 || model == TMC5160) {
      softSpi.begin();
      
      uint32_t data_out = (cl_semin<<0)+(cl_seup<<5)+(cl_semax<<8)+(cl_sedn<<13)+(cl_seimin<<15)+(cl_sgt<<16)+(cl_sfilt<<24);
      write(REG_COOLCONF, data_out);
      
      softSpi.end();
      return true;
    } else
  #endif

  return false;
}

#ifdef TMC_SPI_DRIVER_PRESENT
  uint8_t TmcDriver::write(byte Address, uint32_t data_out) {
    Address = Address | 0x80;
    uint8_t status_byte = softSpi.transfer(Address);
    softSpi.transfer32(data_out);
    return status_byte;
  }

  uint8_t TmcDriver::read(byte Address, uint32_t* data_out) {
    Address = Address & ~0x80;
    uint8_t status_byte = softSpi.transfer(Address);
    *data_out = softSpi.transfer32(*data_out);
    return status_byte;
  }
#endif

#endif
