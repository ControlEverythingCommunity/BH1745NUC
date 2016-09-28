# Distributed with a free-will license.
# Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
# BH1745NUC
# This code is designed to work with the BH1745NUC_I2CS I2C Mini Module available from ControlEverything.com.
# https://www.controleverything.com/content/Color?sku=BH1745NUC_I2CS#tabs-0-product_tabset-2

from OmegaExpansion import onionI2C
import time

# Get I2C bus
i2c = onionI2C.OnionI2C()

# BH1745NUC address, 0x38(56)
# Select mode control register1, 0x41(65)
#		0x00(00)	RGBC measurement time = 160 ms
i2c.writeByte(0x38, 0x41, 0x00)
# BH1745NUC address, 0x38(56)
# Select mode control register2, 0x42(66)
#		0x90(144)	RGBC measurement active, Gain = 1X
i2c.writeByte(0x38, 0x42, 0x90)
# BH1745NUC address, 0x38(56)
# Select mode control register3, 0x44(68)
#		0x02(02)	Default value
i2c.writeByte(0x38, 0x44, 0x02)

time.sleep(0.5)

# BH1745NUC address 0x38(56)
# Read data back from 0x50(80), 8 bytes
# Red LSB, Red MSB, Green LSB, Green MSB, Blue LSB, Blue MSB
# cData LSB, cData MSB 
data = i2c.readBytes(0x38, 0x50, 8)

# Convert the data
red = data[1] * 256 + data[0]
green = data[3] * 256 + data[2]
blue = data[5] * 256 + data[4]
cData = data[7] * 256 + data[6]

# Output data to the screen
print "Red Color Luminance : %d lux" %red
print "Green Color Luminance : %d lux" %green
print "Blue Color Luminance : %d lux" %blue
print "Clear Data Luminance : %d lux" %cData
