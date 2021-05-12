/* ---------------------------------------------------------------------------------------------------------------------------------
 * Configuration for OnStepX controller options
 *
 *          For more information on setting OnStep up see http://www.stellarjourney.com/index.php?r=site/equipment_onstep 
 *                      and join the OnStep Groups.io at https://groups.io/g/onstep
 * 
 *           *** Read the compiler warnings and errors, they are there to help guard against invalid configurations ***
 *
 * ---------------------------------------------------------------------------------------------------------------------------------
 * ADJUST THE FOLLOWING TO CONFIGURE YOUR CONTROLLER FEATURES ----------------------------------------------------------------------
 * <-Req'd = always must set, <-Often = usually must set, Option = optional, Adjust = adjust as req'd, Infreq = infrequently changed
*/
//      Parameter Name              Value   Default  Notes                                                                      Hint

// PINMAP ---------------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#PINMAP
#define PINMAP                        OFF //    OFF, Choose from: MiniPCB, MiniPCB2, MaxPCB2, MaxESP3, MaxSTM3,              <-Req'd
                                          //         MksGenL2, STM32Blue, FYSETC_S6_2.  Check Constants.h for more info.

// SERIAL PORT COMMAND CHANNELS ------------------------------------ see https://onstep.groups.io/g/main/wiki/6-Configuration#SERIAL
#define SERIAL_A_BAUD_DEFAULT        9600 //   9600, n. Where n=9600,19200,57600,115200 (common baud rates.)                  Infreq
#define SERIAL_B_BAUD_DEFAULT        9600 //   9600, n. See (src/HAL/) for your MCU Serial port # etc.                        Option
#define SERIAL_B_ESP_FLASHING         OFF //    OFF, ON Upload ESP8266 WiFi firmware through SERIAL_B with :ESPFLASH# cmd.    Option
#define SERIAL_C_BAUD_DEFAULT         OFF //    OFF, n. See (src/HAL/) for your MCU Serial port # etc.                        Option

// SENSORS -------------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#SENSORS
#define WEATHER                       OFF //    OFF, BME280 (I2C 0x77,) BME280_0x76, BME280_SPI (see pinmap for CS.)          Option
                                          //         BMP280 (I2C 0x77,) BMP280_0x76, BMP280_SPI (see pinmap for CS.)
                                          //         BME280 or BMP280 for temperature, pressure.  BME280 for humidity also.

#define TELESCOPE_TEMPERATURE         OFF //    OFF, DS1820, n. Where n is the DS1820 s/n for focuser temperature.            Adjust

// ---------------------------------------------------------------------------------------------------------------------------------
#define FileVersionConfig 5
