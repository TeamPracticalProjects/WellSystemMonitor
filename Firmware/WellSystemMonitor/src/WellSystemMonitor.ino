/*********************************************************************************************************
    WellSystemMonitor: Monitor and log the activity of key components of a water well pump and
    storage system.

        This program reads two inputs which are pulled low when one of two pumps is activated:
        a well pump, and a pressure pump. When one of these inputs transitions, the event is logged
        to a cloud storage system - a Google Docs spreadsheet in the first release.

        This first version of the system is intended only to log events so that some human analysis
        of the normal activity can be performed. We hope that in the future this system could incorporate
        alerts and alarms when the pumps run in an abnormal pattern.

        Eventually other sensors could be added to this system as discussed in the concept document.

        Because this system is built using the WaterLeakDetector code, temperature and humidity
        from a DHT11 sensor is also logged to the cloud storage every hour. Perhaps water pump
        activity will correlate with temperature and/or humidity.

        This program uses the Blynk cloud system to pass debug information, and current sensor
        readings to a smart phone application.

        This program also supports a "test" button. When pressed a test event is sent to the cloud
        storage.

    author: Bob Glicksman, Jim Schrempp; 06/25/2018

    (c) 2017, Bob Glicksman and Jim Schrempp, Team Practical Projects

20170530a: Added Blynk application notification of water detection with Blynk Terminal and LED.
***********************************************************************************************************/
//#define IFTTT_NOTIFY    // comment out if IFTTT alarm notification is not desired

#include <PietteTech_DHT.h> // non-blocking library for DHT11
#include <blynk.h>  // Blynk library

// Constants and definitions
#define DHTTYPE  DHT11              // Sensor type DHT11/21/22/AM2301/AM2302
const int WELL_PUMP_SENSOR_PIN = A0;
const int PRESSURE_PUMP_SENSOR_PIN = A1;
const int LED_PIN = D7;
const int INDICATOR_PIN = D5;
const int BUTTON_PIN = D4;
const int DHTPIN = D2;        	    // Digital pin for communications
const int HT_SWITCH_PIN = D1;               // pin for temperature/humidity toggle switch
const int SERVO_PIN = A5;                // servo pin
const bool HT_SWITCH_HUMIDITY = false;
const bool HT_SWITCH_TEMPERATURE = true;
#define DHT_SAMPLE_INTERVAL   4000  // Sample every 4 seconds; must not be less than the time required to read DHT
#define PARTICLE_PUBLISH_INTERVAL 60000 // Publish values every 60 seconds
#define SECOND_NOTIFY_DELAY 30000  // the second alarm notification comes 30 seconds after the first notification

// servo calibration values
const int MIN_POS = 5;  // the minimum position value allowed
const int MAX_POS = 175;  // the maximum position value allowed

// meter face range values
const int HI_TEMP = 120;  // based upon meter dial face for temperature (F)
const int LO_TEMP = 40;   // based upon meter dial face for temperature (F)
const int HI_HUM = 100;  // based upon meter dial face for humidity (%RH)
const int LO_HUM = 0;  // based upon meter dial face for humidity (%RH)

// calculate meter dial ranges from hi and lo values
#define TEMP_RANGE (HI_TEMP - LO_TEMP)
#define HUM_RANGE (HI_HUM - LO_HUM)

// sensor return status codes
const unsigned int ACQUIRING  = 0;
const unsigned int COMPLETE_OK  = 1;
const unsigned int COMPLETE_ERROR  = 2;



// global to hold the smoothed values of humidity and temperature that we display and report
float mg_smoothedTemp = 0.0, mg_smoothedHumidity = 0.0; // smoothed for the display

// globals to hold state of the pump sensors
bool mg_wellPumpState, mg_pressurePumpState;

// structure to hold pins that need to be DEBOUNCED
typedef struct {
    unsigned int pinNumber;
    bool value;
    bool lastReadValue;
    unsigned long beginTime;
    unsigned long debounceDelay; // in milliseconds
} ty_debouncePin;
ty_debouncePin mg_pushbutton, mg_wellPumpSensor, mg_pressurePumpSensor, mg_htSwitchPin;

bool readPinDebounced(ty_debouncePin *_pinToRead); // Early declare to avoid compiler issue

// Lib instantiate
PietteTech_DHT DHT(DHTPIN, DHTTYPE);    // create DHT object to read temp and humidity
Servo myservo;  // create servo object to control a servo


// Utility functions

String dateTimeString(){
    time_t timeNow = Time.now();
    String dateTime = Time.format(timeNow,TIME_FORMAT_DEFAULT) + " UTC";
    return dateTime;
}

// We leave the method calls so that we don't have to ifdef every place it might be
// called in the code.
void raiseAlarm()
{

}

void reportRestart()
{

}


// Globals
boolean LEDPinState = false;   // D7 LED is used for indicating DHT measurements

int mg_debugValue = 100;
int mg_debugValue2 = 200;
String mg_debugString1 = "***";

