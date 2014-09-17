////  A Home Built Aquarium Controller for a 55 Gallon Saltwater FOWLR (Fish only
////  with live rock).  With this program(Controller), i hope to make it a
////  Reef Tank.  This software was mostly stolen from around the Internet, under 
////  Open Source Licenses, so it shall stay that way.  If I can pick this C+
////  If this code help, Great!  If not, I dont care 
////  either way, I made it for me, by me!  
////  Copy write 2085 "AQA" published by "Kiss My Ass Industries"
////-------------------------------------------------------------------------------------------------
#include <Wire.h> // iC2 connections Pins A4&A5 UNO Pins 19&20 MEGA
#include <DS1307RTC.h> //Holds time when unit has no power
#include <Time.h> //Keeps track of time
#include <TFTLCD.h> // Display "Driver" 
#include <OneWire.h> //OneWire library for Dallas waterproof temp probe
#include <DallasTemperature.h>  //Temp Library includes
#include <DHT.h> //Library for other temp & humidity sensor
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#define ONE_WIRE_BUS 40 
#define TEMPERATURE_PRECISION 12
#define DHTPIN 42  //Temp & Humidity Sensor
#define DHTTYPE DHT11  //Temp & Humidity Sensor
#define LCD_CS A3    //TFT Pin assingments same on Uno and MEGA 
#define LCD_CD A2    //TFT Pin assingments
#define LCD_WR A1    //TFT Pin assingments
#define LCD_RD A0    //TFTPin assingments
#define LCD_RESET A4  //TFT Pin assingments 
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0 
#define WHITE           0xFFFF
#define soft_reset()
#define FLOAT_PIN digitalRead(39)
#define OVERFLOW_PIN digitalRead(38)
#define SUMP_LOW_LED 30
#define	SUMP_HIGH_LED 31
#define SUMP_GOOD_LED 33
#define SYS_LED_RED 13
#define SYS_LED_GREEN 32
TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);  //TFT LCD ".h" PIN ASSIGNMENTS
DHT dht(DHTPIN, DHTTYPE);  //Temp & Humidity Sensor
OneWire oneWire(ONE_WIRE_BUS);  // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire ds(40);  //Data wire is plugged into port 12 on the Arduino
DallasTemperature sensors(&oneWire);  // Pass our oneWire reference to Dallas Temperature.
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);  //LEX Sensor
int airTempF; //Room Air Temp
int airHumidity; //Room Air Humidity %
int tMMin;  //MINUTE ver for printing time
int tMHour;  //HOUR ver for printing time
int tMSec;  //SECOND ver for printing time  
int numberOfDevices; //OneWire
DeviceAddress tempDeviceAddress;		   
//int timer = 50000;           // The higher the number, the slower the timing.
int RelayPinsAUTO[] = { 2, 3, 4, 5, 6, 7, };
// an array of pin numbers to which LEDs are attached
int RelayAutoONpinCount = 5;           // the number of pins (i.e. the length of the array)

///-----------------------------------VOID SETUP---------------------------------------->

