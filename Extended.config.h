/* ---------------------------------------------------------------------------------------------------------------------------------
 * Extended configuration for OnStepX INFREQUENTLY USED controller options
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

// =================================================================================================================================
// CONTROLLER ======================================================================================================================

// DEBUG ------------------------------------------------------------ see https://onstep.groups.io/g/main/wiki/6-Configuration#DEBUG
// Enable additional debugging and/or status messages on the specified SERIAL_DEBUG port
// Note that the SERIAL_DEBUG port cannot be used for normal communication with OnStep
#define DEBUG                         OFF //    OFF, Use ON for background error messages only, use VERBOSE for all           Infreq
                                          //         error and status messages, use CONSOLE for VT100 debug console,
                                          //         or use PROFILER for VT100 task profiler.
#define DEBUG_SERVO                   OFF //    OFF, n. Where n=1 to 9 as the designated axis for logging servo activity.     Option
#define DEBUG_ECHO_COMMANDS           OFF //    OFF, Use ON to log command/responses to the debug serial port.                Option
#define SERIAL_DEBUG               Serial // Serial, Use any available h/w serial port. Serial1 or Serial2, etc.              Option
#define SERIAL_DEBUG_BAUD            9600 //   9600, n. Where n=9600,19200,57600,115200 (common baud rates.)                  Option

// ESP32 VIRTUAL SERIAL BLUETOOTH AND IP COMMAND CHANNELS --------------------------------------------------------------------------
#define SERIAL_BT_MODE                OFF //    OFF, Use SLAVE to enable the interface (ESP32 only.)                          Option
#define SERIAL_BT_NAME          "OnStepX" //         "OnStepX", Bluetooth device name.                                        Adjust
#define SERIAL_IP_MODE                OFF //    OFF, Use ACCESS_POINT or STATION to enable the interface (ESP32 only.)        Option

// EXTERNAL GPIO SUPPORT -----------------------------------------------------------------------------------------------------------
#define GPIO_DEVICE                   OFF //    OFF, DS2413 for 2-ch 1-wire GPIO, MCP23008 for 8-ch I2C GPIO, MCP23017 or     Option
                                          //         X9555 for a 16-ch I2C GPIO. Works with most OnStep features.
                                          //         Channels are assigned to pin#'s starting at 512 (for channel 0, etc.)

// UART STEP/DIR DRIVER SUPPORT ----------------------------------------------------------------------------------------------------
#define DRIVER_UART_HARDWARE_SERIAL   OFF //    OFF, Uses SoftwareSerial (transmit only) or ON for hardware serial.           Option

// =================================================================================================================================
// MOUNT ===========================================================================================================================

// AXIS1 RA/AZM -------------------------------- see https://onstep.groups.io/g/main/wiki/Configuration---Rotator-and-Focusers#AXIS1

// for SERVO_II and SERVO_SD driver models, PID and encoder settings:
#define AXIS1_SERVO_ENCODER        ENC_AB // ..._AB, Encoder type, ENC_AB, ENC_CW_CCW, ENC_PULSE_DIR, ENC_PULSE_ONLY.         Adjust
#define AXIS1_SERVO_P                 2.0 //    2.0, Proportional; scale of immediate response to position error.             Adjust 
#define AXIS1_SERVO_I                 5.0 //    5.0, Integral; rate of increasing response to position error over time.       Adjust
#define AXIS1_SERVO_D                 1.0 //    1.0, Derivative; overshoot supression.                                        Adjust
#define AXIS1_SERVO_P_GOTO            2.0 //    2.0, Proportional; as above except active only while slewing.                 Adjust 
#define AXIS1_SERVO_I_GOTO            5.0 //    5.0, Integral; as above except active only while slewing.                     Adjust
#define AXIS1_SERVO_D_GOTO            1.0 //    1.0, Derivative; ; as above except active only while slewing.                 Adjust

// AXIS2 DEC/ALT ------------------------------- see https://onstep.groups.io/g/main/wiki/Configuration---Rotator-and-Focusers#AXIS2

// for SERVO_II and SERVO_SD driver models, PID and encoder settings:
#define AXIS2_SERVO_ENCODER        ENC_AB // ..._AB, Encoder type, ENC_AB, ENC_CW_CCW, ENC_PULSE_DIR, ENC_PULSE_ONLY.         Adjust
#define AXIS2_SERVO_P                 2.0 //    2.0, Proportional; scale of immediate response to position error.             Adjust 
#define AXIS2_SERVO_I                 5.0 //    5.0, Integral; rate of increasing response to position error over time.       Adjust
#define AXIS2_SERVO_D                 1.0 //    1.0, Derivative; overshoot supression.                                        Adjust
#define AXIS2_SERVO_P_GOTO            2.0 //    2.0, Proportional; as above except active only while slewing.                 Adjust 
#define AXIS2_SERVO_I_GOTO            5.0 //    5.0, Integral; as above except active only while slewing.                     Adjust
#define AXIS2_SERVO_D_GOTO            1.0 //    1.0, Derivative; ; as above except active only while slewing.                 Adjust

// =================================================================================================================================
// ROTATOR =========================================================================================================================

// AXIS3 ROTATOR ------------------------------- see https://onstep.groups.io/g/main/wiki/Configuration---Rotator-and-Focusers#AXIS3

// for SERVO_II and SERVO_SD driver models, PID and encoder settings:
#define AXIS3_SERVO_ENCODER        ENC_AB // ..._AB, Encoder type, ENC_AB, ENC_CW_CCW, ENC_PULSE_DIR, ENC_PULSE_ONLY.         Adjust
#define AXIS3_SERVO_P                 2.0 //    2.0, Proportional; scale of immediate response to position error.             Adjust 
#define AXIS3_SERVO_I                 5.0 //    5.0, Integral; rate of increasing response to position error over time.       Adjust
#define AXIS3_SERVO_D                 1.0 //    1.0, Derivative; overshoot supression.                                        Adjust
#define AXIS3_SERVO_P_GOTO            2.0 //    2.0, Proportional; as above except active only while slewing.                 Adjust 
#define AXIS3_SERVO_I_GOTO            5.0 //    5.0, Integral; as above except active only while slewing.                     Adjust
#define AXIS3_SERVO_D_GOTO            1.0 //    1.0, Derivative; ; as above except active only while slewing.                 Adjust

// =================================================================================================================================
// FOCUSERS ========================================================================================================================

// AXIS4 FOCUSER 1 -------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#AXIS4

// for SERVO_II and SERVO_SD driver models, PID and encoder settings:
#define AXIS4_SERVO_ENCODER        ENC_AB // ..._AB, Encoder type, ENC_AB, ENC_CW_CCW, ENC_PULSE_DIR, ENC_PULSE_ONLY.         Adjust
#define AXIS4_SERVO_P                 2.0 //    2.0, Proportional; scale of immediate response to position error.             Adjust 
#define AXIS4_SERVO_I                 5.0 //    5.0, Integral; rate of increasing response to position error over time.       Adjust
#define AXIS4_SERVO_D                 1.0 //    1.0, Derivative; overshoot supression.                                        Adjust
#define AXIS4_SERVO_P_GOTO            2.0 //    2.0, Proportional; as above except active only while slewing.                 Adjust 
#define AXIS4_SERVO_I_GOTO            5.0 //    5.0, Integral; as above except active only while slewing.                     Adjust
#define AXIS4_SERVO_D_GOTO            1.0 //    1.0, Derivative; ; as above except active only while slewing.                 Adjust

// AXIS5 FOCUSER 2 -----------------------------------------------------------------------------------------------------------------
// Up to 6 focusers can be present (AXIS4 to AXIS9) simply copy the above text for focuser 1 and rename to AXIS5_ for focuser 2, etc

// ---------------------------------------------------------------------------------------------------------------------------------
