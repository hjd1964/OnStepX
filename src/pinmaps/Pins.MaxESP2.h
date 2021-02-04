// -------------------------------------------------------------------------------------------------
// Pin map for OnStep MaxESP Version 2.x (ESP32S)

#if defined(ESP32)

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define Aux1                    12               // pullup driver RST, SPI MISO, or Fault
#define Aux3                    21               // Home SW, I2C SDA
#define Aux4                    22               // Home SW, I2C SCL
#define Aux7                    39               // PPS, Limit SW, etc.
#define Aux8                    25               // 1-Wire, Status LED, Status2 LED, Reticle LED, Tone, etc.

// Misc. pins
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN          Aux8             // Default Pin for 1-Wire bus
#endif

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PEC_SENSE_PIN           36
#define PEC_ANALG_PIN           A0               // PEC Sense, analog or digital (GPIO36)

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define STATUS_LED1_PIN         Aux8             // Drain
#define STATUS_LED2_PIN         Aux8             // Drain
#define RETICLE_LED_PIN         Aux8             // Drain

// For a piezo buzzer
#define BUZZER_PIN              Aux8             // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PPS_SENSE_PIN           Aux7             // PPS time source, GPS for example

// The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking
#define LIMIT_SENSE_PIN         Aux7

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN        4
#define AXIS1_M0_PIN            13               // SPI MOSI
#define AXIS1_M1_PIN            14               // SPI SCK
#define AXIS1_M2_PIN            23               // SPI CS
#define AXIS1_M3_PIN            Aux1             // SPI MISO
#define AXIS1_STEP_PIN          18
#define AXIS1_DIR_PIN           19
#define AXIS1_DECAY_PIN         AXIS1_M2_PIN
#define AXIS1_FAULT_PIN         Aux1
#define AXIS1_HOME_SENSE_PIN    Aux3

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN        SHARED
#define AXIS2_M0_PIN            13               // SPI MOSI
#define AXIS2_M1_PIN            14               // SPI SCK
#define AXIS2_M2_PIN            5                // SPI CS
#define AXIS2_M3_PIN            Aux1             // SPI MISO
#define AXIS2_STEP_PIN          27
#define AXIS2_DIR_PIN           26
#define AXIS2_DECAY_PIN         AXIS2_M2_PIN
#define AXIS2_FAULT_PIN         Aux1
#define AXIS2_HOME_SENSE_PIN    Aux4

// For rotator stepper driver
#define AXIS3_ENABLE_PIN        0
#define AXIS3_M0_PIN            OFF              // SPI MOSI
#define AXIS3_M1_PIN            OFF              // SPI SCK
#define AXIS3_M2_PIN            OFF              // SPI CS
#define AXIS3_M3_PIN            OFF              // SPI MISO
#define AXIS3_STEP_PIN          2
#define AXIS3_DIR_PIN           15

// For focuser1 stepper driver
#define AXIS4_ENABLE_PIN        0
#define AXIS4_M0_PIN            OFF              // SPI MOSI
#define AXIS4_M1_PIN            OFF              // SPI SCK
#define AXIS4_M2_PIN            OFF              // SPI CS
#define AXIS4_M3_PIN            OFF              // SPI MISO
#define AXIS4_STEP_PIN          2
#define AXIS4_DIR_PIN           15

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN        OFF
#define AXIS5_M0_PIN            OFF              // SPI MOSI
#define AXIS5_M1_PIN            OFF              // SPI SCK
#define AXIS5_M2_PIN            OFF              // SPI CS
#define AXIS5_M3_PIN            OFF              // SPI MISO
#define AXIS5_STEP_PIN          OFF
#define AXIS5_DIR_PIN           OFF

// ST4 interface
#define ST4_RA_W_PIN            34               // ST4 RA- West
#define ST4_DEC_S_PIN           32               // ST4 DE- South
#define ST4_DEC_N_PIN           33               // ST4 DE+ North
#define ST4_RA_E_PIN            35               // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
