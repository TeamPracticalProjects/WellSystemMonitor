/*******************************************************************************
 * WSMAlertProcessor:  class to process WSM alerts
 * See: "WSM_Alert design" document for further details:   
 * https://docs.google.com/document/d/1pfs4bKGO6k_pArTxXKbhxDiTZ_W6TxbhMkOEr5PQL1w/edit?usp=sharing
 * 
 * By: Bob Glicksman, Jim Schrempp, Team Practical Projects
 * (c) 2022, Bob Glicksman, Jim Schrempp, Team Practical Projects
 * 
 * version 1.0: 8/9/2022.  Initial release
 * version 1.1: 8/23/22.  Fixed initialization bug 
 * 
 *******************************************************************************/
#ifndef wsmap
#define wsmap

#include "application.h"

class WSMAlertProcessor  {
    private:
        // Constants
        const float PP_ON_TOO_SHORT_LIMIT = 0.5; // PP should not run <= 1/2 minute
        const float PP_ON_TOO_LONG_LIMIT = 3.0;  // PP should not run >= 3 minutes
        const float WP_ON_TOO_SHORT_LIMIT = 20.0; // WP should not run <= 20 minutes
        const float WP_ON_TOO_LONG_LIMIT = 40.0; // WP should not run >= 40 minutes
        const float WP_RUN_TOO_SOON_LIMIT = 10.0;  // WP should not come on if total PP ontime <= 10 minutes
        const float WP_RUN_TOO_LONG_LIMIT = 30.0;  // WP should come on if total PP ontime >= 30 minutes
        const unsigned int ONE_DAY = 48; // one day = 24 hours = 48 half hour ticks
        const unsigned int THREE_DAYS = 144;  // three days = 3 x 48 = 144 half hour ticks

        // Variables
        float _ppAccumulatedOnTime; // accumulation of PP run imes
        unsigned int _timeBetweenPPevents;  // accumulation of ½ hour “ticks”
        unsigned int _ppAlertHoldoff;   // holdoff between new sms alerts for PP conditions.
        unsigned int _wpAlertHoldoff;   // holdoff between new sms alerts for WP conditions.
        unsigned int _interPumpAlertHoldoff;    // holdoff between new sms alerts for WP-PP conditions.
        unsigned int _interPPrunTime;   // holdoff between new sms alerts for inter PP condition.
        unsigned int _ppNotRunAlertHoldoff; // holdoff between new sms alerts for no PP condition.

        // Private methods (internal use only)
        void publishPPOnTooLongAlert(float onTime);     // alert #1
        void publishPPOnTooShortAlert(float onTime);    // alert #2
        void publishWPOnTooLongAlert(float onTime);     // alert #3
        void pubLishWPOnTooShortAlert(float onTime);    // alert #4
        void publishWPNotComeOnAlert(float accumulatedPPTime);  // alert #5
        void publishWPOnTooSoon(float accumulatedPPTime);   // alert #6
        void publishPPNotRun(unsigned int tickTime);    // alert #7

    public:
        // Constructor
        WSMAlertProcessor();

        // Initialization
        void begin();
        
        // Methods for generating alerts
        void halfHourTimeTick();    // called every ½ hour when publishTRH() is called
        void ppTurnedOn();  // called from the function publishPPchange(), if the PP has come on
        void ppTurnedOff(float runTime);    // called from the function publishPPchange(), 
                                            // if the PP has turned off.
        void wpTurnedOn();  // called from the function publishWPchange(), if the WP has come on.
        void wpTurnedOff(float runTime);    // called from the function publishWPchange(), 
                                            // if the WP has turned off.

        // Methods for testing purposes
        float get_ppAccumulatedOnTime(); 
        unsigned int get_timeBetweenPPevents();
        unsigned int get_ppAlertHoldoff();
        unsigned int get_wpAlertHoldoff();
        unsigned int get_interPumpAlertHoldoff();
        unsigned int get_interPPrunTime();
        unsigned int get_ppNotRunAlertHoldoff();
};

#endif
