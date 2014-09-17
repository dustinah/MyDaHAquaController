/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino Mega 2560 or Mega ADK, Platform=avr, Package=arduino
*/

#define __AVR_ATmega2560__
#define ARDUINO 101
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
#define __cplusplus
extern "C" void __cxa_pure_virtual() {;}

//
//
void RunPrograms(void);
void statusSerial (void);
void sumpstatus (void);
void LUX(void);
void printTemperature(DeviceAddress deviceAddress);
void print2digits(int number);
void eRrors(void);
int tiMe(void);
void leds();
void airTempsetup();
void waterTempsetup();
void AutoFill(int fillaction);
void ManAutofill();
int freeRam ();
void statustft();
void configureSensor(void);
void displaySensorDetails(void);
void printAddress(DeviceAddress deviceAddress);

#include "C:\Program Files\Arduino\hardware\arduino\variants\mega\pins_arduino.h" 
#include "C:\Program Files\Arduino\hardware\arduino\cores\arduino\arduino.h"
#include "Z:\Documents\Arduino\DHAQAVER0_003ALPHA\DHAQAVER0_003ALPHA.ino"
