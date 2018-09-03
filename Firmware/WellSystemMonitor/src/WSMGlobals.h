#ifndef WSMGlOBALS_H_INCLUDE
#define WSMGlOBALS_H_INCLUDE
/***************************************************************************************************/
// WSMGlobals.h
//  Defines all global variables used by the Well System Monitor firmware. Note that any variables
//  defined in this file are declared in the file WSMGlobals.cpp
//
//  Use of this software is subject to the Terms of Use which can be found at:
//  https://github.com/TeamPracticalProjects/SISProject/blob/master/SISDocs/Terms_of_Use_License_and_Disclaimer.pdf
//
//  1/20/2017
//
//  (c) 2015, 2016, 2017 by Bob Glicksman and Jim Schrempp
/***************************************************************************************************/
#include "application.h"

// array to hold parsed substrings from a command string
// NOTE: used by parser in SISUtilities.
// XXX we should eventually eliminate this. See note in Parser routine header.
const int MAX_SUBSTRINGS = 6;   // the largest number of comma delimited substrings in a command string
extern String g_dest[];

// convenience constants
const String DOUBLEQ = "\"";

// Configuration constants


// this variable is exposed to the cloud
extern char cloudDebug[];    // used when debugging to give the debug client a message




#endif //prevent double includes
