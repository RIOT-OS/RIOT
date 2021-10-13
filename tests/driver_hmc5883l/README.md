# About

Test application for the HMC5883L 3-axis digital compass

# Usage

The test application demonstrates the use of the HMC5883L. It uses the
default configuration parameters.

- Continuous measurement at a Data Output Rate (DOR) of 15 Hz
- Normal mode, no biasing
- Gain 1090 LSB/Gs
- No averaging of data samples

The application can use different approaches to get new data:

- using the #hmc5883l_read function at a lower rate than the the DOR
- using the data-ready interrupt (**DRDY**), see #hmc5883l_init_int.

To use the data-ready interrupt (**DRDY**), the application has to enable
module `hmc5883l_int` and has to configure the GPIO to which the
interrupt signal is connected. This is done by overrding the default
configuration parameter `HMC5883L_PARAM_INT_PIN` if necessary, for example:

```
USEMODULE=hmc5883l_int CFLAGS='-DHMC5883L_PARAM_INT_PIN=GPIO_PIN\(0,12\)' \
make flash -C tests/driver_hmc5883l BOARD=...
```

The heading angle is calculated for a magnetic declination in radians defined
by `HMC5883L_MAG_DECL`. This depends on the current location. The value for
the current location can be determined at http://www.magnetic-declination.com/.
