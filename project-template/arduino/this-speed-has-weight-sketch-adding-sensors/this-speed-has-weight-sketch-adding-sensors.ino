/*
   Arduino Sketch for Plataforma Workshop : This speed has weight
   This code is WIP
   This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
   Nathan Gates 2018
*/

#define DEBUG 1
#define SEALEVELPRESSURE_HPA (1013.25)

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

Adafruit_BME280 bme; // I2C
Adafruit_BNO055 bno = Adafruit_BNO055(55);
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)

int x_orientation, y_orientation, z_orientation = 0;
uint32_t luminosity = 0;
uint16_t irLight, fullLight, lux, visibleLight = 0;
int temp, pressure, altitude, humidity = 0;

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

void setup()
{
  // start serial port at 115200 bps:
  Serial.begin(115200);

  //define feedback led, onboard led on pin13
  pinMode(led, OUTPUT);
  pinMode(mic, INPUT);


  //BNO
  /* Initialise the sensor */


  bool status;
  status = bme.begin();
  if (!status) {
    if (DEBUG) {
      Serial.println("Could not find a valid BME280 sensor, check wiring!");
      //while (1);
    }
  }

  if (!tsl.begin())
  {
    if (DEBUG) {
      Serial.println(F("No TSL2591found ... check your wiring?"));
      //while (1);
    }
  }

  /* Configure the sensor */
  configureSensor();
  
  if (!bno.begin())
  {
    if (DEBUG) {
      Serial.println("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
      //while (1);
    }
  }
}

void loop()
{
  // wait for the node server to contact us
  if (Serial.available() > 0) {
    inString = Serial.readStringUntil('\r');
  }
  //============== READ ==================//
  if (inString == "read") {

    //Read all the sensors and store in variables
    readBNO();
    readMic();
    advancedReadTSL();
    readBme280();
    //Process Data if necesary

    //Send sensor values to nodejs server over serial(USB cable):
    //the entire data packet is split using a comma separator
    //the serialport module uses a built in parser to determine the end of the packet
    //in this case we are use '\r\n' to signal this, that is the println()
    // the data packet will look something like this: "data1, data2, data3\r\n"
    Serial.print(x_orientation);
    Serial.print(',');
    Serial.print(y_orientation);
    Serial.print(',');
    Serial.print(z_orientation);
    Serial.print(',');
    Serial.print(soundLevel);;
    Serial.print(',');
    Serial.print(temp);
    Serial.print(',');
    Serial.print(pressure);
    Serial.print(',');
    Serial.print(altitude);
    Serial.print(',');
    Serial.print(humidity);
    Serial.print(',');
    Serial.print(irLight);
    Serial.print(',');
    Serial.print(visibleLight);
    Serial.print(',');
    Serial.print(fullLight);
    Serial.print(',');
    Serial.println(lux);

    inString = "";
  }

  //============== WRITE ==================//
  if (inString == "write") {
    //read info from nodejs server and do something with that information on the arduino
    //or use these branches to trigger different states instead of just read/write
    //you need to figure out how to incoporate code into this structure
    // or write your own code to handle the serial communication
    //wait abit for the data to arrive
    delay(10);
    //read the data, same structure as before.Reading string untill '\r'
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

      //use this recevied data to do something like toggle the onboard LED
      if (data0 == 12) {
        ledState = !ledState;
        digitalWrite(led, ledState);
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

  /* Board layout:
         +----------+
         |         *| RST   PITCH  ROLL  HEADING
     ADR |*        *| SCL
     INT |*        *| SDA     ^            /->
     PS1 |*        *| GND     |            |
     PS0 |*        *| 3VO     Y    Z-->    \-X
         |         *| VIN
         +----------+
  */

  /* The processing sketch expects data as roll, pitch, heading */
  //  Serial.print(F("Orientation: "));
  //  Serial.print((float)event.orientation.x);
  //  Serial.print(F(" "));
  //  Serial.print((float)event.orientation.y);
  //  Serial.print(F(" "));
  //  Serial.print((float)event.orientation.z);
  //  Serial.println(F(""));
  x_orientation = (int)event.orientation.x;
  y_orientation = (int)event.orientation.y;
  z_orientation = (int)event.orientation.z;

}


/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2591
*/
/**************************************************************************/
void configureSensor(void)
{
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
  //tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain

  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  //tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

  /* Display the gain and integration time for reference sake */
  //  Serial.println(F("------------------------------------"));
  //  Serial.print  (F("Gain:         "));
  tsl2591Gain_t gain = tsl.getGain();
  //  switch (gain)
  //  {
  //    case TSL2591_GAIN_LOW:
  //      Serial.println(F("1x (Low)"));
  //      break;
  //    case TSL2591_GAIN_MED:
  //      Serial.println(F("25x (Medium)"));
  //      break;
  //    case TSL2591_GAIN_HIGH:
  //      Serial.println(F("428x (High)"));
  //      break;
  //    case TSL2591_GAIN_MAX:
  //      Serial.println(F("9876x (Max)"));
  //      break;
  //  }
  //  Serial.print  (F("Timing:       "));
  //  Serial.print((tsl.getTiming() + 1) * 100, DEC);
  //  Serial.println(F(" ms"));
  //  Serial.println(F("------------------------------------"));
  //  Serial.println(F(""));
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
  // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
  // That way you can do whatever math and comparisons you want!
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  //  Serial.print(F("[ ")); Serial.print(millis()); Serial.print(F(" ms ] "));
  //  Serial.print(F("IR: ")); Serial.print(ir);  Serial.print(F("  "));
  //  Serial.print(F("Full: ")); Serial.print(full); Serial.print(F("  "));
  //  Serial.print(F("Visible: ")); Serial.print(full - ir); Serial.print(F("  "));
  //  Serial.print(F("Lux: ")); Serial.println(tsl.calculateLux(full, ir), 6);

  irLight = ir;
  visibleLight = (full - ir);
  fullLight = full;
  lux = (tsl.calculateLux(full, ir), 6);
}

/**************************************************************************/
/*
    Read BME280
*/
/**************************************************************************/
void readBme280(void) {
  temp = bme.readTemperature(); //c
  pressure = bme.readPressure() / 100.0F; //hpa
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);// m
  humidity = bme.readHumidity(); // %
}

