// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// BH1745NUC
// This code is designed to work with the BH1745NUC_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Color?sku=BH1745NUC_I2CS#tabs-0-product_tabset-2

#include <Wire.h>

// I2C address of the BH1745NUC
#define Addr 0x38

void setup()
{
    // Initialise I2C communication as MASTER
    Wire.begin();
    // Initialise serial communication, set baud rate = 9600
    Serial.begin(9600);

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
}

void loop()
{
    unsigned int data[8];
    for(int i = 0; i < 8; i++)
    {
        // Start I2C Transmission
        Wire.beginTransmission(Addr);
        // Select data register
        Wire.write((80+i));
        // Stop I2C Transmission
        Wire.endTransmission();
        
        // Request 1 byte of data from the device
        Wire.requestFrom(Addr, 1);
        
        // Read 6 bytes of data
        // Red lsb, Red msb, Green lsb, Green msb, Blue lsb, Blue msb
        // cData lsb, cData msb
        if(Wire.available() == 1)
        {
            data[i] = Wire.read();
        }
        delay(300);
    }

    // Convert the data
    int red = ((data[1] & 0xFF) * 256) + (data[0] & 0xFF);
    int green = ((data[3] & 0xFF) * 256) + (data[2] & 0xFF);
    int blue = ((data[5] & 0xFF) * 256) + (data[4] & 0xFF);
    int cData = ((data[7] & 0xFF) * 256) + (data[6] & 0xFF);
    
    // Output data to dashboard
    Serial.print("Red Color luminance  : ");
    Serial.println(red);
    Serial.print("Green Color luminance : ");
    Serial.println(green);
    Serial.print("Blue Color luminance : ");
    Serial.println(blue);
    Serial.print("Clear Data Color luminance : ");
    Serial.println(cData);
}

