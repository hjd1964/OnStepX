// -----------------------------------------------------------------------------------------------------------------
// NTP time code
// from here: https://github.com/PaulStoffregen/Time
#pragma once

#include "../../../Common.h"

#if defined(TIME_LOCATION_SOURCE) && TIME_LOCATION_SOURCE == NTP

#if OPERATIONAL_MODE == WIFI
  #include <WiFiUdp.h>
#else
  #include <EthernetUdp.h>
#endif

#include "../TLS.h"

#ifndef NTP_TIMEOUT_SECONDS
  #define NTP_TIMEOUT_SECONDS 300 // wait up to 5 minutes to get date/time, use 0 to disable timeout
#endif

class TlsNTP : public TimeLocationSource {
  public:
    // initialize (also enables the RTC PPS if available)
    bool init();

    // restart UDP server
    void restart();

    // set the RTC's time
    void set(JulianDate ut1);
    void set(int year, int month, int day, int hour, int minute, int second);

    // get the RTC's time
    bool get(JulianDate &ut1);

    // update from NTP
    void poll();

    // for conversion from UTC to UT1
    double DUT1 = 0.0L;

  private:
    // send an NTP request to the time server at the given address
    void sendNTPpacket(IPAddress &address);

    // NTP time is in the first 48 bytes of message
    static const int NTP_PACKET_SIZE = 48;

    // buffer to hold incoming & outgoing packets
    uint8_t packetBuffer[NTP_PACKET_SIZE];

    unsigned long startTime = 0;

    bool active = false;
    uint8_t handle = 0;
    
};

#endif
