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
#include <TPPUtils.h>
#include <WSMGlobals.h>

/*************************************** parser() **********************************************/
// parser(): parse a comma delimited string into its individual substrings.
//  Arguments:
//  	source:  String object to parse
//  Return: the number of substrings found and parsed out
//
//  This functions uses the following global constants and variables:
//  	const int MAX_SUBSTRINGS -- nominal value is 6
//  	String g_dest[MAX_SUBSTRINGS] -- array of Strings to hold the parsed out substrings

// XXX This should be made a generic routine that returns an array of strings so
//     that we don't have to use a global.

int parser(String source)
{
	int lastComma = 0;
	int presentComma = 0;

	//parse the string argument until there are no more substrings or until MAX_SUBSTRINGS
	//  are parsed out
	int index = 0;
	do
	{
    	presentComma = source.indexOf(',', lastComma);
    	if(presentComma == -1)
    	{
        	presentComma = source.length();
    	}
      g_dest[index++] = "" + source.substring(lastComma, presentComma);
    	lastComma = presentComma + 1;

	} while( (lastComma < source.length() ) && (index < MAX_SUBSTRINGS) );

	return (index);
}
/************************************ end of parser() ********************************************/


/************************************** nbBlink() ************************************************/
// nbBlink():  Blink the D7 LED without blocking.  Note: setup() must declare
//          	pinMode(D7, OUTPUT) in order for this function to work
//  Arguments:
//  	numBlinks:  the number of blinks for this function to implement
//  	blinkTime:  the time, in milliseconds, for the LED to be on or off
//
//  Return:  true if function is ready to be triggered again, otherwise false
//

boolean nbBlink(byte numBlinks, unsigned long blinkTime)
{
	const byte READY = 0;
	const byte LED_ON = 1;
	const byte LED_OFF = 2;
	const byte NUM_BLINKS = 3;


	static byte state = READY;
	static unsigned long lastTime;
	static unsigned long newTime;
	static byte blinks;

	newTime = millis();

	switch(state)
	{
    	case(READY):
        	digitalWrite(D7, HIGH); 	// turn the LED on
        	state = LED_ON;
        	lastTime = newTime;
        	blinks = numBlinks;
        	break;

    	case(LED_ON):
        	if( (newTime - lastTime) >= blinkTime) // time has expired
        	{
            	state = LED_OFF;
            	lastTime = newTime;
        	}
        	break;

    	case(LED_OFF):
        	digitalWrite(D7, LOW);  	// turn the LED off
        	if( (newTime - lastTime) >= blinkTime)
        	{
            	if(--blinks > 0) 	// another blink set is needed
            	{
                	digitalWrite(D7, HIGH);
                	state = LED_ON;
                	lastTime = newTime;
            	}
            	else
            	{
                	state = READY;
            	}

        	}
        	break;

    	default:
        	digitalWrite(D7, LOW);
        	state = READY;

	}

	if(state == READY)
	{
    	return true;
	}
	else
	{
    	return false;
	}
}

/*********************************** end of nbBlink() ********************************************/


/*********************************** makeNameValuePair() ********************************************/
// makeNameValuePair()
// parameters
//      String name  -  the "name"
//      String value -  the "value"
// return
//      a string of the format "name":"value" for use in making JSON
String makeNameValuePair(String name, String value)
{
	String nameValuePair = "";
	nameValuePair += DOUBLEQ + name + DOUBLEQ;
	nameValuePair += ":";
	nameValuePair += DOUBLEQ + value + DOUBLEQ;
	return nameValuePair;
}
/*********************************** end of makeNameValuePair() ********************************************/
