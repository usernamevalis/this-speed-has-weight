/*
   Arduino Sketch for Plataforma Workshop : This speed has weight
   This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
   Nathan Gates 2018
*/

//Library includes
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/*BN055
  Connections
   ===========
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 3.3-5V DC
   Connect GROUND to common ground

   History
   =======
   2015/MAR/03  - First release (KTOWN)
*/

/* Set the delay between fresh samples */
Adafruit_BNO055 bno = Adafruit_BNO055(55);
int x, y, z = 0;

int soundSensor = A0;

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

  pinMode(soundSensor, INPUT);

  //BNO
  Serial.println("Orientation Sensor Test"); Serial.println("");

  /* Initialise the sensor */
  if (!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  delay(1000);

  /* Display some basic information on this sensor */
  displaySensorDetails();
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
    int sound = analogRead(0);
    //Process Data if necesary

    //Send sensor values to nodejs server over serial(USB cable):
    //the entire data packet is split using a comma separator
    //the serialport module uses a built in parser to determine the end of the packet
    //in this case we are use '\r\n' to signal this, that is the println()
    // the data packet will look something like this: "data1, data2, data3\r\n"
    Serial.print(x);
    Serial.print(',');
    Serial.print(y);
    Serial.print(',');
    Serial.print(z);
    Serial.print(',');
    Serial.println(sound);
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

void serialTest() {
  digitalWrite(led , 1);
  if (testCounter < 360) {
    testCounter++;
  } else {
    testCounter = 0;
  }
  Serial.println(testCounter);
  inString = "";
  digitalWrite(led, 0);
}

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
  x = (int)event.orientation.x;
  y = (int)event.orientation.y;
  z = (int)event.orientation.z;
}

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor;
  bno.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" xxx");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" xxx");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" xxx");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

