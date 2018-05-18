/*
   Arduino Sketch for Plataforma Workshop : This speed has weight
   This code is WIP and was created ad hoc - there is lots of leftovers
   This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
   Nathan Gates 2018

   still to do - add api to turn on / off certain sensors over serial, then create an interface for control on the webpage

*/


#define DEBUG 0
#define BNO 1
#define BME 1
#define TSL 1
#define GPS 1
#define BUTTONS 1
//i have removed the section where i actually transfer the data over serial.
//this doesnt work well as sampling delays data transer. I have in the function for posterity
#define MIC 0

//Library includes
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Adafruit_BME280.h>

//Software I2C
//#include "Adafruit_TSL2591.h"
#define SCL_PORT PORTC
#define SCL_PIN 3 //A3
#define SDA_PORT PORTC
#define SDA_PIN 2 //A2
#include <SoftI2CMaster.h>
#include "Adafruit_TSL2591Soft.h"
#include <TinyGPS++.h> // Include the TinyGPS++ library
#include <SoftwareSerial.h>

#define GPS_BAUD 9600 // GPS module baud rate. GP3906 defaults to 9600.
#define ARDUINO_GPS_RX 4 // GPS TX, Arduino RX pin
#define ARDUINO_GPS_TX 3 // GPS RX, Arduino TX pin
#define gpsPort ssGPS  // Alternatively, use Serial1 on the Leonardo
#define SerialMonitor Serial
#define SEALEVELPRESSURE_HPA (1013.25)

TinyGPSPlus tinyGPS; // Create a TinyGPSPlus object
SoftwareSerial ssGPS(ARDUINO_GPS_TX, ARDUINO_GPS_RX); // Create a SoftwareSerial

Adafruit_BME280 bme; // I2C
Adafruit_BNO055 bno = Adafruit_BNO055(55);
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)

int x_orientation, y_orientation, z_orientation = 0;
int irLight, fullLight, lux, visibleLight = 0;
int temp, pressure, alt, humidity = 0;
double latitude, longitude = 0;
int sats, altGps, speedKm = 0;
int transmissionDelay = 50;

//microphone
float filtered = 0.0;
float filteredPrev = 0.0;
int mic = A0;
double soundLevel = 0;
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

//Variables for outputs
int led = 13;
boolean ledState = false;

//other variables
String inString;
int testCounter = 0;

// BUTTON code
int button1 = 2;
int button1State = 0;
int button2 = 3;
int button2State = 0;
int button3 = 4;
int button3State = 0;
int button4 = 5;
int button4State = 0;
int button5 = 6;
int button5State = 0;

void setup()
{
  // start serial port at 115200 bps:
  Serial.begin(115200);
  gpsPort.begin(GPS_BAUD);

  //define feedback led, onboard led on pin13
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  setupSensors();
  digitalWrite(led, LOW);
  Serial.println("ready");

  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
  pinMode(button5, INPUT);
}

void loop()
{
  //  updateGps();

  if (Serial.available() > 0) {
    inString = Serial.readStringUntil('\r');
  }
  if (inString == "r")  {
    serialFlush();
    delay(transmissionDelay);
    if (BNO) {
      readBNO();
    }
    if (GPS) {
      updateGps();
    }
    if (MIC) {
      readMic();
    }
    if (TSL) {
      advancedReadTSL();
    }
    if (BME) {
      readBme280();
    }
    if (BUTTONS) {
      readButtons();
    }
    //Read all the sensors and store in variables
    Serial.print(x_orientation);
    Serial.print(',');
    Serial.print(y_orientation);
    Serial.print(',');
    Serial.print(z_orientation);
    Serial.print(',');
    Serial.print(temp);
    Serial.print(',');
    Serial.print(pressure);
    Serial.print(',');
    Serial.print(alt);
    Serial.print(',');
    Serial.print(humidity);
    Serial.print(',');
    Serial.print(irLight);
    Serial.print(',');
    Serial.print(lux);
    Serial.print(',');
    if (GPS) {
      Serial.print(latitude, 6);
      Serial.print(',');
      Serial.print(longitude, 6);
    } else {
      Serial.print(0);
      Serial.print(',');
      Serial.print(0);
    }
    Serial.print(',');
    Serial.print(altGps);
    Serial.print(',');
    Serial.print(speedKm);
    Serial.print(',');
    Serial.print(sats);
    if (BUTTONS) {
      Serial.print(button1State);
      Serial.print(',');
      Serial.print(button2State);
      Serial.print(',');
      Serial.print(button3State);
      Serial.print(',');
      Serial.print(button4State);
      Serial.print(',');
      Serial.print(button5State);
    }
    Serial.println();
    inString = "";
  }

  //============== WRITE ==================//
  if (inString == "write") {
    delay(10);
    if (Serial.available()) {
      String inValue = Serial.readStringUntil('\r');

      //seperate values at comma;
      int commaIndex = inValue.indexOf(',');
      //  Search for the next comma just after the first
      int secondCommaIndex = inValue.indexOf(',', commaIndex + 1);

      String firstValue = inValue.substring(0, commaIndex);
      String secondValue = inValue.substring(commaIndex + 1, secondCommaIndex);
      String thirdValue = inValue.substring(secondCommaIndex + 1); // To the end of the string

      //convert String to ints
      int data0 = firstValue.toInt();
      int data1 = secondValue.toInt();
      int data2 = thirdValue.toInt();

      if (firstValue == "transDelay") {
        transmissionDelay = data1;
        Serial.println(transmissionDelay);
      }
      //clear string like above
      inString = "";
    }
  }
}


