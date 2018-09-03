/***************************************************************************************************/
// WSMGlobals.cpp
//  Declares all global variables used by the Well System Monitor firmware. Note that any variables
//  declared in this file are defined in the file WSMGlobals.h
//
//  Use of this software is subject to the Terms of Use which can be found at:
//  https://github.com/TeamPracticalProjects/SISProject/blob/master/SISDocs/Terms_of_Use_License_and_Disclaimer.pdf
//
//  9/2/2018
//
//  (c) 2015, 2016, 2017, 2018 by Bob Glicksman and Jim Schrempp
/***************************************************************************************************/
#include <WSMGlobals.h>

// array to hold parsed substrings from a command string
// NOTE: used by parser in SISUtilities.
// XXX we should eventually eliminate this. See note in Parser routine header.
String g_dest[MAX_SUBSTRINGS];

// this variable is exposed to the cloud
char cloudDebug[80];    // used when debugging to give the debug client a message
