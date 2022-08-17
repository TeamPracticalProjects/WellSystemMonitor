/*******************************************************************************
 * WSMAlertProcessor:  class to process WSM alerts
 * See: "WSM_Alert design" document for further details:   
 * https://docs.google.com/document/d/1pfs4bKGO6k_pArTxXKbhxDiTZ_W6TxbhMkOEr5PQL1w/edit?usp=sharing
 * 
 * By: Bob Glicksman, Jim Schrempp, Team Practical Projects
 * (c) 2022, Bob Glicksman, Jim Schrempp, Team Practical Projects
 * 
 * version 1.0: 8/1/2022.  Initial release
 * version 1.1: 8/5/2022.  Fixed up published string format
 * version 1.2: 8/9/2022.  Completed unit testing and verified all alerts and holdoffs appear to work.
 * 
 *******************************************************************************/
#include <WSMAlertProcessor.h>

// Constructor
WSMAlertProcessor::WSMAlertProcessor() {
    // follow convention and put all initializations in begin() method   
}   // end of Constructor

// Initialization
void WSMAlertProcessor::begin() {
    // initialize all of the internal variables.

    // initialize accumulators to zero
    _ppAccumulatedOnTime = 0.0; // accumulation of PP run imes
    _timeBetweenPPevents = 0;  // accumulation of ½ hour “ticks”

    // initialize holdoff to max values so that first alerts will happen
    _ppAlertHoldoff = ONE_DAY;   // holdoff between new sms alerts for PP conditions.
    _wpAlertHoldoff = ONE_DAY;   // holdoff between new sms alerts for WP conditions.
    _interPumpAlertHoldoff = ONE_DAY;    // holdoff between new sms alerts for WP-PP conditions.
    _interPPrunTime = ONE_DAY;   // holdoff between new sms alerts for inter PP condition.
    _ppNotRunAlertHoldoff = THREE_DAYS; // holdoff between new sms alerts for no PP condition.
}

// Methods for testing purposes
float WSMAlertProcessor::get_ppAccumulatedOnTime() {
    return _ppAccumulatedOnTime;

}   // end of get_ppAccumulatedOnTime()

unsigned int WSMAlertProcessor::get_timeBetweenPPevents() {
    return _timeBetweenPPevents;

}   // end of get_timeBetweenPPevents()

unsigned int WSMAlertProcessor::get_ppAlertHoldoff() {
    return _ppAlertHoldoff;

}   // end of get_timeBetweenPPevents()

unsigned int WSMAlertProcessor::get_wpAlertHoldoff() {
    return _wpAlertHoldoff;

}   // end of get_wpAlertHoldoff()

unsigned int WSMAlertProcessor::get_interPumpAlertHoldoff() {
    return _interPumpAlertHoldoff;

}   // end of get_interPumpAlertHoldoff()

unsigned int WSMAlertProcessor::get_interPPrunTime() {
    return _interPPrunTime;

}   // end of get_interPPrunTime()

unsigned int WSMAlertProcessor::get_ppNotRunAlertHoldoff() {
    return _ppNotRunAlertHoldoff;

}   // end of et_ppNotRunAlertHoldoff()

// Methods for processing WSM data into alert events

// halfHourTimeTick(): called every ½ hour (when publishTRH() is called).  
//  This method increments all of the ½ hour time tick variables.  
//  It clamps all such variables if the variable value already exceeds the holdoff threshold, 
//  so that the values don’t get needlessly large.  
//  Generates an alert for “PP has not run for greater than a threshold time”.
void WSMAlertProcessor::halfHourTimeTick() {
    if(_timeBetweenPPevents < ONE_DAY) {
        _timeBetweenPPevents++;
    }
    
    if(_ppAlertHoldoff < ONE_DAY) {
        _ppAlertHoldoff++;
    }

    if(_wpAlertHoldoff < ONE_DAY) {
        _wpAlertHoldoff++;
    }

    if(_interPumpAlertHoldoff < ONE_DAY) {
        _interPumpAlertHoldoff++;
    }
    
    if(_ppNotRunAlertHoldoff < THREE_DAYS) {
        _ppNotRunAlertHoldoff++;
    }
    
    // we must test to see if PP didn't run at all for a long time
    if(_interPPrunTime < ONE_DAY) {
        _interPPrunTime++;
    } else {
        if (_ppNotRunAlertHoldoff >= THREE_DAYS) {
            // generate PP not run after too long time alert #7
            publishPPNotRun(_interPPrunTime);

            // reset the alert holdoff
            _ppNotRunAlertHoldoff = 0;
        }
        // clamp at one day
        _interPPrunTime = ONE_DAY;  

    }
        
} // end halfHourTimeTick()

