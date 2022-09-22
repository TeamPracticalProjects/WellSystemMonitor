The program WS_Alert_Dev.ino is a test program to perform unit tests on the WSMAlertProcessor library that is included with the firmware
in this repository.  The test firmware is compiled with the library files and flashed to a standalone Particle Photon processor.  A
momentary pushbutton switch is wired to Photon pin D0; the other side of the switch is wired to GND.

The Photon must be USB connected to a host computer and a console (serial monitor; e.g. PuTTy) program must be run while the tests are being performed.
The Particle console should also be opened and the Photon device selected, so as to monitor the WSMAlert publications to the
Particle cloud.

Each press of the button generates a test case which is described in the messages to the serial monitor.  The internal variables in the library
code are also dumped out so that the cause of any test failure can be investigated.  The messages on the serial monitor tell you what the
test result should be (publication of an alert or not -- due to holdoffs).

Each of the seven alerts are tested three times:
#1: the alert condition is forced and an alert should show on the Particle console.
#2: the same alert condition is forced but the holdoff has not been reset, so no alert is generated.
#3: the same alert condition is ofrced but the holdoff period is expired; thus an alert should be generated.

This code is ONLY used for unit testing of the WSMAlertProcessor library and is not part of the deployed Well System Monitor project.
