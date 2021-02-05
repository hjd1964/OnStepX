// Helper console for debugging

#if DEBUG == PROFILER
  void debugConsole() {
    EquCoordinate instrument, mount, observed, topocentric;
    HorCoordinate horizon;
  
    instrument.h=axis1.getInstrumentCoordinate();
    instrument.d=axis2.getInstrumentCoordinate();
  
    mount = transform.equInstrumentToMount(instrument);
  
    Serial.println("");
  
    observed = transform.equMountToObservedPlace(mount);
  
    transform.hourAngleToRightAscension(&observed);
    Serial.print("DATE/TIME = "); Serial.print("12/01/21"); Serial.print(" "); Serial.println("12:12:12");
    Serial.print("LST = "); Serial.println((observatory.getLAST()/SIDEREAL_RATIO)*3600, 1);
    Serial.print("RA  = "); Serial.println(radToDeg(observed.r), 4);
    Serial.print("HA  = "); Serial.println(radToDeg(observed.h), 4);
    Serial.print("Dec = "); Serial.println(radToDeg(observed.d), 4);
  
    horizon = transform.equToHor(instrument);
    Serial.print("Alt = "); Serial.println(radToDeg(horizon.a), 4);
    Serial.print("Azm = "); Serial.println(radToDeg(horizon.z), 4);

    Serial.print("\x1b[J");  // clear to end of screen
    Serial.print("\x1b[H");  // cursor to upper left
    Serial.print("\x1b[K");  // clear to end of line
  }
#endif
