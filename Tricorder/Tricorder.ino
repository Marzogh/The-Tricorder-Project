//Definitions
#define DEBUG false                             //Permit all debugging code to be run
#define GPSattached false                       //Define if GPS is attached
#define UVSensor false                          //Define if UV Sensor is attached
#define HTU21DFattached true                    //Define if HTU21DF sensor is attached
#define AM2315attached false                    //Define if AM2315 is attached
#define SHT1xattached true                      //Define if SHT1X is attached
#define TSL2591attached false                    //Define if TSL2591 is attached
#define TSL2561attached true                   //Define if TSL2591 is attached
#define RTCattached true                        //Define if RTC is attached
#define LCDattached true                        //Define if LCD is attached
#define SDattached true                         //Define if SD is attached
#define EEPROMrequired false                    //Define id EEPROM is required
#define Neopixel true

//Libraries

#include <Wire.h>                                //For I2C devices

#if GPSattached
#include <TinyGPS++.h>                           //For GPS
#include <SoftwareSerial.h>                      //For GPS
#endif

#if LCDattached
#include <LCD.h>                                 //For LCD display
#include <LiquidCrystal_I2C.h>                   //For LCD display
#endif

#if SDattached
#include <SdFat.h>                               //For SD card
#endif

#if RTCattached
#include <RTClib.h>                              //For RTC
#endif

#include <Adafruit_Sensor.h>                     //For Light sensor

#if TSL2561attached
#include <Adafruit_TSL2561_U.h>                  //For Light sensor
#endif

#if AM2315attached
#include <Adafruit_AM2315.h>                     //For Air Temperature and Humidity sensor
#endif

#if HTU21DFattached
#include <Adafruit_HTU21DF.h>                    //For Air Temperature and Humidity sensor
#endif

#if SHT1xattached
#include <SHT1x.h>                               //For Soil Temperature and Moisture sensor
#endif

#if EEPROMrequired
#include <EEPROM.h>                              //For EEPROM access
#endif

#if Neopixel
#include <Adafruit_NeoPixel.h>                   //For Neopixel
#endif

#include <avr/interrupt.h>                       //For interrupt
#include <avr/power.h>                           //For power control
#include <avr/sleep.h>                           //For sleep functions
#include <avr/io.h>                              //For fine grained IO control

//Variables
boolean sType = 0;
//boolean newRead = LOW;

#if TSL2561attached
uint16_t lux;
#endif

#if TSL2591attached
uint16_t ir, full, vis, uv, lux;
uint32_t lum;
#endif

float soilT, soilH, airT, airH;
float prev_soilT, prev_soilH, prev_airT, prev_airH;
char filename[13] = {0};


//Pins
//const uint8_t RLED = 5;                           //Red LED pin
//const uint8_t GLED = 6;                           //Blue LED pin
//const uint8_t BLED = 9;                           //Green LED pin
const uint8_t chipSelect = 10;                    //SD card chipselect pin
//const uint8_t sampleSelect = 3;                   //Sample selection (in this case - spider or artificial hole) - an SPST switch is connected to this pin
const uint8_t clockPin = A2;                      //Clock pin for Soil sensor (SHT1X)
const uint8_t dataPin = A3;                       //Data pin for Soil sensor (SHT1X)
//const uint8_t BUTTON = 2;

#if GPSattached
static const int RXPin = 7, TXPin = 8;            //TX and RX pins for Software Serial for GPS
static const uint32_t GPSBaud = 9600;             //Set GPS Baud rate
#endif

//Initiate Objects
#if GPSattached
TinyGPSPlus gps;                                  //Initiate TinyGPS object for GPS
#endif
#if LCDattached
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7); //Initiate LiquidCrystal object for LCD. (0x27 is the I2C bus address for an unmodified I2C backpack)
#endif
#if SDattached
SdFat sd;                                         //Initiate sd object for SdFat library
SdFile logfile;                                   //Initiate File object for SD card
#endif
#if RTCattached
RTC_DS1307 rtc;                                   //Initiate RTC object for Real Time Clock
#endif
#if TSL2591attached
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);    //Initiate TSL2591 object for light sensor
#endif
#if TSL2561attached
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345); //Initiate TSL2591 object for light sensor
#endif
#if HTU21DFattached
Adafruit_HTU21DF htu = Adafruit_HTU21DF();        //Initiate HTU21DF object for air sensor
#endif
#if AM2315attached                                //Initiate AM2315 object for air sensor
Adafruit_AM2315 am2315;
#endif
#if SHT1xattached
SHT1x sht1x(dataPin, clockPin);                   //Initiate SHT1X object for soil sensor
#endif
#if Neopixel
#define PIN 4                                     //Define signal pin for Neopixels
#define NUMPIXELS 1                               //Define number of Neopixels
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); //Initiate Neopixel
#endif


//Declarations
#if GPSattached
SoftwareSerial GPS(RXPin, TXPin);                 //Declare Software Serial
//HardwareSerial GPS = Serial1;                     //Only for Arduino Mega/Leonardo
#endif
HardwareSerial debug = Serial;                    //Declare debug from Hardware Serial


void setup()
{
#if DEBUG
  debug.begin(9600);                              //Begin debug Hardware Serial stream
#endif
#if GPSattached
  GPS.begin(GPSBaud);                             //Begins GPS Software Serial stream
#endif
  Wire.begin();                                   //Begin I2C library
#if RTCattached
  rtc.begin();                                    //Begin RTC
#endif

  pinMode (chipSelect, OUTPUT);                   //Set SD chipselect pin to output
  //pinMode (sampleSelect, INPUT_PULLUP);           //Set the sampleSelect pin to INPUT and turn on pull-up resistors for the sampleSelect pin
  pinMode (2, INPUT_PULLUP);                      //Set  pin 2 (interrupt pin 0) to INPUT and turn on pull-up resistors
  pinMode (3, INPUT_PULLUP);                      //Set  pin 3 (interrupt pin 1) to INPUT and turn on pull-up resistors

  //pinMode(RLED, OUTPUT);
  //pinMode(GLED, OUTPUT);
  //pinMode(BLED, OUTPUT);

  //attachInterrupt(0, readButton, FALLING);        //Attach interrupt to pin 2 to detect change of state to FALLING
  //attachInterrupt(1, sampleType, CHANGE);            //Attach interrupt to pin 3 to detect change of state to LOW

  startLCD();                                     //Starts the LCD
  splashScreen();                                 //Shows splash screen
  pixels.begin();                                 //Starts Neopixel
  startSensors();                                 //Starts the sensors
  initialLCDState();
}

void loop()
{
  confirmReading();                                //Take a new reading and confirm the values are correct
  displaySensors(3);                              //Displays readings three times
  sleepNow();
}






