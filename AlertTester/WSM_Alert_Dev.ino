/*********************************************************************
 * Project WSM_Alert_Dev
 * Description:  Test program to test out WSMAlertProcessor code
 * Author: Bob Glicksman, Jim Schrempp
 * Date: 8/2/2022
 * (c) 2022, Bob Glicksman, Jim Schrempp, Team Practical Projects;
 *    all righnts reserved.
 * 
 * version 1.0: 8/9/22.  Initial release
 * version 1.1: 8/23/22.  Fixed but in ppNotRunAlertHoldoff test 21
 *********************************************************************/
#include "WSMAlertProcessor.h"

// Constants
const int LED_PIN = D7;
const int BUTTON_PIN = D0;

enum ButtonStates {
  NOT_PRESSED = 1,
  TENTATIVE_PRESS = 2,
  PRESSED_ACTION = 3,
  WAIT_FOR_RELEASE = 4
};

// create instance of WSMAlertProcessor class
WSMAlertProcessor alerter;

//SYSTEM_THREAD(ENABLED);

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);   // Serial port will be used for test status messages
  alerter.begin();      // initializae all alert processor internal variables 
  digitalWrite(LED_PIN, HIGH);  // signal to open putty or other serial monitor
  delay(5000);  // wait 5 seconds to get serial monitor open
  digitalWrite(LED_PIN, LOW);
  Serial.println("Testing of WSMAlertProcessor code. View publications on the Particle Console.");
  Serial.print("\nInternal Variable Values = ");
  printVar();
}

void loop() {
  static unsigned int testNum = 1; // start with testcase number 1

  // wait for the button to be pressed
  if(buttonPressed() == true) {
    executeTestCase(testNum);
    testNum++;
  }
}

