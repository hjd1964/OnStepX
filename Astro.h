// -----------------------------------------------------------------------------------------------------------------------------
// Astronomy related functions

enum PrecisionMode {PM_LOW, PM_HIGH, PM_HIGHEST};

// string to int with error checking
bool atoi2(char *a, int *i, bool sign=true) {
  int len=strlen(a);
  if (len == 0 || len > 6) return false;
  for (int l=0; l < len; l++) {
    if ((l == 0) && ((a[l] == '+') || (a[l] == '-')) && sign) continue;
    if ((a[l] < '0') || (a[l] > '9')) return false; 
  }
  long l=atol(a);
  if ((l < -32767) || (l > 32768)) return false;
  *i=l;
  return true;
}

// string to float with error checking
bool atof2(char *a, double *d, bool sign=true) {
  int dc=0;
  int len=strlen(a);
  for (int l=0; l < len; l++) {
    if ((l == 0) && ((a[l] == '+') || (a[l] == '-')) && sign) continue;
    if (a[l] == '.') { if (dc == 0) { dc++; continue; } else return false; }
    if ((a[l] < '0') || (a[l] > '9')) return false;
  }
  *d=atof(a);
  return true;
}

// convert string in format MM/DD/YY to julian date
bool dateToDouble(double *JulianDay, char *date) {
  char m[3],d[3],y[3];
  int  m1,d1,y1;
  
  if (strlen(date) !=  8) return false;

  m[0]=*date++; m[1]=*date++; m[2]=0; if (!atoi2(m,&m1,false)) return false;
  if (*date++ != '/') return false; d[0]=*date++; d[1]=*date++; d[2]=0; if (!atoi2(d,&d1,false)) return false;
  if (*date++ != '/') return false; y[0]=*date++; y[1]=*date++; y[2]=0; if (!atoi2(y,&y1,false)) return false;
  if ((m1 < 1) || (m1 > 12) || (d1 < 1) || (d1 > 31) || (y1 < 0) || (y1 > 99)) return false;
  if (y1 > 11) y1=y1+2000; else y1=y1+2100;
  
  *JulianDay = transform.toJulian(y1,m1,d1);
  return true;
}

// convert string in format HH:MM:SS to double
// (also handles)           HH:MM.M
// (also handles)           HH:MM:SS
// (also handles)           HH:MM:SS.SSSS
bool hmsToDouble(double *f, char *hms, PrecisionMode p) {
  char h[3],m[5];
  int  h1,m1,m2=0;
  double s1=0;

  while (*hms == ' ') hms++; // strip prefix white-space

  if (strlen(hms) > 13) hms[13]=0; // limit maximum length
  int len=strlen(hms);
  
  if (p == PM_HIGHEST || p == PM_HIGH) { // validate length
    if (len != 8 && len < 10) return false;
  } else
  if (p == PM_LOW) {
    if (len != 7) return false;
  }

  // convert the hours part
  h[0]=*hms++; h[1]=*hms++; h[2]=0; if (!atoi2(h,&h1,false)) return false;

  // make sure the seperator is an allowed character, then convert the minutes part
  if (*hms++ != ':') return false;
  m[0]=*hms++; m[1]=*hms++; m[2]=0; if (!atoi2(m,&m1,false)) return false;

  if (p == PM_HIGHEST || p == PM_HIGH) {
    // make sure the seperator is an allowed character, then convert the seconds part
    if (*hms++ != ':') return false;
    if (!atof2(hms,&s1,false)) return false;
  } else
  if (p == PM_LOW) {
    // make sure the seperator is an allowed character, then convert the decimal minutes part
    if (*hms++ != '.') return false;
    m2=(*hms++)-'0';
  }
  
  if (h1 < 0 || h1 > 23 || m1 < 0 || m1 > 59 || m2 < 0 || m2 > 9 || s1 < 0 || s1 > 59.9999) return false;

  *f=(double)h1+(double)m1/60.0+(double)m2/600.0+s1/3600.0;
  return true;
}
bool hmsToDouble(double *f, char *hms) {
  if (!hmsToDouble(f,hms,PM_HIGHEST))
    if (!hmsToDouble(f,hms,PM_HIGH))
      if (!hmsToDouble(f,hms,PM_LOW)) return false;
  return true;
}

