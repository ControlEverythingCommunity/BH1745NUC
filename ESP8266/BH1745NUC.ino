// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// BH1745NUC
// This code is designed to work with the BH1745NUC_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Color?sku=BH1745NUC_I2CS#tabs-0-product_tabset-2

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Wire.h>

// I2C address of the BH1745NUC
#define Addr 0x38

const char* ssid = "WiFi";
const char* password = "000@wifi";
int red, green, blue, cData;

ESP8266WebServer server(80);

void handleroot()
{
  unsigned int data[8];

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select mode control register1
  Wire.write(0x41);
  // Set RGBC measurement time 160 msec
  Wire.write(0x00);
  // Stop I2C Transmission
  Wire.endTransmission();

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select mode control register2
  Wire.write(0x42);
  // Set measurement mode is active, gain = 1x
  Wire.write(0x90);
  // Stop I2C Transmission
  Wire.endTransmission();

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select mode control register3
  Wire.write(0x44);
  // Set default value
  Wire.write(0x02);
  // Stop I2C Transmission
  Wire.endTransmission();
  delay(300);


  for (int i = 0; i < 8; i++)
  {
    // Start I2C Transmission
    Wire.beginTransmission(Addr);
    // Select data register
    Wire.write((80 + i));
    // Stop I2C Transmission
    Wire.endTransmission();

    // Request 1 byte of data
    Wire.requestFrom(Addr, 1);

    // Read 8 bytes of data
    // Red lsb, Red msb, Green lsb, Green msb, Blue lsb, Blue msb
    // cData lsb, cData msb
    if (Wire.available() == 1)
    {
      data[i] = Wire.read();
    }
    delay(300);
  }

  // Convert the data
  red = ((data[1] & 0xFF) * 256) + (data[0] & 0xFF);
  green = ((data[3] & 0xFF) * 256) + (data[2] & 0xFF);
  blue = ((data[5] & 0xFF) * 256) + (data[4] & 0xFF);
  cData = ((data[7] & 0xFF) * 256) + (data[6] & 0xFF);

  // Output data to serial monitor
  Serial.print("Red Color luminance : ");
  Serial.print(red);
  Serial.println(" lux");
  Serial.print("Green Color luminance : ");
  Serial.print(green);
  Serial.println(" lux");
  Serial.print("Blue Color luminance : ");
  Serial.print(blue);
  Serial.println(" lux");
  Serial.print("Clear Data Color luminance : ");
  Serial.print(cData);
  Serial.println(" lux");

  // Output data to web server
  server.sendContent
  ("<html><head><meta http-equiv='refresh' content='3'</meta>"
   "<h1 style=text-align:center;font-size:300%;color:blue;font-family:britannic bold;>CONTROL EVERYTHING</h1>"
   "<h3 style=text-align:center;font-family:courier new;><a href=http://www.controleverything.com/ target=_blank>www.controleverything.com</a></h3><hr>"
   "<h2 style=text-align:center;font-family:tahoma;><a href=https://www.controleverything.com/content/Color?sku=BH1745NUC_I2CS#tabs-0-product_tabset-2 \n"
   "target=_blank>BH1745NUC Sensor I2C Mini Module</a></h2>");
  server.sendContent
  ("<h3 style=text-align:center;font-family:tahoma;>Red Color luminance = " + String(red) + " lux");
  server.sendContent
  ("<h3 style=text-align:center;font-family:tahoma;>Green Color luminance = " + String(green) + " lux");
  server.sendContent
  ("<h3 style=text-align:center;font-family:tahoma;>Blue Color luminance = " + String(blue) + " lux");
  server.sendContent
  ("<h3 style=text-align:center;font-family:tahoma;>Clear Data Color luminance = " + String(cData) + " lux");
}

void setup()
{
  // Initialise I2C communication as MASTER
  Wire.begin(2, 14);
  // Initialise serial communication, set baud rate = 115200
  Serial.begin(115200);

  // Connect to WiFi network
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);

  // Get the IP address of ESP8266
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.on("/", handleroot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient();
}

