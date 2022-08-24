# WellSystemMonitor

## Project Overview.
The purpose of this project is to monitor the operation of a water well system and to
alert the system owner of problems with the system before they cause system damage. The
well system consists of the following components:

1. Well Pump: the Well Pump is buried and is very expensive to replace. The pump is used
to pump water out of the well and into a reservoir. The well pump should only run for
limited amounts of time to extend its life.

2. Reservoir: a large reservoir holds water from the well, in order to limit the pumping times
from the Well Pump.  A float sensor in the reservoir determines when the well pump should turn on
and off.

3. Pressure Tank:  a large pressure tank holds water, under pressure, for use within the premises.

4. Pressure Pump: the Pressure Pump recharges the pressure tank using water from the reservoir.

The Well Pump and the Pressure Pump are run by a control system whose inputs are the reservoir float sensor
and the Pressure Tank pressure sensor.  A problem with either of these sensors, or with the Pressure Tank
bladder, or other component of the system, can cause the Pressure Pump and/or the Well Pump to run either too long
or too frequently.  Long term improper running of these pumps can cause them to fail prematurely, resulting
in a very expensive repair.  It is much more cost effective to proactively repair the cause of improper pump
operation rather than wait for a pump to fail.  The current control system does not provide the owner with
information that can alert him/her when the system is running outside of normal operating parameters.  This project
adds in logging of Well Pump and Pressure Pump (to a cloud spreadsheet) and an alert system that sends SMS
texts to the well owner whenever an out-of-spec operation occurs.  Out of spec operations that cause alerts are:

1. Pressure Pump runs too long.
2. Pressure Pumps runs to short.
3. Well pump runs too long.
4. Well Pump runs too short.
5. Well Pump does not come on after accumulated Pressure Pump times reach a threshold.
6. Well Pump comes on too soon (not enough Pressure Pumping time).
7. Pressure Pump does not come on at all within an expected period of time (e.g. one day).

The SMS text alerts generated whenever any of these alert events occurs should prompt the well system owner to consult
the on-line spreadsheet log of historical pump events.  One year of data collection and evaluation has allowed us to
develop criteria on the trends of the logged data that are indicative of various system failure conditions that should
be repaired in order to preserve the life of the expensive pumps.

## Project Implementation.
The existing well control system has multi-pole relays that active the Well Pump and the Pressure Pump.  There is an
unused pole on each relay and we use these contact closures to detect activations and deactivations of the respective
pumps.  We have repurposed the hardware from our Water Leak Detector project; see: 

https://github.com/TeamPracticalProjects/WaterLeakSensor

The hardware is based upon a Particle Photon Wi-Fi enabled microcontroller module.

We have converted the Water Leak Sensor firmware to fit the needs of this project. In addition to the Particle Photon firmware,
the project has the following software components:

1. A Google Sheets cloud spreadsheet to log pump activation and deactivation events, along with temperaure and relative humidity
readings that can help determine if pump running trends are in or out of normal.

2. Two Google Apps Scripts.  The first script is activated by event publications in the Photon firmware, via Particle webhooks. This
script processes the event data and writes new rows of logging data to the Google spreadsheet.  This script also manages the size of the
Google spreadsheet to prevent it from getting too large.  The second script is activated by alert detection publications in the 
Photon firmware, via Particle webhooks. This script processes the alert data and sends emails to the well owner's cellular carrier's
sms gateway in order to alert the owner via SMS texts to a his/her cell phone.

3. A Android app: the app is used to spot check the monitoring and alert system.

4. Two Particle webhooks:  these are simple webhooks that process publications from the Particle Photon and make https:// POST
calls to their respective scripts.

## Project Status.
We developed the first version of the system to log well data to a Google sheet and to evaluate the usefulness of these logs.
Experience with this first version caused us to make changes to the presentation of the logged data so that we could better utilize the
spreadsheet's capabilities to process and graph the data.  The second version of the system was used for over one year to monitor logged
data and, in particular, the trends of the data.  We found that the trends of the data told us a lot about the internal health of the
well system.  For example, a slow reduction in Pressure Pump run times is a clear indication that the pressure tank bladder is leaking.

After one year of data observation and analysis, we were able to develop the seven alert criteria described above.  These seven alerts
are sufficient to tell the owner to consult the logs to determine the specific nature of the system problem.  A document that describes
the possible well system issues associated with each alert is included in this repository.

## Repository Contents.