void setup() {
  Serial.begin(9600);  //Start Serial Port to display Diag
  Serial.println("A Controller for my aquarium! Dustin Holloway! !");
  Serial1.begin(9600);
  pinMode(52, OUTPUT);
  pinMode(30, OUTPUT);  //sump LOW LED
  pinMode(51, INPUT_PULLUP);  //Button for ManAutofIll
  pinMode(38, INPUT_PULLUP);  //float 2
  pinMode(39, INPUT_PULLUP);  //float 1
  pinMode(31, OUTPUT);  //SUMP HIGH LED
  pinMode(33, OUTPUT);  //SUMP GOOD LED
  pinMode(13, OUTPUT);  //ON BOARD SYSTEM LED RED/BAD
  pinMode(32, OUTPUT);  // SYS GOOD LED
  pinMode(13, OUTPUT); //on board LED PIN
  digitalWrite(SUMP_GOOD_LED, LOW);
  digitalWrite(SUMP_HIGH_LED, LOW);
  digitalWrite(SUMP_LOW_LED, LOW);
  digitalWrite(SYS_LED_GREEN, LOW);
  digitalWrite(SYS_LED_RED, LOW);  
  digitalWrite(52, HIGH);  // AutoFill Pump, best to turn this off right away....
  digitalWrite(13, LOW); //on board LED PIN
  uint16_t time = millis();
  time = millis() - time;
  Serial.println(time, DEC);
  sensors.begin();    //Temperature startup (onewire?)
  numberOfDevices = sensors.getDeviceCount();
  Serial.print("Found ");
  Serial.print(numberOfDevices);
  Serial.println(" devices.");
  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++){
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){
		Serial.print("Found device ");
		Serial.print(i);
		Serial.print(" with address: ");
		printAddress(tempDeviceAddress);
		Serial.println();
		Serial.print("Setting resolution to ");
		Serial.println(TEMPERATURE_PRECISION);
		// set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
		sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
		Serial.print("Resolution actually set to: ");
		Serial.print(sensors.getResolution(tempDeviceAddress)); 
		Serial.println();
	}else{
		Serial.print("Found ghost device at ");
		Serial.print(i);
		Serial.print(" but could not detect address. Check power and cabling");
	}
  }
  tft.reset();    //TTF Reset //TFT RESET IS ON MAIN RESET PIN //Chipset is either ILI9325 or ILI9328...  
  tft.initDisplay(); //TFT 
  dht.begin(); //Temp and humidity INT  
  
 // int RelayNumber;
  //turns ON Auto relays.
  for (int RelayNumber = 0; RelayNumber < RelayAutoONpinCount; RelayNumber++)  {
	  pinMode(RelayPinsAUTO[RelayNumber], OUTPUT);
         // digitalWrite(RelayPinsAUTO[thisPin], HIGH);       
}

	if(!tsl.begin())
	{
		/* There was a problem detecting the ADXL345 ... check your connections */
		Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
		while(1);
	}
	/* Setup the sensor gain and integration time */
	configureSensor();
	/* We're ready to go! */
	Serial.println("");
}

void loop() {
	 
  tiMe();
  // relays();
  airTempsetup();
  waterTempsetup();
  sumpstatus();
  ManAutofill();
  LUX();
  leds();
  statustft();
  statusSerial();
  ManAutofill();
  } 


void RunPrograms(void) { 
  
}
void statusSerial (void) {
	
}  

void sumpstatus (void){
    Serial.print(F("Sump Level Status: "));
	Serial.print(FLOAT_PIN);
	Serial.println(F(" | 1= Healthy, 0= Low/Danger"));	
	digitalRead(39);
	if(FLOAT_PIN == LOW) { 
		digitalWrite(SUMP_LOW_LED, HIGH);
		digitalWrite(SUMP_GOOD_LED, LOW);
		AutoFill(TRUE);
	}
	else{
		if(FLOAT_PIN == HIGH) {
			Serial.println(F("Sump Level OK! :) "));
			digitalWrite(SUMP_GOOD_LED, HIGH);
		} else {	
			if(OVERFLOW_PIN == LOW){
				Serial.println(F("Sump NOT over flowing!  :)"));
				digitalWrite(SUMP_GOOD_LED, HIGH);
			} else {
				if(OVERFLOW_PIN == HIGH){
					  // Call AutoDump to drain water..  Not written yet...
					  Serial.print(F("Sump Over Flow!  Need to shut off intake!"));
					  tft.fillScreen(RED);
					  tft.setRotation(1);
					  tft.setTextSize(3);
					  tft.setCursor(25, 100);
					  tft.setTextColor(YELLOW);
					  tft.println(F("  SUMP OVERFLOW!"));
					  digitalWrite(SUMP_GOOD_LED, LOW);
					  digitalWrite(SUMP_HIGH_LED, HIGH);		  
					  delay(1000);
				}
			}
		}
	}
	digitalWrite(52, HIGH);
	digitalWrite(SUMP_GOOD_LED, HIGH);
	digitalWrite(SUMP_LOW_LED, LOW);
}

void LUX(void) {
	  
  /* Get a new sensor event */ 
  sensors_event_t event;
  tsl.getEvent(&event);
  /* Display the results (light is measured in lux) */
  if (event.light) { Serial.print(event.light); Serial.println(F(" lux"));
  } else {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    Serial.println(F("Sensor overload"));
  }
}

void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print(F("Temp C: "));
  Serial.print(tempC);
  Serial.print(F(" Temp F: "));
  Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
}

void print2digits(int number) {  //im poretty sure this has to do with the time..
  if (number >= 0 && number < 10) {
    Serial.write('0');
    }
  Serial.print(number);          //im poretty sure this has to do with the time..
}

void eRrors(void) {
}

