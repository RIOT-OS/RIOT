# About

This is a test application for the HD44780 LCD driver. This display comes with
many Arduino starter kits under the name of LCM1602C, and typically has 16x2
columns and rows.

This examples covers the I2C mode of the HD44780 driver. For the parallel mode,
please refer to `tests/drivers/hd44780`.

# Details

This test application will initialize the HD44780 driver with the configuration
as specified in the default `hd44780_params.h` file.

The HD44780 compatible displays are often available with an I2C interface,
which is realized with an I2C port expander connected to the parallel
interface. Typically these I2C add-on boards feature a PCF8574(A) or PCF8575
chip with selectable I2C addresses.
 - PCF8574: Address Range from 0x20 to 0x27
 - PCF8574A: Address Range from 0x38 to 0x3F
 - PCF8575: Address Range from 0x20 to 0x27

Please specify the chip present on your board and the according I2C address
with a command such as this:

```sh
DRIVER=pcf8575 ADDRESS=0x27 BOARD=... make ...
```

This would select the PCF8575 IO expander with the I2C address 0x27.
If no chip is selected, the build system sets the PCF8574 with address 0x20 as
default.

If you don't know which base address the board has, you can use the
`tests/periph/i2c` test program and perform an I2C bus scan. This scan reveals
all devices connected to the I2C bus and their respective addresses.
