# WellSystemMonitor

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

This system publishes events and exposes variables through the Particle.io cloud system 
to pass current sensor readings to a smart phone application. It also uses IFTTT to 
capture these published events to a Google Docs spreadsheet. 

This program also supports a "test" button. When pressed a test event is sent to the cloud
storage.