// executeTestCase(unsigned int testCase):  the test cases to run
void executeTestCase(unsigned int testcase) {
  switch(testcase) {
    case 1:   // run an initial PP on too long test - should generate an alert
      Serial.println("Test 1: PP on too long test.");
      alerter.ppTurnedOn();
      alerter.ppTurnedOff(3.1); // longer than alert threshold
      Serial.println("Should have gotten an alert publication");
      printVar(); // dump out the variables
      Serial.println("\nnow press the button again.  Should not get the alert due to holdoff");
      break;

    case 2:   // run PP on too long test again - should not get alert due to holdoff
      Serial.println("Test 2:  PP alert holdoff");
      alerter.ppTurnedOn();
      alerter.ppTurnedOff(3.1); // longer than alert threshold

      // make 48 half hour ticks
      for(int i = 0; i< 48; i++) {
        alerter.halfHourTimeTick();
      }
      printVar(); // dump out the variables
      Serial.println("\nnow press the button again.  Should get the alert again after the holdoff");

      break;

    case 3: // holdoff is timed out, so should get PP on too long alert again
      Serial.println("Test 3: PP alert holdoff timed out");
      alerter.ppTurnedOn();
      alerter.ppTurnedOff(3.1); // longer than alert threshold
      Serial.println("Should have gotten another alert -- see Console.\n");
      alerter.begin();  // reset all variables for the next set of tests
      Serial.println("press button for the next test");
      break;

    case 4:   // run an initial PP on too short test - should generate an alert
      Serial.println("Test 4: PP on too short test.");
      alerter.ppTurnedOn();
      alerter.ppTurnedOff(0.49); // less than alert threshold
      Serial.println("Should have gotten an alert publication");
      printVar(); // dump out the variables
      Serial.println("\nnow press the button again.  Should not get the alert due to holdoff");
      break;

    case 5:   // run PP on too short test again - should not get alert due to holdoff
      Serial.println("Test 5: PP alert holdoff");
      alerter.ppTurnedOn();
      alerter.ppTurnedOff(0.49); // shorter than alert threshold

      // make 48 half hour ticks
      for(int i = 0; i< 48; i++) {
        alerter.halfHourTimeTick();
      }
      printVar(); // dump out the variables
      Serial.println("\nnow press the button again.  Should get the alert again after the holdoff");

      break;

    case 6: // holdoff is timed out, so should get PP on too short alert again
      Serial.println("Test 6: PP alert holdoff timed out");
      alerter.ppTurnedOn();
      alerter.ppTurnedOff(0.49); // shorter than alert threshold
      Serial.println("Should have gotten another alert -- see Console.\n");
      alerter.begin();  // reset all variables for the next set of tests
      Serial.println("press button for the next test");
      break;

    case 7:   // run an initial WP on too long test - should generate an alert
      Serial.println("Test 7: WP on too long test.");

      // make sure that PP has come on > 10 miuntes total in 3 minute increments
      for(int i = 0; i < 4; i++) {
        alerter.ppTurnedOn();
        alerter.ppTurnedOff(3.0); // pp on OK amount of time
      }

      alerter.wpTurnedOn();
      alerter.wpTurnedOff(40.1); // longer than alert threshold
      Serial.println("Should have gotten an alert publication");
      printVar(); // dump out the variables
      Serial.println("\nnow press the button again.  Should not get the alert due to holdoff");
      break;

    case 8:   // run WP on too long test again - should not get alert due to holdoff
      Serial.println("Test 8: WP alert holdoff");

      // make sure that PP has come on > 10 miuntes total in 3 minute increments
      for(int i = 0; i < 4; i++) {
        alerter.ppTurnedOn();
        alerter.ppTurnedOff(3.0); // pp on OK amount of time
      }

      alerter.wpTurnedOn();
      alerter.wpTurnedOff(40.1); // longer than alert threshold

      // make 48 half hour ticks
      for(int i = 0; i< 48; i++) {
        alerter.halfHourTimeTick();
      }
      printVar(); // dump out the variables
      Serial.println("\nnow press the button again.  Should get the alert again after the holdoff");

      break;

    case 9: // holdoff is timed out, so should get WP on too long alert again
      Serial.println("Test 9: WP alert holdoff timed out");

      // make sure that PP has come on > 10 miuntes total in 3 minute increments
      for(int i = 0; i < 4; i++) {
        alerter.ppTurnedOn();
        alerter.ppTurnedOff(3.0); // pp on OK amount of time
      }

      alerter.wpTurnedOn();
      alerter.wpTurnedOff(40.1); // longer than alert threshold
      Serial.println("Should have gotten another alert -- see Console.\n");
      alerter.begin();  // reset all variables for the next set of tests
      Serial.println("press button for the next test");
      break;

    case 10:   // run an initial WP on too short test - should generate an alert
      Serial.println("Test 10: WP on too short test.");

      // make sure that PP has come on > 10 miuntes total in 3 minute increments
      for(int i = 0; i < 4; i++) {
        alerter.ppTurnedOn();
        alerter.ppTurnedOff(3.0); // pp on OK amount of time
      }

      alerter.wpTurnedOn();
      alerter.wpTurnedOff(19.9); // less than alert threshold
      Serial.println("Should have gotten an alert publication");
      printVar(); // dump out the variables
      Serial.println("\nnow press the button again.  Should not get the alert due to holdoff");
      break;

    case 11:   // run WP on too short test again - should not get alert due to holdoff
      Serial.println("Test 11: WP alert holdoff");

      // make sure that PP has come on > 10 miuntes total in 3 minute increments
      for(int i = 0; i < 4; i++) {
        alerter.ppTurnedOn();
        alerter.ppTurnedOff(3.0); // pp on OK amount of time
      }

      alerter.wpTurnedOn();
      alerter.wpTurnedOff(19.9); // shorter than alert threshold

      // make 48 half hour ticks
      for(int i = 0; i< 48; i++) {
        alerter.halfHourTimeTick();
      }
      printVar(); // dump out the variables
      Serial.println("\nnow press the button again.  Should get the alert again after the holdoff");

      break;

    case 12: // holdoff is timed out, so should get WP on too short alert again
      Serial.println("Test 12: WP alert holdoff timed out");

      // make sure that PP has come on > 10 miuntes total in 3 minute increments
      for(int i = 0; i < 4; i++) {
        alerter.ppTurnedOn();
        alerter.ppTurnedOff(3.0); // pp on OK amount of time
      }
      
      alerter.wpTurnedOn();
      alerter.wpTurnedOff(19.9); // shorter than alert threshold
      Serial.println("Should have gotten another alert -- see Console.\n");
      alerter.begin();  // reset all variables for the next set of tests
      Serial.println("press button for the next test");
      break;

    case 13:  // test for WP didn't come on after total PP times > 30 minutes
      Serial.println("\nTest 13: WP didn't come on after a lot of PP time");

      // accumulate > 30 minutes PP on time, in normal PP run times
      for(int i = 0; i < 31; i++) {
        alerter.ppTurnedOn();
        alerter.ppTurnedOff(3.0); // pp on OK amount of time    
      }

      Serial.println("Should have gotten alert #5 - WP didn't come on after a lot of PP time");
      printVar(); // dump out the variables
      Serial.println("Press the button for the next test");
      break;

    case 14:  // test for WP didn't come on after total PP times > 30 minutes alert holdoff
      Serial.println("\nTest 14: Alert holdoff test for WP didn't come on after a lot of PP time");

      // add another PP 3 minute on time without WP.  No alert due to holdoff
      alerter.ppTurnedOn();
      alerter.ppTurnedOff(3.0); // pp on OK amount of time    
      

      Serial.println("Should not have gottn alert due to holdoff");
      printVar(); // dump out the variables
      Serial.println("Press the button for the next test");
      break;

    case 15:  // test for WP didn't come on after total PP times > 30 minutes alert holdoff after holdoff
      Serial.println("\nTest 15: Alert holdoff expited test for WP didn't come on after a lot of PP time");

      // add a day (48 ticks) to the time to expire the holdoff
      for(int i = 0; i< 48; i++) {
        alerter.halfHourTimeTick();
      }     

      // add another PP 3 minute on time without WP.  No alert due to holdoff
      alerter.ppTurnedOn();
      alerter.ppTurnedOff(3.0); // pp on OK amount of time    
      
      Serial.println("Should  have gotten alert due to holdoff");
      printVar(); // dump out the variables
      Serial.println("Press the button for the next test");
      alerter.begin();  // reset all variables for the next set of tests
      break;


    case 16:  // test for WP come on after too little total PP time (< 10 minutes)
      Serial.println("\nTest 16: WP came on after too little accumulated PP time");

      // accumulate < 10 minutes PP on time, in normal PP run times
      for(int i = 0; i < 9; i++) {
        alerter.ppTurnedOn();
        alerter.ppTurnedOff(1.0); // pp on OK amount of time    
      }

      // turn on the WP and run it for a normal time
      alerter.wpTurnedOn();
      alerter.wpTurnedOff(30.0);

      Serial.println("Should have gotten alert #6 - WP came on after too short PP total time");
      printVar(); // dump out the variables
      Serial.println("Press the button for the next test");
      break;

    case 17:  // test for WP came on after total PP time too short alert holdoff
      Serial.println("\nTest 17: Alert holdoff test for WP came on after too little total PP time");

      // turn on the WP and run it for a normal time
      alerter.wpTurnedOn();
      alerter.wpTurnedOff(30.0);

      Serial.println("Should not have gottn alert due to holdoff");
      printVar(); // dump out the variables
      Serial.println("Press the button for the next test");
      break;

    case 18:  // test for WP came on after too little total PP times after holdoff
      Serial.println("\nTest 18: Alert holdoff expired test for WP didn't come on after a lot of PP time");

      // add a day (48 ticks) to the time to expire the holdoff
      for(int i = 0; i< 48; i++) {
        alerter.halfHourTimeTick();
      }     

      // turn on the WP and run it for a normal time
      alerter.wpTurnedOn();
      alerter.wpTurnedOff(30.0); 
      
      Serial.println("Should  have gotten alert due to holdoff expired");
      printVar(); // dump out the variables
      Serial.println("Press the button for the next test");
      alerter.begin();  // reset all variables for the next set of tests
      break;

    case 19:  // test for PP doesn't run for > 1 day
      Serial.println("\nTest 19: Alert when PP doesn't come on for > 1 day");

      // add a little more than a day (50 ticks) to the time with no PP runs
      for(int i = 0; i< 50; i++) {
        alerter.halfHourTimeTick();
      }

      Serial.println("Should have gotten an alert that PP hasn't run");
      printVar(); // dump out the variables
      Serial.println("Press the button for the next test");     
      break;

    case 20:  // test for alert holdoff for PP doesn't run for > 1 day
      Serial.println("\nTest 20: Alert holdoff - no alert when PP doesn't come on for > 1 day");

      // add a little more than two days (100 ticks) to the time with no PP runs
      for(int i = 0; i< 100; i++) {
        alerter.halfHourTimeTick();
      }

      Serial.println("Should not have gotten an alert due to holdoff");
      printVar(); // dump out the variables
      Serial.println("Press the button for the next test");     
      break;

    case 21:  // test for removal of alert holdoff for PP doesn't run for > 1 day after 3 days
      Serial.println("\nTest 21: Alert holdoff removal- alert when PP doesn't come on for > 1 day after 3 day holdoff");

      // add a little more than day (50 ticks) to the time with no PP runs  -- this makes > 3 days
      for(int i = 0; i< 50; i++) {
        alerter.halfHourTimeTick();
      }

      Serial.println("Should have gotten an alert after holdoff expires");
      printVar(); // dump out the variables
      alerter.begin();  // clear out all variables
      Serial.println("Press the button for the next test");     
      break;
  

  default:
    Serial.println("No more test cases.  Reset the Photon to repeat tests.");
    break;

  }
  return;

} // end of executeTestCase()

