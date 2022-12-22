//--------------------------------------------------------------------------------------------------
// telescope celestial object library, commands

#include "Library.h"

#if defined(MOUNT_PRESENT)

#include "../coordinates/Transform.h"
#include "../goto/Goto.h"

char const *ObjectStr[] = {"UNK", "OC", "GC", "PN", "DN", "SG", "EG", "IG", "KNT", "SNR", "GAL", "CN", "STR", "PLA", "CMT", "AST"};

bool Library::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  *supressFrame = false;

    //   L - Object Library Commands
    if (command[0] == 'L') {

      // :LB#       Find previous catalog object subject to the current constraints
      //            Returns: Nothing
      if (command[1] == 'B' && parameter[0] == 0) { 
        prevRec();
        *numericReply = false;
      } else 

      // :LC[n]#    Set current target object to catalog object number
      //            Returns: Nothing
      if (command[1] == 'C') {
        int16_t i;
        if (convert.atoi2(parameter, &i)) {
            if (i >= 0) {
            gotoRec(i);
            *numericReply = false;
            } else *commandError = CE_PARAM_RANGE;
        } else *commandError = CE_PARAM_FORM;
      } else 

      // :LI#       Get Object Information
      //            Returns: s# (string containing the current target object’s name and object type)
      if (command[1] == 'I' && parameter[0] == 0) {
        int i;
        Coordinate target;
        readVars(reply, &i, &target.r, &target.d);

        char const *objType = ObjectStr[i];
        strcat(reply, ",");
        strcat(reply, objType);
        *numericReply = false;
      } else 

      // :LIG#      Get catalog object information and goto target
      //            Returns: Nothing
      if (command[1] == 'I' && parameter[0] == 'G' && parameter[1] == 0) {
        int i;
        Coordinate target;
        readVars(reply, &i, &target.r, &target.d);
        goTo.setGotoTarget(&target);
        *numericReply = false;
      } else 

      // :LR#       Get catalog object information including RA and Dec, with advance to next record
      //            Returns: s# (string containing the current target object’s name, type, RA, and Dec)
      if (command[1] == 'R' && parameter[0] == 0) {
        int i;
        Coordinate target;
        readVars(reply, &i, &target.r, &target.d);
        goTo.setGotoTarget(&target);

        char const * objType = ObjectStr[i];
        char ws[20];

        strcat(reply, ",");
        strcat(reply, objType);
        if (strcmp(reply, ",UNK") != 0) {
          convert.doubleToHms(ws, radToHrs(target.r), false, PM_HIGH); strcat(reply, ","); strcat(reply, ws);
          convert.doubleToDms(ws, radToDeg(target.d), false, true, PM_HIGH); strcat(reply, ","); strcat(reply, ws);
        }

        nextRec();
        *numericReply = false;
      } else 

      // :LW[s]#    Write catalog object information including current target RA,Dec to next available empty record
      //            If at the end of the object list (:LI# command returns an empty string "#") a new item is automatically added
      //            [s] is a string of up to eleven chars followed by a comma and a type designation for ex. ":LWM31 AND,GAL#"
      //            Returns: 0 on failure (memory full, for example)
      //                     1 on success
      if (command[1] == 'W') {
        char name[12];
        char objType[4];

        // extract object name
        int l = 0;
        do {
          name[l] = 0;
          if (parameter[l] == ',') break;
          name[l] = parameter[l];
          name[l + 1] = 0;
          l++;
        } while (l < 12);

        // extract object type
        int16_t i = 0;
        if (parameter[l] == ',') {
          l++;
          int16_t m = 0;
          do {
            objType[m + 1] = 0;
            objType[m] = parameter[l];
            l++;
            m++;
          } while (parameter[l] != 0);
          // encode
          //   0      1      2      3      4      5      6      7      8      9     10     11     12     13     14     15    
          // "UNK",  "OC",  "GC",  "PN",  "DN",  "SG",  "EG",  "IG", "KNT", "SNR", "GAL",  "CN", "STR", "PLA", "CMT", "AST"
          for (l = 0; l <= 15; l++) { if (strcmp(objType, ObjectStr[l]) == 0) i = l; }
        }

        Coordinate target = goTo.getGotoTarget();
        if (firstFreeRec()) writeVars(name, i, target.r, target.d); else *commandError = CE_LIBRARY_FULL;
      } else 

      // :LN#       Find next catalog object subject to the current constraints
      //            Returns: Nothing
      if (command[1] == 'N' && parameter[0] == 0) { 
        nextRec();
        *numericReply = false;
      } else 

      // :L$#       Move to catalog name record
      //            Returns: 1
      if (command[1] == '$' && parameter[0] == 0) { 
        nameRec();
        *numericReply = true;
      } else 

      // :LD#       Clear current record
      //            Returns: Nothing
      if (command[1] == 'D' && parameter[0] == 0) { 
        clearCurrentRec();
        *numericReply = false;
      } else 

      // :LL#       Clear current catalog
      //            Returns: Nothing
      if (command[1] == 'L' && parameter[0] == 0) { 
        clearLib();
        *numericReply = false;
      } else 

      // :L!#       Clear library (all catalogs)
      //            Returns: Nothing
      if (command[1] == '!' && parameter[0] == 0) { 
        clearAll();
        *numericReply = false;
      } else 

      // :L?#       Get library free records (all catalogs)
      //            Returns: n#
      if (command[1] == '?' && parameter[0] == 0) { 
        sprintf(reply, "%ld", recFreeAll());
        *numericReply = false;
      } else 

      // :Lo[n]#    Select Library catalog by catalog number n
      //            Catalog number ranges from 0..14, catalogs 0..6 are user defined, the remainder are reserved
      //            Return: 0 on failure
      //                    1 on success
      if (command[1] == 'o') {
      int16_t i;
      if (convert.atoi2(parameter, &i)) {
        if (i >= 0 && i <= 14) {
          setCatalog(i);
        } else *commandError = CE_PARAM_RANGE;
      } else *commandError = CE_PARAM_FORM;
    } else *commandError = CE_CMD_UNKNOWN;

  } else return false;

  return true;
}

#endif
