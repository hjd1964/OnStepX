/* ---------------------------------------------------------------------------------------------------------------------------------
 * Configuration for OnStepX infrequently used eXtended controller options
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
//      Parameter Name              Value        Default  Notes                                                                 Hint

// DEBUG ------------------------------------------------------------ see https://onstep.groups.io/g/main/wiki/6-Configuration#DEBUG
// Enable additional debugging and/or status messages on the specified SERIAL_DEBUG port
// Note that the SERIAL_DEBUG port cannot be used for normal communication with OnStep
#define DEBUG                         OFF //         OFF, Use ON for background error messages only, use VERBOSE for all      Infreq
                                          //              error and status messages, use CONSOLE for VT100 debug console,
                                          //              or use PROFILER for VT100 task profiler.
#define DEBUG_ECHO_COMMANDS           OFF //         OFF, Use ON to log command/responses to the debug serial port.           Option
#define SERIAL_DEBUG             SERIAL_A //    SERIAL_A, Or use any h/w serial port. Serial1 or Serial2, etc. as supported.  Option
#define SERIAL_DEBUG_BAUD            9600 //        9600, n. Where n=9600,19200,57600,115200 (common baud rates.)             Option

// NON-VOLATILE STORAGE ------------------------------------------------------------------------------------------------------------
#define NV_DEFAULT                     ON //          ON, Uses HAL specified default, for remembering settings when           Infreq
                                          //              powered off.  This is usually done using the microcontrollers
                                          //              built-in EEPROM or FLASH emulation of EEPROM.  Sometimes (Bluepill)
                                          //              it uses an DS3231 RTC's onboard EEPROM AT24C32 chip.

// ESP32 VIRTUAL SERIAL BLUETOOTH COMMAND CHANNEL ----------------------------------------------------------------------------------
#define SERIAL_BT_MODE                OFF //         OFF, Use SLAVE to enable the interface (ESP32 only.)                     Option
#define SERIAL_BT_NAME           "OnStep" //    "OnStep", Bluetooth device name.                                              Adjust

// ESP32 VIRTUAL SERIAL IP COMMAND CHANNEL -----------------------------------------------------------------------------------------
#define SERIAL_IP_MODE                OFF //         OFF, Use ACCESS_POINT or STATION to enable the interface (ESP32 only.)   Option

#define STANDARD_COMMAND_CHANNEL       ON //          ON, Wifi command channel for simultanious connections on port 9999      Option
#define PERSISTENT_COMMAND_CHANNEL     ON //          ON, Wifi command channel for a single connection on port 9998           Option

#define AP_SSID                 "ONSTEPX" //   "ONSTEPX", Wifi Access Point SSID.                                             Adjust
#define AP_PASSWORD            "password" //  "password", Wifi Access Point password.                                         Adjust
#define AP_CHANNEL                      7 //           7, Wifi Access Point channel.                                          Adjust
#define AP_IP_ADDR          (192,168,0,1) // ..,168,0,1), Wifi Access Point IP Address.                                       Adjust
#define AP_GW_ADDR          (192,168,0,1) // ..,168,0,1), Wifi Access Point GATEWAY Address.                                  Adjust
#define AP_SN_MASK        (255,255,255,0) // ..55,255,0), Wifi Access Point SUBNET Mask.                                      Adjust

#define STA_SSID                   "Home" //      "Home", Wifi Station SSID to connnect to.                                   Adjust
#define STA_PASSWORD           "password" //  "password", Wifi Station mode password.                                         Adjust
#define STA_IP_ADDR        (192,168,1,55) // ..168,1,55), Wifi Station IP Address.                                            Adjust
#define STA_GW_ADDR         (192,168,1,1) // ..,168,1,1), Wifi Station GATEWAY Address.                                       Adjust
#define STA_SN_MASK       (255,255,255,0) // ..55,255,0), Wifi Station SUBNET Mask.                                           Adjust

// EXTERNAL GPIO SUPPORT -----------------------------------------------------------------------------------------------------------
#define GPIO_DEVICE                   OFF //         OFF, DS2413 for 2-ch 1-wire GPIO, MCP23008 for 8-ch I2C GPIO, or         Option
                                          //              MCP23017 for 16-ch I2C GPIO. The device channels are assigned to
                                          //              pin#'s starting at 1000 and can be used for most OnStep features.

// ---------------------------------------------------------------------------------------------------------------------------------
