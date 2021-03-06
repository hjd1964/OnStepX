// -------------------------------------------------------------------------------------------------
// Pin map for OnStep MaxSTM STM32F411CE/STM32F401CE PCB (or the Blackpill https://github.com/WeActTC/MiniF4-STM32F4x1)

// For an 8KB EEPROM on the MaxSTM3.6I
#if PINMAP == MaxSTM3I
  #define NV_M24C32
#endif

#if defined(STM32F411xE) || defined(STM32F401xC)

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define Aux0                   PB12             // Status LED
#define Aux2                   PA13             // PPS
#define Aux3                   PB13             // Home SW
#define Aux4                   PB14             // 1-Wire, Home SW
#define Aux5                   PA9              // TX1 
#define Aux6                   PA10             // RX1
#define Aux7                   PB15             // Limit SW
#define Aux8                   PA8              // Status2 LED, Reticle LED

// Misc. pins
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN         Aux4             // Default Pin for OneWire bus (note: this pin has a 0.1uF capacitor that must be removed for OneWire to function)
#endif
#define ADDON_GPIO0_PIN        PB0              // ESP8266 GPIO0 (shared with AXIS2_DIR_PIN)
#define ADDON_RESET_PIN        Aux2             // ESP8266 RST

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PEC_SENSE_PIN          PB1
#define PEC_ANALG_PIN          PB1              // PEC Sense, analog or digital

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define STATUS_LED1_PIN        Aux0             // Drain
#define STATUS_LED2_PIN        Aux8             // Drain
#define RETICLE_LED_PIN        Aux8             // Drain

// For a piezo buzzer
#define BUZZER_PIN             PA14             // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#ifdef MAXSTM_AUX0_PPS
  #define PPS_SENSE_PIN        Aux0             // PPS time source, GPS for example (MaxSTM version 3.6)
#else
  #define PPS_SENSE_PIN        Aux2             // PPS time source, GPS for example (MaxSTM version 3.61 and later)
#endif

// The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking
#define LIMIT_SENSE_PIN        Aux7

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN       OFF
#define AXIS1_M0_PIN           PA7              // SPI MOSI
#define AXIS1_M0_PINPORT       GPIOA
#define AXIS1_M0_PINBIT        GPIO_PIN_7
#define AXIS1_M1_PIN           PA5              // SPI SCK
#define AXIS1_M1_PINPORT       GPIOA
#define AXIS1_M1_PINBIT        GPIO_PIN_5
#define AXIS1_M2_PIN           PA1              // SPI CS
#define AXIS1_M2_PINPORT       GPIOA
#define AXIS1_M2_PINBIT        GPIO_PIN_1
#define AXIS1_M3_PIN           PA6              // SPI MISO
#define AXIS1_M3_PINPORT       GPIOA
#define AXIS1_M3_PINBIT        GPIO_PIN_6
#define AXIS1_STEP_PIN         PB10
#define Axis1_StpPORT          GPIOB
#define Axis1_StpBIT           GPIO_PIN_10
#define AXIS1_DIR_PIN          PB2
#define Axis1_DirPORT          GPIOB
#define Axis1_DirBIT           GPIO_PIN_2
#define AXIS1_DECAY_PIN        AXIS1_M2_PIN
#define AXIS1_FAULT_PIN        Aux1
#define AXIS1_HOME_SENSE_PIN   Aux3

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN       OFF
#define AXIS2_M0_PIN           PA7              // SPI MOSI
#define AXIS2_M0_PINPORT       GPIOA
#define AXIS2_M0_PINBIT        GPIO_PIN_7
#define AXIS2_M1_PIN           PA5              // SPI SCK
#define AXIS2_M1_PINPORT       GPIOA
#define AXIS2_M1_PINBIT        GPIO_PIN_5
#define AXIS2_M2_PIN           PA0              // SPI CS
#define AXIS2_M2_PINPORT       GPIOA
#define AXIS2_M2_PINBIT        GPIO_PIN_0
#define AXIS2_M3_PIN           PA6              // SPI MISO
#define AXIS2_M3_PINPORT       GPIOA
#define AXIS2_M3_PINBIT        GPIO_PIN_6
#define AXIS2_STEP_PIN         PA4
#define Axis2_StpPORT          GPIOA
#define Axis2_StpBIT           GPIO_PIN_4
#define AXIS2_DIR_PIN          PB0
#define Axis2_DirPORT          GPIOB
#define Axis2_DirBIT           GPIO_PIN_0
#define AXIS2_DECAY_PIN        AXIS2_M2_PIN
#define AXIS2_FAULT_PIN        Aux2
#define AXIS2_HOME_SENSE_PIN   Aux4

// For rotator stepper driver
#define AXIS3_ENABLE_PIN       OFF
#define AXIS3_M0_PIN           PA7              // SPI MOSI
#define AXIS3_M1_PIN           PA5              // SPI SCK
#define AXIS3_M2_PIN           PC15             // SPI CS
#define AXIS3_M3_PIN           PA6              // SPI MISO
#define AXIS3_STEP_PIN         PB8
#define AXIS3_DIR_PIN          PC13

// For focuser1 stepper driver
#define AXIS4_ENABLE_PIN       OFF
#define AXIS4_M0_PIN           PA7              // SPI MOSI
#define AXIS4_M1_PIN           PA5              // SPI SCK
#define AXIS4_M2_PIN           PC14             // SPI CS
#define AXIS4_M3_PIN           PA6              // SPI MISO
#define AXIS4_STEP_PIN         PB9
#define AXIS4_DIR_PIN          PC13

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN       OFF
#define AXIS5_M0_PIN           PA7              // SPI MOSI
#define AXIS5_M1_PIN           PA5              // SPI SCK
#define AXIS5_M2_PIN           PC15             // SPI CS
#define AXIS5_M3_PIN           PA6              // SPI MISO
#define AXIS5_STEP_PIN         PB8
#define AXIS5_DIR_PIN          PC13

// ST4 interface
#define ST4_RA_W_PIN           PA15             // ST4 RA- West
#define ST4_DEC_S_PIN          PB3              // ST4 DE- South
#define ST4_DEC_N_PIN          PB4              // ST4 DE+ North
#define ST4_RA_E_PIN           PB5              // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
