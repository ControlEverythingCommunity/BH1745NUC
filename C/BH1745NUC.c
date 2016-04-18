// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// BH1745NUC
// This code is designed to work with the BH1745NUC_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Color?sku=BH1745NUC_I2CS#tabs-0-product_tabset-2

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

void main() 
{
	// Create I2C bus
	int file;
	char *bus = "/dev/i2c-1";
	if ((file = open(bus, O_RDWR)) < 0) 
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
	// Get I2C device, BH1745NUC I2C address is 0x38(56)
	ioctl(file, I2C_SLAVE, 0x38);

	// Select mode control1 register(0x41)
	// RGBC measurement time = 160 ms(0x00)
	char config[2] = {0};
	config[0] = 0x41;
	config[1] = 0x00;
	write(file, config, 2);
	// Select mode control2 register(0x42)
	// RGBC measurement active, 1x gain(0x90)
	config[0] = 0x42;
	config[1] = 0x90;
	write(file, config, 2);
	// Select mode control3 register(0x44)
	// Default value(0x02)
	config[0] = 0x44;
	config[1] = 0x02;
	write(file, config, 2);
	sleep(1);

	// Read 8 bytes of data from register(0x50)
	// red lsb, red msb, green lsb, green msb
	// blue lsb, blue msb, cData lsb, cData msb
	char reg[1] = {0x50};
	write(file, reg, 1);
	char data[8] = {0};
	if(read(file, data, 8) != 8)
	{
		printf("Erorr : Input/output Erorr \n");
	}
	else
	{
		// Convert the data
		int red = (data[1] * 256 + data[0]);
		int green = (data[3] * 256 + data[2]);
		int blue = (data[5] * 256 + data[4]);
		int cData = (data[7] * 256 + data[6]);

		// Output data to screen
		printf("Red color luminance : %d lux \n", red);
		printf("Green color luminance : %d lux \n", green);
		printf("Blue color luminance : %d lux \n", blue);
		printf("Clear Data  Luminance : %d lux \n ", cData);
	}
}
