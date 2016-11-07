// Copyright (c) Microsoft. All rights reserved.

using System;
using System.Threading;
using Windows.UI.Xaml.Controls;
using Windows.Devices.Enumeration;
using Windows.Devices.I2c;

namespace BH1745NUC
{
    struct Color
    {
        public double R;
        public double G;
		public double B;
		public double C;
    };

    //	App that reads data over I2C from an BH1745NUC Digital Ambient Light and Color Sensor
    public sealed partial class MainPage : Page
    {
        private const byte COLOR_I2C_ADDR = 0x38;           	//	I2C address of the BH1745NUC
        private const byte COLOR_REG_CONTROL1 = 0x41;  			//	Mode control register 1
        private const byte COLOR_REG_CONTROL2 = 0x42;    			//	 Mode control register 2
		private const byte COLOR_REG_CONTROL3 = 0x44;    			//	 Mode control register 3
        private const byte COLOR_REG_RDATA = 0x50;              //	 Red ADC low data register register
        private const byte COLOR_REG_GDATA = 0x52;              //	 Green ADC low data register register
		private const byte COLOR_REG_BDATA = 0x54;              //	 Blue ADC low data register register
		private const byte COLOR_REG_CDATA = 0x56;              //	Clear ADC low data register register

        private I2cDevice I2CColor;
        private Timer periodicTimer;

        public MainPage()
        {
            this.InitializeComponent();

            //	Register for the unloaded event so we can clean up upon exit
            Unloaded += MainPage_Unloaded;

            //	Initialize the I2C bus, Digital Ambient Light and Color Sensor, and timer
            InitI2CColor();
        }

        private async void InitI2CColor()
        {
            string aqs = I2cDevice.GetDeviceSelector();             //	Get a selector string that will return all I2C controllers on the system
            var dis = await DeviceInformation.FindAllAsync(aqs);    //	Find the I2C bus controller device with our selector string
            if (dis.Count == 0)
            {
                Text_Status.Text = "No I2C controllers were found on the system";
                return;
            }

            var settings = new I2cConnectionSettings(COLOR_I2C_ADDR);
            settings.BusSpeed = I2cBusSpeed.FastMode;
            I2CColor = await I2cDevice.FromIdAsync(dis[0].Id, settings);    // Create an I2C Device with our selected bus controller and I2C settings
            if (I2CColor == null)
            {
                Text_Status.Text = string.Format(
                    "Slave address {0} on I2C Controller {1} is currently in use by " +
                    "another application. Please ensure that no other applications are using I2C.",
                    settings.SlaveAddress,
                    dis[0].Id);
                return;
            }

            /*
				Initialize the Digital Ambient Light and Color Sensor:
				For this device, we create 2-byte write buffers:
				The first byte is the register address we want to write to.
				The second byte is the contents that we want to write to the register.
			*/
            byte[] WriteBuf_Cntr1 = new byte[] { COLOR_REG_CONTROL1, 0x00 };		// 0x00 sets RGBC measurement time = 160 ms
            byte[] WriteBuf_Cntr2 = new byte[] { COLOR_REG_CONTROL2, 0x90 };		// 0x90 sets RGBC measurement active Gain = 1X
            byte[] WriteBuf_Cntr3 = new byte[] { COLOR_REG_CONTROL3, 0x02 };		// 0x02 sets default value

            //	Write the register settings
            try
            {
                I2CColor.Write(WriteBuf_Cntr1);
                I2CColor.Write(WriteBuf_Cntr2);
                I2CColor.Write(WriteBuf_Cntr3);

            }
            // If the write fails display the error and stop running
            catch (Exception ex)
            {
                Text_Status.Text = "Failed to communicate with device: " + ex.Message;
                return;
            }

            //	Create a timer to read data every 300ms
            periodicTimer = new Timer(this.TimerCallback, null, 0, 300);
        }

        private void MainPage_Unloaded(object sender, object args)
        {
            //	Cleanup
            I2CColor.Dispose();
        }

        private void TimerCallback(object state)
        {
            string rText, gText,bText, cText;
            string addressText, statusText;

            //	Read and format Digital Ambient Light and Color Sensor data
            try
            {
                Color color = ReadI2CColor();
                addressText = "I2C Address of the Digital Ambient Light and Color Sensor BH1745NUC: 0x39";
                rText = String.Format("Red Color Luminance: {0:F0} lux", color.R);
                gText = String.Format("Green Color Luminance: {0:F0} lux", color.G);
				bText = String.Format("Blue Color Luminance: {0:F0} lux", color.B);
				cText = String.Format("Clear Data Luminance: {0:F0} lux", color.C);
                statusText = "Status: Running";
            }
            catch (Exception ex)
            {
                rText = "Red Color Luminance: Error";
                gText = "Green Color Luminance: Error";
				bText = "Blue Color Luminance: Error";
				cText = "Clear Data Luminance Error";
                statusText = "Failed to read from Digital Ambient Light and Color Sensor: " + ex.Message;
            }

            //	UI updates must be invoked on the UI thread
            var task = this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                Text_Red_Color_Luminance.Text = rText;
                Text_Green_Color_Luminance.Text = gText;
				Text_Blue_Color_Luminance.Text = bText;
				Text_Clear_Data_Luminance.Text = cText;
                Text_Status.Text = statusText;
            });
        }

        private Color ReadI2CColor()
        {
            byte[] RegAddrBuf = new byte[] { COLOR_REG_RDATA };     //	Read data from the register address
            byte[] ReadBuf = new byte[8];                       //	We read 8 bytes sequentially to get all 4 two-byte color registers in one read

            /*
				Read from the Digital Ambient Light and Color Sensor 
				We call WriteRead() so we first write the address of the Red color I2C register, then read all 4 colors
			*/
            I2CColor.WriteRead(RegAddrBuf, ReadBuf);

            /*
				In order to get the raw 16-bit data values, we need to concatenate two 8-bit bytes from the I2C read for each color.
			*/
			
			ushort ColorRawR = (ushort)(ReadBuf[0] & 0xFF);
            ColorRawR |= (ushort)((ReadBuf[1] & 0xFF) * 256);
            ushort ColorRawG = (ushort)(ReadBuf[2] & 0xFF);
            ColorRawG |= (ushort)((ReadBuf[3] & 0xFF) * 256);
			ushort ColorRawB = (ushort)(ReadBuf[4] & 0xFF);
            ColorRawB |= (ushort)((ReadBuf[5] & 0xFF) * 256);
			ushort ColorRawC = (ushort)(ReadBuf[6] & 0xFF);
            ColorRawC |= (ushort)((ReadBuf[7] & 0xFF) * 256);


            Color color;
            color.R = ColorRawR;
            color.G = ColorRawG;
			color.B = ColorRawB;
			color.C = ColorRawC;

            return color;
        }
    }
}

