/*
 * Project SheetAPI_Test
 * Description:  Test firmware for publication of data to a Google sheet without
 *  using IFTTT.  This project uses a webhook to publish data to a Google Apps Script
 *  that appends a row of data to a designated Google sheet.
 * 
 * Author: Bob Glicksman, Jim Schrempp, Team Practical Projects
 * Date: 6/05/21
 */

const int LED_PIN = D7;  // declare the onboard LED

void setup() {
  pinMode (LED_PIN, OUTPUT);

  // flash the LED_PIN to indicate end of setup
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);

} // end of setup()

void loop() {
  // declare the variables for the WSM
  float temperature = 65.0f;
  float rh = 20.0f;
  int ppValue = 0;
  int wpValue = 0;

  digitalWrite(LED_PIN, HIGH);  // turn on the onboard LED to indicate tests in progress

  // test #1: publish new temp and RH
  publishTRH(temperature, rh);

    // create new test values for next pass
  temperature += 0.2f;
  rh += 0.4f;

    // delay before next test
  delay(2000);

  // test #2: publish pp turns on, then turns off
  publishPPchange(1);
  delay(10000); // 10 second delay, then pump goes off
  publishPPchange(0);

     // delay before next test
  delay(2000);

  // test #3: publish pp turns on, then turns off again
  publishPPchange(1);
  delay(10000); // 10 second delay, then pump goes off
  publishPPchange(0);

     // delay before next test
  delay(2000);

  // test #4: publish new temp and RH
publishTRH(temperature, rh);

    // create new test values for next pass
  temperature += 0.2f;
  rh += 0.4f;

    // delay before next test
  delay(2000);

  // test #5: publish wp turns on, tehn turns off again
  publishWPchange(1);
  delay(10000); // 10 second delay, then pump goes off
  publishWPchange(0);

     // delay before next test
  delay(2000);

  // turn off the D7 LED and wait 2 minutes before next loop
  digitalWrite(LED_PIN, LOW);
  delay(120000);

} // end of loop()

// Publication functions:

//  publish new temperature and humidity values
void publishTRH(float temp, float rh) {
  String eData = "";

  // build the data string with time, temp and rh values
  eData += "{\"etime\":";
  eData += String(Time.now());
  eData += ",\"temp\":";
  eData += String(temp);
  eData += ",\"rh\":";
  eData += String(rh);
  eData += "}";

  // publish to the webhook
  Particle.publish("wsmEventTRH", eData, PRIVATE);

  return;
} // end of publishTRH()

//  publish pressure pump status change
void publishPPchange(int newPPstatus) {
  String eData = "";

  // build the data string with time, pp value
  eData += "{\"etime\":";
  eData += String(Time.now());
  eData += ",\"pp\":";
  eData += String(newPPstatus);
  eData += "}";

  // publish to the webhook
  Particle.publish("wsmEventPPstatus", eData, PRIVATE);

  return;
} // end of publishPPchange()

//  publish well pump status change
void publishWPchange(int newWPstatus) {
  String eData = "";

  // build the data string with time, pp value
  eData += "{\"etime\":";
  eData += String(Time.now());
  eData += ",\"wp\":";
  eData += String(newWPstatus);
  eData += "}";

  // publish to the webhook
  Particle.publish("wsmEventWPstatus", eData, PRIVATE);

  return;
} // end of publishWPchange()

