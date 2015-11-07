/* 
This is a simple project to upload the data by using Arduino&Wifi-shield combination to the server (yeelink);
Here only on sensor is used: DHT22 to measure temperature and humidity.

By Guannan Wang 2015-04-04
*/

#include <SPI.h>
//#include <Wire.h>
//#include <math.h>
#include <WiFi.h>
#include "DHT.h"

//int BH1750address = 0x23;
//byte buff[2];

char ssid[] = "Hu Core";          //  your network SSID (name) 
char pass[] = "19492009";   // your network password

// for yeelink api
#define APIKEY         "b02bd862507690ba1fcc9b4a6622abcc" // replace your yeelink api key here
#define DEVICEID       19187 // replace your device ID
#define SENSORIDT      33802 // replace your sensor ID temperature
#define SENSORIDH      34203 // replace your sensor ID humidity
// for DHTTYPE
#define DHTTYPE DHT22   // DHT 22  (AM2302)

char server[] = "api.yeelink.net";   // name address for yeelink API

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 30*1000; // delay between 2 datapoints, 30s

int sensorPin = A0;
// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(sensorPin, DHTTYPE);

WiFiClient client;

void setup() {
  //Wire.begin();
  // start serial port:
  Serial.begin(57600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  int status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    // don't do anything else:
    while(true);
  } 
  else {
    Serial.println("Connected to wifi");
  }
}

void loop() {
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }  
  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data:
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    // read sensor data, replace with your code
    //int sensorReading = readTMPSensor();
    int sensorReadingT = readDHTSensorT();
    int sensorReadingH = readDHTSensorH();
    Serial.println("sensorReadingT");
    Serial.println(sensorReadingT);
    Serial.println("sensorReadingH");
    Serial.println(sensorReadingH);
    //send data to server
    sendData(sensorReadingT,SENSORIDT);
    sendData(sensorReadingH,SENSORIDH);
  }
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}

// this method makes a HTTP connection to the server:
// this function is for temperature
void sendData(int thisData, int SENSORID) {
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    Serial.println("something");
    client.print("POST /v1.0/device/");
    client.print(DEVICEID);
    Serial.println(DEVICEID);
    client.print("/sensor/");
    client.print(SENSORID);
    Serial.println(SENSORID);
    client.print("/datapoints");
    client.println(" HTTP/1.1");
    client.println("Host: api.yeelink.net");
    client.print("Accept: *");
    client.print("/");
    client.println("*");
    client.print("U-ApiKey: ");
    client.println(APIKEY);
    client.print("Content-Length: ");

    // calculate the length of the sensor reading in bytes:
    // 8 bytes for {"value":} + number of digits of the data:
    int thisLength = 10 + getLength(thisData);
    client.println(thisLength);

    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Connection: close");
    client.println();

    // here's the actual content of the PUT request:
    client.print("{\"value\":");
    client.print(thisData);
    client.println("}");
    Serial.println("MISTAKE");
  }
    else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
   // note the time that the connection was made or attempted:
  lastConnectionTime = millis();
}

// This method calculates the number of digits in the
// sensor reading.  Since each digit of the ASCII decimal
// representation is a byte, the number of digits equals
// the number of bytes:
 int getLength(int someValue) {
  // there's at least one byte:
  int digits = 1;
  // continually divide the value by ten,
  // adding one to the digit count for each
  // time you divide, until you're at 0:
  int dividend = someValue /10;
  while (dividend > 0) {
    dividend = dividend /10;
    digits++;
  }
  // return the number of digits:
  return digits;
} 


// get data from the TMP
int readTMPSensor(){
   int reading = analogRead(sensorPin);  
 
   // converting that reading to voltage, for 3.3v arduino use 3.3
   float voltage = reading * 5.0;
   voltage /= 1024.0; 
 
   // now print out the temperature
   int TMPC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                       //to degrees ((voltage - 500mV) times 100)
   Serial.print(TMPC); 
   Serial.println(" degrees C");
   
   return TMPC;
}

// get data from the DHT22
int readDHTSensorT(){
   int reading = analogRead(sensorPin);  
 
   // converting that reading to voltage, for 3.3v arduino use 3.3
   float h = dht.readHumidity();
   int t = dht.readTemperature();  
   // print out the voltage
   Serial.print("Humidity:"); 
   Serial.println(h);      //to degrees ((voltage - 500mV) times 100)
   Serial.print("Temperature:"); 
   Serial.print(t);
   Serial.println(" degree(C)");   
   return t;
   Serial.println(t);
}

// get data (humidity) from the DHT22
int readDHTSensorH(){
   int reading = analogRead(sensorPin);  
 
   // converting that reading to voltage, for 3.3v arduino use 3.3
   float h = dht.readHumidity();
   int t = dht.readTemperature();  
   // print out the voltage
   Serial.print("Humidity:"); 
   Serial.println(h);      //to degrees ((voltage - 500mV) times 100)
   Serial.print("Temperature:"); 
   Serial.print(t);
   Serial.println(" degree(C)");   
   return h;
   Serial.println(h);
}
  
