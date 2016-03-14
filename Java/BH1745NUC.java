// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// BH1745NUC
// This code is designed to work with the BH1745NUC_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Color?sku=BH1745NUC_I2CS#tabs-0-product_tabset-2

import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CDevice;
import com.pi4j.io.i2c.I2CFactory;
import java.io.IOException;

public class BH1745NUC
{
	public static void main(String args[]) throws Exception
	{
		// Create I2C bus
		I2CBus Bus=I2CFactory.getInstance(I2CBus.BUS_1);
		// Get I2C device, BH1745NUC I2C address is 0x38(56)
		I2CDevice device = Bus.getDevice(0x38);

		// Select mode control1 register
		// RGBC measurement time = 160 ms
		device.write(0x41, (byte)0x00);
		// Select mode control2 register
		// RGBC measurement active, 1x gain
		device.write(0x42, (byte)0x90);
		// Select mode control3 register
		// Default value
		device.write(0x44, (byte)0x02);
		Thread.sleep(500);

		// Read 8 Bytes of Data from address 0x50(80)
		// red lsb, red msb, green lsb, green msb
		// blue lsb, blue msb, cData lsb, cData msb
		byte[] data = new byte[8];
		device.read(0x50, data, 0, 8);

		// Convert data 
		int red = (((data[1] & 0xFF) * 256) + (data[0] & 0xFF));
		int green = (((data[3] & 0xFF) * 256) + (data[2] & 0xFF));
		int blue = (((data[5] & 0xFF) * 256) + (data[4] & 0xFF));
		int cData = (((data[7] & 0xFF) * 256) + (data[6] & 0xFF));

		// Output data to Screen
		System.out.printf("Red Color Luminance : %d lux %n", red);
		System.out.printf("Green Color Luminance : %d lux %n", green);
		System.out.printf("Blue Color Luminance : %d lux %n", blue);
		System.out.printf("Clear Data  Luminance : %d lux %n ", cData);
	}
}

