# Periph I2C Test

This application enables you to test all available low-level I2C functions
using the RIOT shell.  Consult the 'help' shell command for available actions.

## Background

Test for the low-level I2C driver.  Can be run manually or using automated test
script for RobotFramework, run `make robot-test`.

## Glossary

DEV - The I2C device number, this is usually 0.  Consult the board header file for more information.</br>
ADDR - The address of the slave I2C device, this is usually the sensor.  Information on the slave address should be found in the sensor datasheet.</br>
REG - The register of the slave device/sensor.  Not all sensors follow this access method.  A I2C_NOSTOP (0x04) may be needed.</br>
LEN - The length to read or write.</br>
FLAG - Flags set for the I2C, more information is available in driver/include/periph/i2c.h