// setup()
void setup() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(INDICATOR_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(HT_SWITCH_PIN, INPUT_PULLUP);  // toggle switch uses an internal pullup
    pinMode(WELL_PUMP_SENSOR_PIN, INPUT_PULLUP);
    pinMode(PRESSURE_PUMP_SENSOR_PIN, INPUT_PULLUP);
    myservo.attach(SERVO_PIN);  // attaches to the servo object
    mg_pushbutton = {.pinNumber = BUTTON_PIN, .value = false, .lastReadValue = false, .beginTime = 0, .debounceDelay = 100};
    mg_wellPumpSensor = {.pinNumber = WELL_PUMP_SENSOR_PIN, .value = false, .lastReadValue = false, .beginTime = 0,  .debounceDelay = 1000};
    mg_pressurePumpSensor = {.pinNumber = PRESSURE_PUMP_SENSOR_PIN, .value = false, .lastReadValue = false, .beginTime = 0, .debounceDelay = 1000};
    mg_htSwitchPin = {.pinNumber = HT_SWITCH_PIN, .value = false, .lastReadValue = false, .beginTime = 0, .debounceDelay = 50};

}  // end of setup()

// loop()
void loop() {
    static boolean indicator = false;  // set to true to flash the indicator
    static unsigned long lastDHTReadTime = 0UL;    // DHT 11 reading time
    static unsigned long lastPublishTime = 0UL;  // Published particle event time
    static boolean newDHTData = false; // flag to indicate DHT11 has new data
    static boolean htSwitchState = HT_SWITCH_TEMPERATURE;  // hold the reading of the toggle switch
    static boolean htSwitchLastState = HT_SWITCH_TEMPERATURE;  // hold the previous reading of the toggle switch
    static boolean firstNotification = false;  // indicator to use for a second alarm notification
   	static unsigned long firstNotifyTime;	// record time of first notification to time the second one

    // Non-blocking read of DHT11 data and publish and display it
    float currentTemp, currentHumidity;


    static boolean onceUponRestart = true;
    if (onceUponRestart){
        onceUponRestart = false;

    }

    int DHTsensorStatus = startReadDHT(false);  // refresh the sensor status but don't start a new reading

	if(DHTsensorStatus != ACQUIRING) {
      if(newDHTData == true) { // we have new data
        currentTemp = DHT.getFahrenheit();
        currentHumidity = DHT.getHumidity();

        // Smooth the readings for display
        if (mg_smoothedTemp < 1) {   // first time init
            mg_smoothedTemp = currentTemp;
        }
        if (mg_smoothedHumidity < 1){  // first time init
            mg_smoothedHumidity = currentHumidity;
        }
        // 10 point moving average
        mg_smoothedTemp =  (0.9 * mg_smoothedTemp) +  (0.1 * currentTemp);
        mg_smoothedHumidity =  (0.9 * mg_smoothedHumidity) +  (0.1 * currentHumidity);

	    newDHTData = false; // don't update results again until a new reading
      }

      if((diff(millis(), lastDHTReadTime)) >= DHT_SAMPLE_INTERVAL) { // we are ready for a new reading
          startReadDHT(true);  // start a new reading
          newDHTData = true; // set flag to indicate that a new reading will result
          lastDHTReadTime = millis();


            // toggle the D7 LED to indicate loop timing for DHT11 reading
            LEDPinState = !LEDPinState;
            if (LEDPinState) {
                digitalWrite(LED_PIN, HIGH);
            } else {
                digitalWrite(LED_PIN, LOW);
            }

        }
    }

    // Handle toggle switch and servo meter

    //  read the toggle switch position and set the boolean for type of display accordingly
    if(mg_htSwitchPin.value == false)  {   // indicates a temperature display
        htSwitchState = HT_SWITCH_TEMPERATURE;
    } else {
        htSwitchState = HT_SWITCH_HUMIDITY;
    }

    moveServo(htSwitchState);

    if((diff(millis(), lastPublishTime)) >= PARTICLE_PUBLISH_INTERVAL)  // we should publish our values
    {
        lastPublishTime = millis();
        // publish Smoothed temperature and humidity readings to the cloud
        Particle.publish("Humidity Smoothed (%)", String(mg_smoothedHumidity));
        Particle.publish("Temperature Smoothed (oF)", String(mg_smoothedTemp));
    }


    // Handle pushbutton

    // process the pushbutton
    if(readPinDebounced(&mg_pushbutton) == true) {
        //Pinstate has changed
        String tempString = String(mg_pushbutton.value);
        Particle.publish("pushbutton state:", tempString );
    }


    // Handle the sensors

    // process the well pump sensor
    if(readPinDebounced(&mg_wellPumpSensor) == true) {
        String tempString = String(mg_wellPumpSensor.value);
        Particle.publish("well pump state:", tempString);
    }

    // process the pressure pump sensor
    if(readPinDebounced(&mg_pressurePumpSensor) == true) {
        String tempString = String(mg_pressurePumpSensor.value);
        Particle.publish("pressure pump state:", tempString);
    }

} // end of loop()




