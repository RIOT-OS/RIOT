# ST VL6180X Ranging and Ambient Light Sensor

## About

The test application demonstrates the usage of different functions of the
ST VL6180X sensor driver dependent on used driver variant which differ in
functionality and size:

Module Name     | Driver Variant
:---------------|:-------------------------------------------
`vl6180x`       | Driver with standard functionality and larger size
`vl6180x_basic` | Driver with only basic functionality and smaller size

Both driver variants provide

- continuous ranging and ambient light sensing,
- polling for new measurement results, and
- SAUL capabilies.

In addition to these basic functionalities, the standard driver `vl6180x`
provides interrupt handling, single shot measurements, a number of
configuration functions and power down / power up functionality.

See the online documentation for detailed information about the driver.

## Usage

What driver variant is used can be defined at make command line. Using the
make command
```
    make flash -C tests/driver_vl6180x BOARD=...
```
the standard driver variant `vl6180x` is used with the following default
configuration parameters:

- periodic check of the availability of new data every 100 ms
- a ranging inter-measurement period of 100 ms
- a ranging maximum convergence time of 50 ms
- an ambient light sensing (ALS) inter-measurement period of 500 ms
- an ambient light sensing (ALS) integration time of 100 ms
- an ambient light sensing (ALS) analogue light channel gain of 1.0
- an ambient light sensing period of 500 ms
- a ranging period of 100 ms.

To use the basic driver variant, the `vl6180x_basic` module has
to be specified at make command line
```
    USEMODULE=vl6180x_basic make flash -C tests/driver_vl6180x BOARD=...
```
The default configuration parameters used for the test application with the
basic driver variant are the same as for the standard driver variant.

To use interrupts to fetch new data instead of polling for new data
periodically, the standard driver variant `vl6180x` has to be used and the
pin connected with the interrupt signal GPIO1 of the sensor has to be
defined by configuration paramater `VL6180X_PARAM_PIN_INT`, e.g.,
```
    CFLAGS="-DVL6180X_PARAM_PIN_INT=\(GPIO_PIN\(0,1\)\)" \
    make flash -C tests/driver_vl6180x BOARD=...
```
To test the power down and power up functionality, the pin connected with
the signal GPIO0/CE of the sensor has to be defined by configuration
paramater `VL6180X_PARAM_PIN_SHUTDOWN
`, e.g.,
```
    CFLAGS="-DVL6180X_PARAM_PIN_SHUTDOWN=\(GPIO_PIN\(0,2\)\)" \
    make flash -C tests/driver_vl6180x BOARD=...
```
