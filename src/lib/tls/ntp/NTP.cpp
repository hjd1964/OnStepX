// -----------------------------------------------------------------------------------------------------------------
// NTP time code
// from here: https://github.com/PaulStoffregen/Time

#include "NTP.h"

#if defined(TIME_LOCATION_SOURCE) && TIME_LOCATION_SOURCE == NTP

#include <TimeLib.h> // https://github.com/PaulStoffregen/Time/archive/master.zip

#if OPERATIONAL_MODE == WIFI
  #include <WiFiUdp.h>
  WiFiUDP Udp;
#else
  #include <EthernetUdp.h>
  EthernetUDP Udp;
#endif

#include "../../tasks/OnTask.h"

IPAddress timeServer = IPAddress TIME_IP_ADDR;

// local port to listen for UDP packets
unsigned int localPort = 8888;

TlsNTP *tlsNtp;

void ntpWrapper() {
  if (!tlsNtp->isReady()) tlsNtp->poll();
}

// initialize
bool TlsNTP::init() {
  tlsNtp = this;

  VF("MSG: TLS, start NTP monitor task (rate 5 min priority 7)... ");
  handle = tasks.add(5*60*1000L, 0, true, 7, ntpWrapper, "ntp");
  if (handle) {
    VLF("success");
    active = true;
  } else {
    VLF("FAILED!");
    active = false;
  }

  // flag that start time is unknown
  startTime = 0;

  ready = false;

  return active;
}

void TlsNTP::restart() {
}

void TlsNTP::set(JulianDate ut1) {
  ut1 = ut1;
}

void TlsNTP::set(int year, int month, int day, int hour, int minute, int second) {
  #ifdef TLS_TIMELIB
    setTime(hour, minute, second, day, month, year);
  #else
    (void)year; (void)month; (void)day; (void)hour; (void)minute; (void)second;
  #endif
}

bool TlsNTP::get(JulianDate &ut1) {
  if (!ready) return false;

  if (year() >= 0 && year() <= 3000 && month() >= 1 && month() <= 12 && day() >= 1 && day() <= 31 &&
      hour() <= 23 && minute() <= 59 && second() <= 59) {
    GregorianDate greg; greg.year = year(); greg.month = month(); greg.day = day();
    ut1 = calendars.gregorianToJulianDay(greg);
    ut1.hour = hour() + minute()/60.0 + (second() + DUT1)/3600.0;
  }

  return true;
}

void TlsNTP::poll() {
  Udp.begin(localPort);

  // discard any previously received packets
  unsigned long tOut = millis() + 3000L;
  while ((Udp.parsePacket() > 0) && ((long)(millis() - tOut) < 0)) Y;

  VLF("MSG: TLS, transmit NTP Request");
  sendNTPpacket(timeServer);

  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      VLF("MSG: TLS, receive NTP Response");
      // read packet into the buffer
      Udp.read(packetBuffer, NTP_PACKET_SIZE);
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      time_t ntpTime = secsSince1900 - 2208988800UL;
      setTime(ntpTime);
      DLF("MSG: TLS, next NTP query in 24 hours");
      tasks.setPeriod(handle, 24L*60L*60L*1000L);
      ready = true;

      Udp.stop();
      return;
    }
    Y;
  }
  DLF("MSG: TLS, no NTP Response :-(");
  DLF("MSG: TLS, next NTP query in 5 minutes");
  tasks.setPeriod(handle, 5*60*1000L);

  Udp.stop();
}

// send an NTP request to the time server at the given address
void TlsNTP::sendNTPpacket(IPAddress &address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

#endif