/* nbFlashIndicator():  non-blocking function to flash the indicator LED when alarming
                        or light it constantly when not alarming
    parameters:
        flash - true to flash the LED, false to light it constantly
*/
void nbFlashIndicator(boolean flash) {
    const unsigned long FLASH_INTERVAL = 150; // 150 ms on and off
    static boolean lastOn = true;   // start with LED on
    static unsigned long lastTime = millis();

    if(flash == true) {     // flashes the LED
        if(diff(millis(), lastTime) >= FLASH_INTERVAL) { // flip the LED state
            lastOn = !lastOn;
            lastTime = millis();
        }

    } else {  // not flashing the LED
        lastOn = true;
    }

    if(lastOn == true)  {
        digitalWrite(INDICATOR_PIN, HIGH);
    } else {
        digitalWrite(INDICATOR_PIN, LOW);
    }
    return;
}   // end of nbFlashIndicator()

/* readPinDebounced: read a pin with debouncing
    parameters: pass in structure holding state of the pin
    return:
        flag that indicates the pin status has changed
*/
bool readPinDebounced(ty_debouncePin *_pinToRead) {
    bool stateChanged = false;
    bool currentReadValue;

    int currentPinRead = digitalRead(_pinToRead->pinNumber);

    if (currentPinRead == 0){
        currentReadValue = false;
    } else {
        currentReadValue = true;
    }

    unsigned long timeNow = millis();

    if(currentReadValue != _pinToRead->lastReadValue) { // has the pin changed since last time here?
        // pin changed, reset timer
        _pinToRead->beginTime = timeNow;
        _pinToRead->lastReadValue = currentReadValue;

    } else {
        // pin did not change since last time in this routine
        if(diff(timeNow, _pinToRead->beginTime) > _pinToRead->debounceDelay) { // has pin change lasted?
            // pin change lasted, so it's a valid change
            if(_pinToRead->value != currentReadValue){
                //saved pin state is getting updated
                _pinToRead->value = currentReadValue;
                stateChanged = true; // alert the caller
            }
        }
    }

    return stateChanged;

}  // end of readPinDebounced


/* diff(): function to measure time differences using millis() that corrects for millis() overflow.
    paramters:
        current - the current time value from millis(), as unsigned long
        last - the previous time value from millis(), as unsigned long
    return:
        the difference between current and last, as unsigned long
*/
unsigned long diff(unsigned long _current, unsigned long _last)  {
    const unsigned long MAX = 0xffffffff;  // an unsigned long is 4 bytes
    unsigned long difference;

    if (_current < _last) {       // overflow condition
        difference = (MAX - _last) + _current;
    } else {
        difference = _current - _last;
    }
    return difference;
}  // end of diff()


/*
startReadDHT():  read temperature and humidity from the DHT11 sensor
    arguments:
        startRead:  true to start a reading, false otherwise
    return: status code, per global definitions
*/
int startReadDHT(boolean _startRead) {

    static int state = COMPLETE_OK;
    static int dhtResultCode;  // hold the result code from DHT sensor reading

    if(state == ACQUIRING) {  // test to see if we are done
        if(DHT.acquiring() == false) { // done acquriring
            dhtResultCode = DHT.getStatus();  // store the result code from the library
           if(dhtResultCode == DHTLIB_OK) {
               state = COMPLETE_OK;
           } else {
               state = COMPLETE_ERROR;
           }
        }
    }
    else { // we were not in the process of acquiring
        if(_startRead == true) {  // we must start a new reading of sensor data
           state = ACQUIRING;  // set the state to acquiring
            DHT.acquire(); // start the acquisition
        }
    }

    return state;

}  // end of startReadDHT()

/* moveServo(): function to set the servo position based on loop variable htSwitchState
*/
void moveServo(boolean _switchState) {
    if(_switchState == HT_SWITCH_TEMPERATURE)  {   // temperature reading called for
        meterDisplay(mg_smoothedTemp, LO_TEMP, HI_TEMP);
    }  else  {  // humidity reading called for
        meterDisplay(mg_smoothedHumidity, LO_HUM, HI_HUM);
    }
};

/* meterDisplay():  display a value reading on the servo meter
    arguments:
        displayValue: the value to display on the meter
        lowestValue: the value that corresponds to the lowest side of the display servo position
        highestValue: the value that corresponds to the highest side of the display servo position
    Relies on globals MAX_POS and MIN_POS which are the servo position settings for the lowest
        and highest positions of the servo
    Sample Call: meterDisplay(mg_smoothedHumidity, LO_HUM, HI_HUM)
*/
void meterDisplay(float _displayValue, int _lowestValue, int _highestValue)  {
    int intDisplayValue, relPosition, absPosition;

    intDisplayValue = (int)(_displayValue + 0.5);  // round and truncate to an integer

    // clamp temperature to within dial limits
    if(intDisplayValue < _lowestValue) {
        intDisplayValue = _lowestValue;
    } else if(intDisplayValue > _highestValue) {
        intDisplayValue = _highestValue;
    }

    relPosition = (intDisplayValue - _lowestValue) * (MAX_POS - MIN_POS) / (_highestValue - _lowestValue);
    absPosition = MAX_POS - relPosition;
    myservo.write(absPosition);

    return;
}  // end of meterDisplay()
