#ifndef TPPUTILS_H_INCLUDE
#define TPPUTILS_H_INCLUDE

/***************************************************************************************************/
// TPPUtils.h
//  Routines of general use
//
//  Use of this software is subject to the Terms of Use which can be found at:
//  https://github.com/TeamPracticalProjects/SISProject/blob/master/SISDocs/Terms_of_Use_License_and_Disclaimer.pdf
//
//  1/20/2017
//
//  (c) 2015, 2016, 2017 by Bob Glicksman and Jim Schrempp
/***************************************************************************************************/
#include "application.h"

// parse string into an array of strings based on comma delimiter
// NOTE: result returned via global array
int parser(String source);

// blink the D7 LED without blocking
boolean nbBlink(byte numBlinks, unsigned long blinkTime);

// make a JSON element of "name":"value"
String makeNameValuePair(String name, String value);
String makeNameValuePairLong(String name, long value);
String makeNameValuePairFloat(String name, float value);

#endif  // end of header duplication prevention
