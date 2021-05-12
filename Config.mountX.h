/* ---------------------------------------------------------------------------------------------------------------------------------
 * Configuration for OnStepX infrequently used eXtended mount options
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

// MOUNT COORDINATES ------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#MOUNT_TYPE
#define MOUNT_COORDS          TOPOCENTRIC // TOPOCENTRIC, Applies refraction to coordinates to/from OnStep, except exactly    Infreq
                                          //              at the poles. Use TOPO_STRICT to apply refraction even in that case.
                                          //              Use OBSERVED_PLACE for no refraction.

// GUIDING BEHAVIOUR ---------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#GUIDING
#define SEPARATE_PULSE_GUIDE_RATE      ON //          ON, Uses a separate rate (stored in NV) for pulse guiding               Infreq

// ALIGN ---------------------------------------------------------------------------------------------------------------------------
#define ALIGN_MAX_STARS              AUTO //        AUTO, Uses HAL specified default (either 6 or 9 stars.)                   Infreq
                                          //              Or use n. Where n=1 (for Sync only) or 3 to 9 (for Goto Assist.)

// PEC  ----------------------------------------------------------------------------------------------------------------------------
#define PEC_BUFFER_SIZE_LIMIT         720 //         720, Seconds of PEC buffer allowed.                                      Adjust

// ---------------------------------------------------------------------------------------------------------------------------------
