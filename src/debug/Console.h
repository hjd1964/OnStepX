// Helper console for debugging

#if DEBUG == CONSOLE
  void debugConsole() {
    EquCoordinate instrument, mount, observed, topocentric;
    HorCoordinate horizon;
  
    instrument.h=axis1.getInstrumentCoordinate();
    instrument.d=axis2.getInstrumentCoordinate();
  
    mount = transform.equInstrumentToMount(instrument);
  
    DL();
  
    observed = transform.equMountToObservedPlace(mount);
  
    transform.hourAngleToRightAscension(&observed);
    D("DATE/TIME = "); D("12/01/21"); D(" "); DL("12:12:12");
    D("LST = "); SERIAL_DEBUG.println((observatory.getLAST()/SIDEREAL_RATIO)*3600, 1);
    D("RA  = "); SERIAL_DEBUG.println(radToDeg(observed.r), 4);
    D("HA  = "); SERIAL_DEBUG.println(radToDeg(observed.h), 4);
    D("Dec = "); SERIAL_DEBUG.println(radToDeg(observed.d), 4);
  
    horizon = transform.equToHor(instrument);
    D("Alt = "); SERIAL_DEBUG.println(radToDeg(horizon.a), 4);
    D("Azm = "); SERIAL_DEBUG.println(radToDeg(horizon.z), 4);

    D("\x1b[J");  // clear to end of screen
    D("\x1b[H");  // cursor to upper left
    D("\x1b[K");  // clear to end of line
  }
#endif