// ppTurnedOn():  called every time the PP comes on
void WSMAlertProcessor::ppTurnedOn() {
    // pp has run, so reset alert counter
    _interPPrunTime = 0;

}  // end ppTurnedOn()
        
// ppTurnedOff():  called every time the PP turns off with PP run time as argument
void WSMAlertProcessor::ppTurnedOff(float runTime) {
    // evaluate PP on time for too short or too long alert
    if(_ppAlertHoldoff >= ONE_DAY) {    // we can generate an alert, if needed
        if(runTime < PP_ON_TOO_SHORT_LIMIT) {
            // publish PP too short run alert #2
            publishPPOnTooShortAlert(runTime);

            // reset the holdoff
            _ppAlertHoldoff = 0;
        } else 
        if(runTime > PP_ON_TOO_LONG_LIMIT) {
            // publish PP too long run alert #1
            publishPPOnTooLongAlert(runTime);

            // reset the holdoff
            _ppAlertHoldoff = 0;
        }
    }

    // accumulate the PP on time. Evaluate if WP didn't come on after too much PP run time
    if(_ppAccumulatedOnTime < WP_RUN_TOO_LONG_LIMIT) {
        _ppAccumulatedOnTime += runTime;

    } else {    // alert if WP didn't come on when it should have
        if(_interPumpAlertHoldoff >= ONE_DAY) { // we can generate alert
            // publish the alert #5
            publishWPNotComeOnAlert(_ppAccumulatedOnTime);

            // reset holdoff
            _interPumpAlertHoldoff = 0;

        }

        _ppAccumulatedOnTime = WP_RUN_TOO_LONG_LIMIT;  // clamp to the alert limit
    }
    return;

}   // end ppTurnedOff()

// wpTurnedOn():  called every time the WP comes on
void WSMAlertProcessor::wpTurnedOn() {
    // was accumulated PP on times < threshold when WP came on?
    if(_interPumpAlertHoldoff >= ONE_DAY) {     // we can generate an alert, if needed
        if(_ppAccumulatedOnTime < WP_RUN_TOO_SOON_LIMIT) {
            // publish WP came on too soon alert #6
            publishWPOnTooSoon(_ppAccumulatedOnTime);

            // reset the alert holdoff
            _interPumpAlertHoldoff = 0;
        }
    }
    // since WP came on, reset the PP accumulated run times (between WP events)
    _ppAccumulatedOnTime = 0.0;
    return;

}   // end wpTurnedOn()
        

// wpTurnedOff():  called every time the WP turns off with WP run time as argument
void WSMAlertProcessor::wpTurnedOff(float runTime) {
    // evaluate WP on time for too long or too short
    if(_wpAlertHoldoff >= ONE_DAY) {    // we can generate an alert, if needed
        if(runTime < WP_ON_TOO_SHORT_LIMIT) {
            // publish WP too short run alert #4
            pubLishWPOnTooShortAlert(runTime);

            // reset the holdoff
            _wpAlertHoldoff = 0;
        } else 
        if(runTime > WP_ON_TOO_LONG_LIMIT) {
            // publish WP too long run alert #3

            publishWPOnTooLongAlert(runTime);

            // reset the holdoff
            _wpAlertHoldoff = 0;
        }
    }


}   // end wpTurnedOff()

// methods for publishing the seven alerts

