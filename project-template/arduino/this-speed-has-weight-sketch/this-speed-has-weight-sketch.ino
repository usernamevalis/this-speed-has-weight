/*
   Arduino Sketch for Plataforma Workshop : This speed has weight
   This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
   Nathan Gates 2018
*/

//Variables for sensors - analog  i/o for now. will update to i2c/spi interfaces when I have the sensors
int sensor0 = A0;
int sensor1 = A1;
int sensor2 = A2;

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

  //define sensor inputs
  pinMode(sensor0, INPUT);
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);

  //define feedback led, onboard led on pin13
  pinMode(led, OUTPUT);
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
    int sensorData0 = analogRead(sensor0);
    int sensorData1 = analogRead(sensor1);
    int sensorData2 = analogRead(sensor2);

    //Process Data if necesary

    //Send sensor values to nodejs server over serial(USB cable):
    //the entire data packet is split using a comma separator
    //the serialport module uses a built in parser to determine the end of the packet
    //in this case we are use '\r\n' to signal this, that is the println()
    // the data packet will look something like this: "data1, data2, data3\r\n"
    Serial.print(sensorData0);
    Serial.print(',');
    Serial.print(sensorData1);
    Serial.print(',');
    Serial.println(sensorData2);
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