int tiMe(void){ //Get time from DS1307RTC chip
  tmElements_t tm;
  if (RTC.read(tm)) {
    Serial.print(F("Ok, Time = "));
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(F(", Date (M/D/Y) = "));
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println(F(" "));
    tMMin = (tm.Minute);  //define global time variables..  for now....
    tMSec = (tm.Second);  //define global time variables..  for now....
    tMHour = (tm.Hour);   //define global time variables..  for now....
 
}   else {
    if (RTC.chipPresent()) {
      Serial.println(F("The DS1307 is stopped.  Please run the SetTime"));

    } else {
      Serial.println(F("DS1307 read error!  Please check the circuitry."));
      Serial.println(F(""));
    }
  }
}

//void relays(int RelayNumber) {
  //iF(RelayNumber >= 10) {
	  
  //}
 
//}
  
void leds() {

}
 
void airTempsetup() {  //DHT11 Temp & Humidity Sensors
	// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
	float h = dht.readHumidity();
	// Read temperature as Celsius
	float t = dht.readTemperature();
	// Read temperature as Fahrenheit
	float f = dht.readTemperature(true);
	
	// Check if any reads failed and exit early (to try again).
	if (isnan(h) || isnan(t) || isnan(f)) {
		Serial.println(F("Failed to read from DHT sensor!"));
}
	// Compute heat index
	float hi = dht.computeHeatIndex(f, h);  // Must send in temp in Fahrenheit!
	Serial.print(F("Humidity: "));
	Serial.print(h);
	Serial.print(F(" %\t"));
	Serial.print(F("Temperature: "));
	Serial.print(t);
	Serial.print(F(" *C "));
	Serial.print(f);
	Serial.print(F(" *F\t"));
	Serial.print(F("Heat index: "));
	Serial.print(hi);
	Serial.println(F(" *F"));
	airTempF = f;  //Global Ver's I set for quick pull of time, this can be changed to whatever...
	airHumidity = h; 
}

void waterTempsetup() {
  //28CF112706000033
 // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print(F("Requesting temperatures..."));
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println(F("DONE"));
  // Loop through each device, print out temperature data
  for(int i=0;i<numberOfDevices; i++)
  {
   // Search the wire for address
   if(sensors.getAddress(tempDeviceAddress, i))
	{
		// Output the device ID
		Serial.print(F("Temperature for device: "));
		Serial.println(i);
		// It responds almost immediately. Let's print out the data
		printTemperature(tempDeviceAddress); // Use a simple function to print out the data
	} 
	//else ghost device! Check your power requirements and cabling
  }	
  Serial.print(F("Water Temperature Main Tank: "));
Serial.println(sensors.getTempFByIndex(1));
}

void AutoFill(int fillaction) {
	if(fillaction == TRUE) {
		digitalWrite(SUMP_GOOD_LED, LOW);
		digitalWrite(SUMP_LOW_LED, HIGH);
		Serial.print(F( "Starting AutoFill"));
		Serial.print(F( "NOW!"));
	}
		do {
	        tft.fillScreen(BLACK);
			tft.setRotation(1);
			tft.setTextSize(4);
			tft.setCursor(25, 75);
			tft.setTextColor(YELLOW);
			tft.println(F("   FILLING "));
			tft.println(F("      SUMP"));
			pinMode(52, OUTPUT);
			digitalWrite(52, LOW);
			delay(1000);		
} while (digitalRead(39) == LOW);{
		}
	digitalWrite(SUMP_LOW_LED, LOW);
	Serial.print(F("AutoFill Done!  Returning to program"));
   }
				
void ManAutofill() {
	Serial.print(F("Sump Fill Button State: "));
	Serial.print(digitalRead(51));
	if(digitalRead(51) == LOW) {
			Serial.print(F("Sump Fill Button State: "));
			Serial.println(digitalRead(51));
	
		 do 
	 {   digitalWrite(SUMP_GOOD_LED, LOW);
		 digitalWrite(SUMP_LOW_LED, HIGH);
		 Serial.print(F( "Starting AutoFill(Manual) "));
		 Serial.println(F( "NOW!"));
		 pinMode(52, OUTPUT);
		 digitalWrite(52, LOW);
		 tft.fillScreen(BLACK);
		 tft.setRotation(1);
		 tft.setTextSize(4);
		 tft.setCursor(25, 100);
		 tft.setTextColor(YELLOW);
		 tft.println(F("  FILLING "));
		 tft.println(F("     SUMP"));
		 digitalWrite(52, LOW);
		 delay(1000);
	 }
	 while (digitalRead(51) == LOW);{
	}
	delay(20);
	digitalWrite(52, HIGH);
	digitalWrite(SUMP_LOW_LED, LOW);
}
}

