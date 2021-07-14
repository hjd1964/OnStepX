/* ---------------------------------------------------------------------------------------------------------------------------------
 * Configuration for OnStepX auxiliary feature options
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

// AUXILIARY FEATURE CONTROL ------------------------------ see https://onstep.groups.io/g/main/wiki/6-ConfigurationMaster#AUXILIARY
// Note: Temporarily set DEBUG mode to VERBOSE to list DS18B20 device serial numbers.

#define FEATURE1_PURPOSE              OFF //    OFF, SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER.                          Option
#define FEATURE1_NAME          "FEATURE1" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE1_TEMP                 OFF //    OFF, THERMISTOR, DS1820, n. Where n is the ds18b20 s/n. For DEW_HEATER temp.  Adjust
#define FEATURE1_PIN                  OFF //    OFF, AUX for auxiliary pin, n. Where n is the pin#.                           Adjust
#define FEATURE1_DEFAULT_VALUE        OFF //    OFF, ON, n. Where n=0..255 for ANALOG_OUT purpose.                            Adjust

#define FEATURE2_PURPOSE              OFF //    OFF, SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER.                          Option
#define FEATURE2_NAME          "FEATURE2" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE2_TEMP                 OFF //    OFF, THERMISTOR, DS1820, n. Where n is the ds18b20 s/n. For DEW_HEATER temp.  Adjust
#define FEATURE2_PIN                  OFF //    OFF, AUX for auxiliary pin, n. Where n is the pin#.                           Adjust
#define FEATURE2_DEFAULT_VALUE        OFF //    OFF, ON, n. Where n=0..255 for ANALOG_OUT purpose.                            Adjust

#define FEATURE3_PURPOSE              OFF //    OFF, SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER.                          Option
#define FEATURE3_NAME          "FEATURE3" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE3_TEMP                 OFF //    OFF, THERMISTOR, DS1820, n. Where n is the ds18b20 s/n. For DEW_HEATER temp.  Adjust
#define FEATURE3_PIN                  OFF //    OFF, AUX for auxiliary pin, n. Where n is the pin#.                           Adjust
#define FEATURE3_DEFAULT_VALUE        OFF //    OFF, ON, n. Where n=0..255 for ANALOG_OUT purpose.                            Adjust

#define FEATURE4_PURPOSE              OFF //    OFF, SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER.                          Option
#define FEATURE4_NAME          "FEATURE4" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE4_TEMP                 OFF //    OFF, THERMISTOR, DS1820, n. Where n is the ds18b20 s/n. For DEW_HEATER temp.  Adjust
#define FEATURE4_PIN                  OFF //    OFF, AUX for auxiliary pin, n. Where n is the pin#.                           Adjust
#define FEATURE4_DEFAULT_VALUE        OFF //    OFF, ON, n. Where n=0..255 for ANALOG_OUT purpose.                            Adjust

#define FEATURE5_PURPOSE              OFF //    OFF, SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER.                          Option
#define FEATURE5_NAME          "FEATURE5" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE5_TEMP                 OFF //    OFF, THERMISTOR, DS1820, n. Where n is the ds18b20 s/n. For DEW_HEATER temp.  Adjust
#define FEATURE5_PIN                  OFF //    OFF, AUX for auxiliary pin, n. Where n is the pin#.                           Adjust
#define FEATURE5_DEFAULT_VALUE        OFF //    OFF, ON, n. Where n=0..255 for ANALOG_OUT purpose.                            Adjust

#define FEATURE6_PURPOSE              OFF //    OFF, SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER.                          Option
#define FEATURE6_NAME          "FEATURE6" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE6_TEMP                 OFF //    OFF, THERMISTOR, DS1820, n. Where n is the ds18b20 s/n. For DEW_HEATER temp.  Adjust
#define FEATURE6_PIN                  OFF //    OFF, AUX for auxiliary pin, n. Where n is the pin#.                           Adjust
#define FEATURE6_DEFAULT_VALUE        OFF //    OFF, ON, n. Where n=0..255 for ANALOG_OUT purpose.                            Adjust

#define FEATURE7_PURPOSE              OFF //    OFF, SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER.                          Option
#define FEATURE7_NAME          "FEATURE7" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE7_TEMP                 OFF //    OFF, THERMISTOR, DS1820, n. Where n is the ds18b20 s/n. For DEW_HEATER temp.  Adjust
#define FEATURE7_PIN                  OFF //    OFF, AUX for auxiliary pin, n. Where n is the pin#.                           Adjust
#define FEATURE7_DEFAULT_VALUE        OFF //    OFF, ON, n. Where n=0..255 for ANALOG_OUT purpose.                            Adjust

#define FEATURE8_PURPOSE              OFF //    OFF, SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER.                          Option
#define FEATURE8_NAME          "FEATURE8" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE8_TEMP                 OFF //    OFF, THERMISTOR, DS1820, n. Where n is the ds18b20 s/n. For DEW_HEATER temp.  Adjust
#define FEATURE8_PIN                  OFF //    OFF, AUX for auxiliary pin, n. Where n is the pin#.                           Adjust
#define FEATURE8_DEFAULT_VALUE        OFF //    OFF, ON, n. Where n=0..255 for ANALOG_OUT purpose.                            Adjust

// ---------------------------------------------------------------------------------------------------------------------------------
