// -----------------------------------------------------------------------------------
// Setup Network
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

const char htmL_NETWORKSerial[] PROGMEM =
"<b>" L_NETWORK_PERFORMANCE ":</b><br/>"
"<form method='post' action='/net.htm'>"
L_NETWORK_CMD_TO ": <input style='width:4em' name='ccto' value='%d' type='number' min='100' max='300'> ms<br/>"
L_NETWORK_WWW_TO ": <input style='width:4em' name='wcto' value='%d' type='number' min='100' max='300'> ms<br/>"
"<button type='submit'>" L_UPLOAD "</button></form><br />\n";

const char htmL_NET_MAC[] PROGMEM =
"MAC: <input style='width:10em' name='%s' type='text' value='%s' maxlength='17' disabled><br/>";

const char htmL_NET_IP[] PROGMEM =
"<table><tr><td>" L_IP_ADDRESS ": </td><td>"
"<input name='%sip1' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='%sip2' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='%sip3' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='%sip4' value='%d' type='number' min='0' max='255'></td>";

const char htmL_NET_GW[] PROGMEM =
"<tr><td>" L_GATEWAY ": </td><td>"
"<input name='%sgw1' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='%sgw2' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='%sgw3' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='%sgw4' value='%d' type='number' min='0' max='255'></td>";

const char htmL_NET_SN[] PROGMEM =
"<tr><td>" L_SUBNET ": </td><td>"
"<input name='%ssn1' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='%ssn2' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='%ssn3' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='%ssn4' value='%d' type='number' min='0' max='255'></td></tr></table>";

#if OPERATIONAL_MODE == WIFI
  const char htmL_NETWORKSSID1[] PROGMEM =
  "<br/><b>" L_NETWORK_STA_TITLE ":</b><br/>"
  "<form method='post' action='/net.htm'>"
  "SSID: <input style='width:6em' name='stssid' type='text' value='%s' maxlength='31'>&nbsp;&nbsp;&nbsp;"
  L_NETWORK_PWD ": <input style='width:8em' name='stpwd' type='password' value='%s' minlength='8' maxlength='31'> (" L_NETWORK_PWD_MSG ")<br/>";

  const char htmL_NETWORKSSID2[] PROGMEM =
  L_NETWORK_EN_DHCP ": <input type='checkbox' name='stadhcp' value='1' %s> (" L_NETWORK_EN_DHCP_MSG ")<br/>"
  L_NETWORK_EN_STA ": <input type='checkbox' name='staen' value='1' %s><br/>"
  "<button type='submit'>" L_UPLOAD "</button></form><br />\n";

  const char htmL_NETWORKSSID3A[] PROGMEM =
  "<br/><b>" L_NETWORK_AP ":</b><br/>"
  "<form method='post' action='/net.htm'>"
  "SSID: <input style='width:6em' name='apssid' type='text' ";

  const char htmL_NETWORKSSID3B[] PROGMEM =
  "value='%s' maxlength='31'>&nbsp;&nbsp;&nbsp;"
  L_NETWORK_PWD ": <input style='width:8em' name='appwd' type='password' value='%s' minlength='8' maxlength='31'> " L_NETWORK_PWD_MSG "&nbsp;&nbsp;&nbsp;"
  L_NETWORK_CHA ": <input style='width:3em' name='apch' value='%d' type='number' min='1' max='11'><br/>";

  const char htmL_NETWORKSSID7[] PROGMEM =
  L_NETWORK_EN_AP_MODE ": <input type='checkbox' name='apen' value='1' %s> (Note: auto-enabled if Station Mode fails to connect)<br/>"
  "<button type='submit'>" L_UPLOAD "</button></form><br />\n";
#else
  const char htmL_NETWORK_ETH_BEG[] PROGMEM =
  "<br/><b>Ethernet:</b><br/>"
  "<form method='post' action='/net.htm'><br />\n";

  const char htmL_NETWORK_ETH_END[] PROGMEM =
  "<button type='submit'>" L_UPLOAD "</button></form><br />\n";
#endif

const char html_logout[] PROGMEM =
"<br/><b>" L_NETWORK_CONFIG_SECURITY ":</b><br/>"
"<form method='post' action='/net.htm'>"
L_NETWORK_PWD ": <input style='width:8em' name='webpwd' type='password' minlength='3' maxlength='31'> "
"<button type='submit'>" L_UPLOAD "</button></form>"
"<form method='post' action='/net.htm'>"
"<button type='submit' name='logout' value='1'>" L_LOGOUT "</button></form><br />\n";

const char html_reboot[] PROGMEM =
"<br/><br/><br/><br/><br/><form method='post' action='/net.htm'>"
"<b>" L_NETWORK_RESTART_MSG "</b><br/><br/>"
"<button type='submit'>" L_CONTINUE "</button>"
"</form><br/><br/><br/><br/>"
"\n";

const char html_login[] PROGMEM =
"<br/><form method='post' action='/net.htm'>"
"<br/>" L_NETWORK_TITLE "<br />"
"<input style='width:8em' name='login' type='password' minlength='3' maxlength='31'>"
"<button type='submit'>" L_OK "</button>"
"</form><br/><br/><br/>"
#if OPERATIONAL_MODE == WIFI
  L_NETWORK_ADVICE1 L_NETWORK_ADVICE2 L_NETWORK_ADVICE3 L_NETWORK_ADVICE4 L_NETWORK_ADVICE5 L_NETWORK_ADVICE6 L_NETWORK_ADVICE7 L_NETWORK_ADVICE8 L_NETWORK_ADVICE9 
  "<br/><br/>"
#endif
"\n";
