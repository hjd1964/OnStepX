// -----------------------------------------------------------------------------------
// Library help
#pragma once

#include "htmlHeaders.h"
#include "htmlMessages.h"
#include "htmlScripts.h"

const char html_libCatalogHelp1[] PROGMEM =
"Data format:<br /><pre>"
"Object Name Cat    RA       Dec   <br />"
"ccccccccccc,ccc,HH:MM:SS,sDD*MM:SS<br /></pre>"
"Sample data:<br /><pre>"
"$PlaNeb1<br />"
"M97        ,PN ,11:15:53,+54*55:24<br />"
"M27        ,PN ,20:00:26,+22*46:29<br />"
"</pre>"
L_CAT_EXAMPLE1 L_CAT_EXAMPLE2 L_CAT_EXAMPLE3 L_CAT_EXAMPLE4 L_CAT_EXAMPLE5
"  UNK = Unknown<br />"
"  OC  = Open Cluster<br />"
"  GC  = Globular Cluster<br />";
const char html_libCatalogHelp2[] PROGMEM =
"  PN  = Planetary Nebula<br />"
"  DN  = Dark Nebula<br />"
"  SG  = Spiral Galaxy<br />"
"  EG  = Elliptical Galaxy<br />"
"  IG  = Irregular Galaxy<br />"
"  KNT = Knot<br />"
"  SNR = Supernova Remnant<br />"
"  GAL = Galaxy<br />"
"  CN  = Cluster w/Nebula<br />"
"  STR = Star<br />"
"  PLA = Planet<br />"
"  CMT = Comet<br />"
"  AST = Asteroid<br /><br />"
L_CAT_EXAMPLE6 L_CAT_EXAMPLE7
"<br /></pre>\n";