int freeRam () {
	extern int __heap_start, *__brkval;
	int v;
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}


void statustft() {  
  tmElements_t tm;
  sensors_event_t event;
  tsl.getEvent(&event);
  
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  tft.setTextSize(3);
  tft.setCursor(15, 0);
  tft.setTextColor(GREEN);
  tft.println(F(" DaH Aqua Control  "));
  if (RTC.read(tm)) {
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print(F("      "));
  tft.print(tMHour);
  tft.print(F(":"));
  tft.print(tMMin);
  tft.print(F(":"));
  tft.print(tMSec);
  tft.print(F(" "));
  tft.print(tm.Day);
  tft.write('/');
  tft.print(tm.Month);
  tft.write('/');
  tft.println(tmYearToCalendar(tm.Year));
  }
  tft.drawLine(0, 45, tft.width()-1, 45, MAGENTA); //draw line separator
  tft.drawLine(0, 46, tft.width()-1, 46, RED); //draw line separator
  tft.drawLine(0, 47, tft.width()-1, 47, GREEN); //draw line separator
  tft.println(F(" "));
  tft.print(F(" System Up Time: "));
  tft.print(millis() / 1000);
  tft.println(F(" S"));
  tft.print(F(" Free Memory: "));
  tft.println(freeRam());
  tft.print(F(" Tank Temp: ")); 
  tft.setTextColor(YELLOW);
  tft.println(  sensors.getTempFByIndex(1));
  tft.setTextColor(WHITE);
  tft.print(F(" Room Temp&Hum: "));
  tft.setTextColor(YELLOW);
  tft.print(airTempF);
  tft.print(F("F @ "));
  tft.print(airHumidity);
  tft.println(F("%"));
  tft.setTextColor(WHITE);
  tft.print(F(" Salinity: "));
  tft.setTextColor(RED);
  tft.println(F(" not installed!"));
  tft.setTextColor(WHITE);
  tft.print(F(" PH: "));
  tft.setTextColor(RED);
  tft.println(F(" not installed!"));
  tft.setTextColor(WHITE);
  tft.print(F(" Fans: "));
  tft.setTextColor(YELLOW);
  tft.println(F("ON"));
  tft.setTextColor(WHITE);
  tft.print(F(" Light Level: "));
  tft.setTextColor(YELLOW);
    if (event.light) { tft.print(event.light); tft.println(F(" lux"));
  } else {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    tft.println(F("Overload"));
  }
  tft.setTextColor(WHITE);
  tft.print(F(" Sump Status: "));
  tft.setTextColor(RED);
  if(FLOAT_PIN == LOW) { 
    tft.setTextSize(3);
    tft.println(F("Low Level!"));
    tft.setTextSize(3);
    tft.println(F(" "));
    tft.println(F("     SUMP LOW!"));  
  }
    if (FLOAT_PIN == HIGH) {
    tft.setTextColor(YELLOW);
    tft.println(F(" Level OK!"));
    tft.setTextSize(3);
  } 
    if (digitalRead(39) > 1.01) {
        tft.setTextColor(RED);
        tft.setTextSize(3);
        tft.println(F(" Unknown?"));
        tft.setTextSize(3);
        tft.println(F(" "));
        tft.println(F("  CHECK SUMP")); 
        Serial.print(F("TFT cant find sump float.. "));
      }
    }

void configureSensor(void)
{
	/* You can also manually set the gain or enable auto-gain support */
	tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
	// tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
	// tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
	
	/* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
	// tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
	tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
	// tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

	/* Update these values depending on what you've set above! */
	Serial.println("------------------------------------");
	Serial.print("Gain:         "); Serial.println("NO GAIN");
	Serial.print("Timing:       "); Serial.println("101MS");
	Serial.println("------------------------------------");
}

	/* Display some basic information on this sensor */
	//displaySensorDetails();
void displaySensorDetails(void)
{
	sensor_t sensor;
	tsl.getSensor(&sensor);
	Serial.println("------------------------------------");
	Serial.print("Sensor:       "); Serial.println(sensor.name);
	Serial.print("Driver Ver:   "); Serial.println(sensor.version);
	Serial.print("Unique ID:    "); Serial.println(sensor.sensor_id);
	Serial.print("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" lux");
	Serial.print("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" lux");
	Serial.print("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" lux");
	Serial.println("------------------------------------");
	Serial.println("");
	delay(500);
}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i]);
  }
}