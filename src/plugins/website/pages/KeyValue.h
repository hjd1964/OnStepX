// key/value handling
#pragma once

String keyValueString(String key, String value1, String value2 = "", String value3 = "", String value4 = "");
String keyValueToggleBoolSelected(String keyOn, String keyOff, bool selectState);
String keyValueBoolSelected(String key, bool selectState);
String keyValueBoolEnabled(String key, bool state);
