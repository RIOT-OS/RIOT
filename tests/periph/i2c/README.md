Expected result
===============
This test enables you to test all available low-level I2C functions using the shell.
Consult the `help` shell command for available actions.

Background
==========
Test for the low-level I2C driver.  Can be run manually or using python script interface.

Testing Software I2C
====================

To test the software I2C bus instead of the hardware I2C bus using, say pin
P0.1 for SCL and P0.2 for SDA, run:

```
make BOARD=<BOARD-NAME> SOFT_I2C=1 'PIN_SCL=GPIO_PIN(0,1)' 'PIN_SDA=GPIO_PIN(0,2)' flash term
```

Glossary
==========
DEV - The I2C device number, this is usually 0.
Consult the board header file for more information.</br>
ADDR - The address of the slave I2C device, this is usually the sensor.
Information on the slave address should be found in the sensor datasheet.</br>
REG - The register of the slave device/sensor.
Not all sensors follow this access method.
A I2C_NOSTOP (0x04) may be needed.</br>
LEN - The length to read or write.</br>
FLAG - Flags set for the I2C, more information is available in driver/include/periph/i2c.h

_note: Automated tests can be found in the
[RobotFW-tests](https://github.com/RIOT-OS/RobotFW-tests/tree/master/tests/periph/i2c)
repository_