/**************************************************************************/
/*
    Read BNO055
*/
/**************************************************************************/
void readBNO() {
  /* Get a new sensor event */
  sensors_event_t event;
  bno.getEvent(&event);

  x_orientation = (int)event.orientation.x;
  y_orientation = (int)event.orientation.y;
  z_orientation = (int)event.orientation.z;

}



/**************************************************************************/
/*
    Read Microphone
*/
/**************************************************************************/
void readMic() {
  unsigned long startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(0);
    if (sample < 1024)  // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;  // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  soundLevel = (peakToPeak * 5.0) / 1024;  // convert to volts
}


/**************************************************************************/
/*
    Read IR and Full Spectrum at once and convert to lux
*/
/**************************************************************************/
void advancedReadTSL(void)
{

  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;

  irLight = ir;
  //visibleLight = (full - ir);
  //fullLight = full;
  lux = tsl.calculateLux(full, ir);
}

/**************************************************************************/
/*
    Read BME280
*/
/**************************************************************************/
void readBme280(void) {
  temp = bme.readTemperature(); //c
  pressure = bme.readPressure() / 100.0F; //hpa
  alt = bme.readAltitude(SEALEVELPRESSURE_HPA);// m
  humidity = bme.readHumidity(); // %
}

/**************************************************************************/
/*
    Read GPS and associated helper functions
*/
/**************************************************************************/
void updateGps() {
  while (gpsPort.available()) {
    tinyGPS.encode(gpsPort.read()); // Send it to the encode function
  }
  latitude = tinyGPS.location.lat();
  longitude = tinyGPS.location.lng();
  altGps = tinyGPS.altitude.meters();
  speedKm = tinyGPS.speed.kmph();
  sats = tinyGPS.satellites.value();
}

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2591
*/
/**************************************************************************/
void configureTslSensor(void)
{
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
  //tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain

  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  //tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

  /* Display the gain and integration time for reference sake */
  //  Serial.println(F("------------------------------------"));
  //  Serial.print  (F("Gain:         "));
  tsl2591Gain_t gain = tsl.getGain();
}


/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensorBNO;
  bno.getSensor(&sensorBNO);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensorBNO.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensorBNO.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensorBNO.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensorBNO.max_value); Serial.println(" xxx");
  Serial.print  ("Min Value:    "); Serial.print(sensorBNO.min_value); Serial.println(" xxx");
  Serial.print  ("Resolution:   "); Serial.print(sensorBNO.resolution); Serial.println(" xxx");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);

  sensor_t sensorTSL;
  tsl.getSensor(&sensorTSL);
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Sensor:       ")); Serial.println(sensorTSL.name);
  Serial.print  (F("Driver Ver:   ")); Serial.println(sensorTSL.version);
  Serial.print  (F("Unique ID:    ")); Serial.println(sensorTSL.sensor_id);
  Serial.print  (F("Max Value:    ")); Serial.print(sensorTSL.max_value); Serial.println(F(" lux"));
  Serial.print  (F("Min Value:    ")); Serial.print(sensorTSL.min_value); Serial.println(F(" lux"));
  Serial.print  (F("Resolution:   ")); Serial.print(sensorTSL.resolution, 4); Serial.println(F(" lux"));
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
  delay(500);
}
void serialFlush() {
  while (Serial.available() > 0) {
    char t = Serial.read();
  }
}

void setupSensors() {
  pinMode(mic, INPUT);
  //BNO
  if (BNO) {
    if (!bno.begin())
    {
      if (DEBUG) {
        Serial.println("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        //while (1);
      }
    }
  }
  /* Initialise the sensor */
  if (BME) {
    bool status;
    status = bme.begin();
    if (!status) {
      if (DEBUG) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        //while (1);
      }
    }
  }

  if (TSL) {
    if (!tsl.begin())
    {
      if (DEBUG) {
        Serial.println(F("No TSL2591found ... check your wiring?"));
        //while (1);
      }
    }
    configureTslSensor();
  }
}

void readButtons() {
  button1State = digitalRead(button1);
  button2State = digitalRead(button2);
  button3State = digitalRead(button3);
  button4State = digitalRead(button4);
  button5State = digitalRead(button5);
}

