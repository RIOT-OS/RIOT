# About

This is a manual test application for the InvenSense ITG320X 3-axis gyroscope driver.

# Usage

The test application demonstrates the use of the ITG320X. It uses the
default configuration parameters:

- Low pass filter bandwidth (LPF_BW) of 5 Hz (`ITG320X_LPF_BW_5`)
- Internal sample rate (ISR) of 1 kHz which results from LPF_BW of 5 Hz
- Internal sample rate divider (ISR_DIV) of 99 (`ITG320X_PARAM_ISR_DIV`)
- Output data rate (ODR) of 10 Hz resulting from ODR = ISR / (ISR_DIV + 1)

The application may use two different approaches to retrieve new data,
either

- periodically fetching the data at a rate lower than the sensor's
  output data rate (ODR), or
- fetching the data when the data-ready interrupt is triggered.

To use the latter approach, module `itg320x_int` has to be enabled and the
GPIO to which the sensor's **INT** output pin is connected has to be
defined by #ITG320X_PARAM_INT_PIN, for example:

```
USEMODULE=itg320x_int CFLAGS="-DITG320X_PARAM_INT_PIN=\(GPIO_PIN\(0,3\)\)" \
make flash -C tests/driver_itg320x BOARD=...
```