// convert double to string in a variety of formats (as above) 
void doubleToHms(char *reply, double *f, PrecisionMode p) {
  double h1,m1,f1,s1,sd=0;

  // round to 0.00005 second or 0.5 second, depending on precision mode
  if (p == PM_HIGHEST) f1=fabs(*f)+0.0000000139; else f1=fabs(*f)+0.000139;

  h1=floor(f1);
  m1=(f1-h1)*60.0;
  s1=(m1-floor(m1))*60.0;

  // finish off calculations for hms and form string template
  char s[]="%s%02d:%02d:%02d.%04d";
  if (p == PM_HIGHEST) {
    sd=(s1-floor(s1))*10000.0;
  } else
  if (p == PM_HIGH) {
    s[16]=0;
  } else
  if (p == PM_LOW) {
    s1=s1/6.0;
    s[11]='.'; s[14]='1'; s[16]=0;
  }

  // set sign and return result string
  char sign[2]="";
  if ((sd != 0 || s1 != 0 || m1 != 0 || h1 != 0) && *f < 0.0) strcpy(sign,"-");
  if (p == PM_HIGHEST) sprintf(reply,s,sign,(int)h1,(int)m1,(int)s1,(int)sd); else sprintf(reply,s,sign,(int)h1,(int)m1,(int)s1);
}

// convert string in format sDD:MM:SS to double
// (also handles)           sDD:MM:SS.SSS
//                          DDD:MM:SS
//                          sDD:MM
//                          DDD:MM
//                          sDD*MM
//                          DDD*MM
bool dmsToDouble(double *f, char *dms, bool sign_present, PrecisionMode p) {
  char d[4],m[5];
  int d1,m1,lowLimit=0,highLimit=360,len;
  double s1=0,sign=1;
  bool secondsOff=false;

  while (*dms == ' ') dms++; // strip prefix white-space
  if (strlen(dms) > 13) dms[13]=0; // maximum length
  len=strlen(dms);

  if (p == PM_HIGHEST || p == PM_HIGH) { // validate length
    if (len != 9 && len < 11) return false;
  } else
  if (p == PM_LOW) {
    if (len != 6) {
      if (len != 9) return false;
      secondsOff=false;
    } else secondsOff = true;
  }

  // determine if the sign was used and accept it if so, then convert the degrees part
  if (sign_present) {
    if (*dms == '-') sign=-1.0; else if (*dms == '+') sign=1.0; else return false; 
    dms++; d[0]=*dms++; d[1]=*dms++; d[2]=0; if (!atoi2(d,&d1,false)) return false;
  } else {
    d[0]=*dms++; d[1]=*dms++; d[2]=*dms++; d[3]=0; if (!atoi2(d,&d1,false)) return false;
  }

  // make sure the seperator is an allowed character, then convert the minutes part
  if (*dms != ':' && *dms != '*' && *dms != char(223)) return false; else dms++;
  m[0]=*dms++; m[1]=*dms++; m[2]=0; if (!atoi2(m,&m1,false)) return false;

  if ((p == PM_HIGHEST || p == PM_HIGH) && !secondsOff) {
    // make sure the seperator is an allowed character, then convert the seconds part
    if (*dms++ != ':' && *dms++ != '\'') return false;
    if (!atof2(dms,&s1,false)) return false;
  }

  if (sign_present) { lowLimit=-90; highLimit=90; }
  if ((d1 < lowLimit) || (d1 > highLimit) || (m1 < 0) || (m1 > 59) || (s1 < 0) || (s1 > 59.999)) return false;

  *f=sign*((double)d1+(double)m1/60.0+s1/3600.0);
  return true;
}
bool dmsToDouble(double *f, char *dms, bool sign_present) {
  if (!dmsToDouble(f,dms,sign_present,PM_HIGHEST))
    if (!dmsToDouble(f,dms,sign_present,PM_HIGH))
      if (!dmsToDouble(f,dms,sign_present,PM_LOW)) return false;
  return true;
}

// convert double to string in a variety of formats (as above) 
void doubleToDms(char *reply, double *f, bool fullRange, bool signPresent, PrecisionMode p) {
  char sign[]="+";
  int  o=0;
  double d1,m1,s1=0,s2,f1;

  // setup formatting, handle adding the sign
  f1=*f;
  if (f1 < 0) { f1=-f1; sign[0]='-'; }

  // round to 0.0005 arc-second or 0.5 arc-second, depending on precision mode
  if (p == PM_HIGHEST) f1=f1+0.000000139; else f1=f1+0.000139; 

  d1=floor(f1);
  m1=(f1-d1)*60.0;
  s1=(m1-floor(m1))*60.0;

  // finish off calculations for dms and form string template
  char s[]="+%02d*%02d:%02d.%03d";
  if (p == PM_HIGHEST) {
    s2=(s1-floor(s1))*1000.0;
  } else s[15]=0;

  if (signPresent) {
    if (sign[0] == '-') s[0]='-';
    o=1;
  } else memmove(&s[0],&s[1],strlen(s));

  if (fullRange) s[2+o]='3';
 
  // return result string
  if (p == PM_HIGHEST) {
    sprintf(reply,s,(int)d1,(int)m1,(int)s1,(int)s2);
  } else
  if (p == PM_HIGH) {
    sprintf(reply,s,(int)d1,(int)m1,(int)s1);
  } else
  if (p == PM_LOW) {
    s[9+o]=0;
    sprintf(reply,s,(int)d1,(int)m1);
  }
}
