# About
This test application is created for testing/demonstrating the driver for
QMC5883L magnetic sensors.

# Configuration
This test uses the driver's default configuration as provided in
`drivers/qmc5883l/include/qmc5883l_params.h`. You can override
selected values from the command line or your application like so:
```
$ CFLAGS="-DQMC5883L_PARAM_OSR=QMC5883L_OSR_128" make all
```

The data ready (DRDY) interrupt pin and handling is disabled per default. If
you which to use it, you have to specify the MCU pin that is connected to the
DRDY pin for your QMC5883L sensor using the `QMC5883L_PARAM_PIN_DRDY`
configuration option.

Independent of the ping configuration, the according submodule `qmc5883l_int` is
always includes. This way, the test application will automatically use
interrupts instead of polling to read the sensor data whenever a specific pin
(anything other than `GPIO_UNDEF`) is configured.

# Usage
Flash this application to any board you have connected a QMC5883L sensor to.
When starting, this application will continuously read sensor data and print
them to STDIO.