// printVar():  function to  print out all internal variables to the console
void printVar() {
  Serial.println("The values of the internal variables are:");
  Serial.print("ppAccumulatedOnTime = ");
  Serial.print(alerter.get_ppAccumulatedOnTime()); 
  Serial.print("\t\ttimeBetweenPPevents = ");
  Serial.println(alerter.get_timeBetweenPPevents());
  Serial.print("ppAlertHoldoff = ");
  Serial.print(alerter.get_ppAlertHoldoff());
  Serial.print("\t\twpAlertHoldoff = ");
  Serial.println(alerter.get_wpAlertHoldoff());
  Serial.print("interPumpAlertHoldoff = ");
  Serial.print(alerter.get_interPumpAlertHoldoff());
  Serial.print("\t\tinterPPrunTime = ");
  Serial.println(alerter.get_interPPrunTime());
  Serial.print("ppNotRunAlertHoldoff = ");
  Serial.println(alerter.get_ppNotRunAlertHoldoff());
  Serial.println(); // extra blank line for readability

} // end of printVar()


// buttonPressed():  function to debounce button in a non bloacking manner
//  return the button action state -- true for a new button press, false otherwise
bool buttonPressed() {
  static ButtonStates buttonState = NOT_PRESSED;
  static unsigned long pressedTime;

  // state machine to track button actions
  switch(buttonState) {
    case NOT_PRESSED:
      if(digitalRead(BUTTON_PIN) == HIGH) { // button is not pressed
        buttonState = NOT_PRESSED;
        return false;
      }
      else {  // the button seems to be pressed
        pressedTime = millis();
        buttonState = TENTATIVE_PRESS;
        return false;
      }
      break;

    case TENTATIVE_PRESS:
      if(digitalRead(BUTTON_PIN) == HIGH) { // button is not pressed; false alarm
        buttonState = NOT_PRESSED;
        return false;
      }
      else {  // button is stilll pressed - time out debounce
        if( (millis() - pressedTime) > 10) {  // button has been pressed for 10 ms, so debounced
          buttonState = WAIT_FOR_RELEASE;
          return true;
        }
        else {  // button not fully debounded
          buttonState = TENTATIVE_PRESS;
          return false;
        }
      }
      break;

    case WAIT_FOR_RELEASE:
      if(digitalRead(BUTTON_PIN) == HIGH) { // button is released
        buttonState = NOT_PRESSED;
        return false;  
      }
      else {  // button still held down, wait for release
        buttonState = WAIT_FOR_RELEASE;
        return false;
      }
      break;

    default:
      if(digitalRead(BUTTON_PIN) == HIGH) { // button is released
        buttonState = NOT_PRESSED;
        return false;  
      }
      else {  // button still held down, wait for release
        buttonState = WAIT_FOR_RELEASE;
        return false;
      }
      break;

  }

} // end of buttonPressed()