// publishPPOnTooLongAlert():  Alert published for PP running too long
//  argument is the PP run time
void WSMAlertProcessor::publishPPOnTooLongAlert(float onTime){      // alert #1
    // code to build json string and publish to Particle cloud
    String eData = "{\"etime\":";
    eData += String(Time.now());
    eData += ",\"msg\":\"PP on for ";
    eData += String(onTime);
    eData += " minutes.\"}";
    Particle.publish("wsmAlertPPOnTooLong", eData, PRIVATE);

} // end of publishPPOnTooLongAlert()

// publishPPOnTooShortAlert():  Alert published for PP running too short of a time
//  argument is PP run time
void WSMAlertProcessor::publishPPOnTooShortAlert(float onTime){    // alert #2
    // code to build json string and publish to Particle cloud
    String eData = "{\"etime\":";
    eData += String(Time.now());
    eData += ",\"msg\":\"PP on for ";
    eData += String(onTime);
    eData += " minutes.\"}";
    Particle.publish("wsmAlertPPOnTooShort", eData, PRIVATE);
        
} // end of publishPPOnTooShortAlert()

// publishWPOnTooLongAlert():  Alert published for WP running too long
//  argument is the WP run time
void WSMAlertProcessor::publishWPOnTooLongAlert(float onTime){     // alert #3
    // code to build json string and publish to Particle cloud
    String eData = "{\"etime\":";
    eData += String(Time.now());
    eData += ",\"msg\":\"WP on for ";
    eData += String(onTime);
    eData += " minutes.\"}";
    Particle.publish("wsmAlertWPOnTooLong", eData, PRIVATE);
       
} // end of publishWPOnTooLongAlert()

// publishWPOnTooShortAlert():  Alert published for WP running too short of a time
//  argument is the WP run time
void WSMAlertProcessor::pubLishWPOnTooShortAlert(float onTime){    // alert #4
    // code to build json string and publish to Particle cloud
    String eData = "{\"etime\":";
    eData += String(Time.now());
    eData += ",\"msg\":\"WP on for ";
    eData += String(onTime);
    eData += " minutes.\"}";
    Particle.publish("wsmAlertWPOnTooShort", eData, PRIVATE);
    
} // end of pubLishWPOnTooShortAlert()

// publishWPNotComeOnAlert(): alert published when WP doesn't come on after a lot of PP activity
//  argument is the accumulated PP run time since last WP run
void WSMAlertProcessor::publishWPNotComeOnAlert(float accumulatedPPTime){  // alert #5
    // code to build json string and publish to Particle cloud
    String eData = "{\"etime\":";
    eData += String(Time.now());
    eData += ",\"msg\":\"WP did not come on after PP run for > ";
    eData += String(accumulatedPPTime);
    eData += " minutes.\"}";
    Particle.publish("wsmAlertWPNotComeOn", eData, PRIVATE);
        
} // end of publishWPNotComeOnAlert()

// publishWPOnTooSoon(): alert published when WP came on after not enough accumulated PP run time
//  argument is the accumulated PP run time since last WP run
void WSMAlertProcessor::publishWPOnTooSoon(float accumulatedPPTime){      // alert #6
    // code to build json string and publish to Particle cloud
    String eData = "{\"etime\":";
    eData += String(Time.now());
    eData += ",\"msg\":\"WP came on after PP run for only ";
    eData += String(accumulatedPPTime);
    eData += " minutes.\"}";
    Particle.publish("wsmAlertWPOnTooSoon", eData, PRIVATE);

} // end of publishWPOnTooSoon()

// publishPPNotRun(): alert published if PP hasn't run for a day or more
//  argument it the time since the last PP run (in 1/2 hour ticks).
void WSMAlertProcessor::publishPPNotRun(unsigned int tickTime){    // alert #7
    // code to build json string and publish to Particle cloud
    String eData = "{\"etime\":";
    eData += String(Time.now());
    eData += ",\"msg\":\"PP did not run for at least the last day.\"}";
    Particle.publish("wsmAlertPPNotRun", eData, PRIVATE);    

} // end of publishPPNotRun()

