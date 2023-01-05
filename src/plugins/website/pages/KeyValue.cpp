// key/value handling
#include <Arduino.h>
#include "KeyValue.h"

String keyValueString(String key, String value1, String value2, String value3, String value4) {
  return key + "|" + value1 + value2 + value3 + value4 + "\n";
}

String keyValueToggleBoolSelected(String keyOn, String keyOff, bool selectState) {
  return keyOn + "|" + (selectState ? "selected\n" : "unselected\n") + keyOff + "|" + (selectState ? "unselected\n" : "selected\n");
}

String keyValueBoolSelected(String key, bool selectState) {
  return key + "|" + (selectState ? "selected\n" : "unselected\n");
}

String keyValueBoolEnabled(String key, bool state) {
  return key + "|" + (state ? "enabled\n" : "disabled\n");
}
