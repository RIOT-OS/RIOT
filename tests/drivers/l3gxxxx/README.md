# About

This is a manual test application for the driver of the L3GXXXX 3-axis
gyroscope sensor family. It demonstrates the use of a L3Gxxxx sensor.
The different functionalities of the driver are enabled and tested by
using different modules.

# Usage

The used sensor must be specified by setting the variable DRIVER to
`l3gd20h`, `l3gd20`, `l3g4200d_ng`, `i3g4250d` or `a3g4250d` which enables
the corresponding pseudomodule:
```
DRIVER=l3gd20h BOARD=... make -C tests/driver_l3gxxxx
```
If the DRIVER variable is not set, `l3gd20h` is used.

By default, the application will initialize the gyroscope with the default
configuration parameter set as defined in the `l3gxxxx_params.h` file:

Configuration Parameter     | Default value
:---------------------------|:----------------------
Output data rate            | 100 Hz
LPF2 cutoff frequency       | 25 Hz
Full scale                  | 245 dps
Filter selection            | HPF and LPF2 are used
HPF mode                    | Normal with autoreset
HPF cutoff frequency        | 8 Hz

The default configuration uses the I2C interface (module `l3gxxxx_i2c`)
of the sensor and polls the data periodically every 200 ms. If the SPI
interface of the sensor should be used, the module `l3gxxxx_spi` has to
be used:
```
DRIVER=l3gd20h USEMODULE=l3gxxxx_spi BOARD=... make -C tests/driver_l3gxxxx
```

The application is configurable by using different pseudomodules to test
most of the functionalities of the L3Gxxxx driver.

- Module `l3gxxxx_irq_drdy`:
  Instead of polling the sensor regularly for data, the data ready interrupt
  on signal `INT2/DRDY` is used to read the data. If module `l3gxxxx_fifo`
  is used at the same time, the FIFO status interrupts are enabled instead
  of the data ready interrupt. The GPIO pin used for the interrupt signal
  can be overridden, for example:

    ```
    DRIVER=l3gd20h USEMODULE=l3gxxxx_irq_drdy \
    CFLAGS='-DL3GXXXX_INT2_PIN=GPIO_PIN\(0,5\) BOARD=... make -C tests/driver_l3gxxxx
    ```

- Module `l3gxxxx_irq_event`:
  Event interrupt generation is configured according to default
  configuration parameters in the `l3gxxxx_params.h` file to generate
  interrupts on signal `INT1` when the absolute value of the angular rate
  of any axis exceeds the default threshold. The GPIO pin used for the
  interrupt signal can be overridden, for example:

    ```
    DRIVER=l3gd20h USEMODULE=l3gxxxx_irq_event \
    CFLAGS='-DL3GXXXX_INT1_PIN=GPIO_PIN\(0,6\) BOARD=... make -C tests/driver_l3gxxxx
    ```

- Module `l3gxxxx_fifo`:
  Instead of fetching single data samples from the sensor, the FIFO of
  the sensor is enabled. Depending on the use of module `l3gxxxx_irq_drdy`,
  the FIFO state is either polled regularly or FIFO status interrupts on
  signal `INT2/DRDY` are used. All available data samples are read from
  the FIFO with a single burst access.

- Module `l3gxxxx_config`:
  The configuration of the event interrupt generation is changed at
  runtime.
