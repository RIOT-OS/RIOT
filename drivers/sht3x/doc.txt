/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
@defgroup    drivers_sht3x   SHT3x Humidity and Temperature Sensor Series
@ingroup     drivers_sensors
@ingroup     drivers_saul
@brief       Device Driver for Sensirion SHT30/SHT31/SHT35 Humidity and Temperature Sensors

# Driver for SHT3x Digital Temperature and Humidity Sensors

The driver is for the usage with [RIOT-OS](https://github.com/RIOT-OS/RIOT). It supports the Sensirion SHT30, SHT31, and SHT35 digital temperature and humidity sensors.

## About the sensors

SHT3x are a digital temperature and humidity sensors that use an I2C interface with up to 1 MHz communication speed. They can operate with **three levels of repeatability** (#sht3x_repeat_t)

- low,
- medium, and
- high,

and in two different modes (#sht3x_mode_t)

- the **single shot data acquisition mode** (or short **single shot mode**), and
- the **periodic data acquisition mode** (or short **periodic mode**).

In periodic mode, the SHT3x sensors support an **Alert Mode**, which allows to define temperature and humidity limits and indicate an alert by a dedicated ALERT pin or a status flag if any of the limits is reached reached.

@note The Alert Mode is not yet supported.

## Measurement process

Once a SHT3x sensor is initialized using the ::sht3x_init function, it can be used for measurements.

### Single shot mode

In **single shot mode**, a measurement command triggers the acquisition of **exactly one data pair**. Each data pair consists of temperature and humidity as 16-bit decimal values.

Due to the measurement duration of up to 16 ms, the measurement process is separated into steps to avoid blocking of the system during measurements:

1. Send a measurement command to the sensor to start exactly one single measurement.
2. Wait for the sensor to complete the measurement depending on the configured repeatability.
3. Fetch the results.

For convenience the high level function ::sht3x_read comprises all three steps above in only one function to perform a measurement.

@note Since in single-shot measurement mode the measurement is only started when ::sht3x_read function is called, it delays the calling task up to 16 ms, depending on the configured repeatability, in order to wait for the results of the measurement.

The advantage of this mode is that the sensor can switch between successive measurements into the sleep mode, which is more energy-efficient. This is particularly useful when the measurement rate is less than 1 measurement per second.

### Periodic mode

In this mode, one issued measurement command yields a stream of data pairs. Each data pair consists again of temperature and humidity as 16-bit decimal values. As soon as the measurement command has been sent to the sensor, it automatically performs measurements **periodically at a rate of 0.5, 1, 2, 4 or 10 measurements per second (mps)**. The data pairs can be fetched with the same rate or a lower rate.

As in *single shot mode*, the measurement process is separated into the following steps:

1. Send a measurement command to the sensor to start periodic measurements.
2. Wait for the sensor to complete the measurement depending on the configured repeatability and measurement period.
3. Fetch the results.

While periodic measurement is started (step 1) already in the ::sht3x_init function, the application has to call the ::sht3x_read function on a regular base to fetch the results from last periodic measurement. Possible periodic measurement modes are defined by #sht3x_mode_t.

@note The ::sht3x_read function should be called less frequently than the periodic measurements are performed. Otherwise, the function implicitly delays the calling task until the results of the next measurement cycle become available to avoid data underrun and sensor blocking.

## Measurement results

Once new measurement results are available, the ::sht3x_read function

- fetches raw sensor data in 16-decimal format
- checks the CRC checksums, and
- converts raw data to measured data, and
- returns the measured data.

These measured data are:

- the temperature given in hundredths of degree Celsius
- the relative humidity in hundredths of percents

For either the \p temp or \p hum parameter also ```NULL``` can be passed, if only one value is of interest.

## Error Handling

All driver functions return 0 on success or one of a negative error code defined by #sht3x_error_codes.

## Repeatability

The SHT3x sensor supports **three levels of repeatability** (low, medium and
high). The stated repeatability is 3 times the standard deviation of multiple
consecutive measurements at the stated repeatability and at constant ambient
conditions. It is a measure for the noise on the physical sensor output.
Different measurement modes allow for high/medium/low repeatability.
[Datasheet SHT3x-DIS]
(https://developer.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Datasheets/Sensirion_Humidity_Sensors_SHT3x_Datasheet_digital.pdf)

The repeatability settings influences the measurement duration as well as the power consumption of the sensor. The measurement takes typically 2.5 ms with low repeatability, 4.5 ms with medium repeatability and 12.5 ms with high repeatability. That is, the measurement produces a noticeable delay in execution.

Reapeatability  | T typ.  | RH typ. | Maximum Measurement Duration
:---------------|:-------:|:-------:|:---------------------------:
high            | 0.04    | 0.08    | 15.5 ms
medium          | 0.08    | 0.15    | 6.5 ms
low             | 0.15    | 0.21    | 4.5 ms

While the sensor measures at the lowest repeatability, the average current consumption is 800 μA. That is, the higher the repeatability level, the longer the measurement takes and the higher the power consumption. The sensor consumes only 0.2 μA in standby mode.

Possible repeatabilities are defined by #sht3x_repeat_t.

## Usage

Before using the SHT3x driver, the configuration has to be defined and the ::sht3x_init function needs to be called to initialize and configure the sensor device of type #sht3x_dev_t.

The structure of a configuration is defined in #sht3x_params_t and consists of the following parameters:

Parameter        | Symbol               | Default
:----------------|:---------------------|:------------------
I2C device       | SHT3X_PARAM_I2C_DEV  | I2C_DEV(0)
I2C address      | SHT3X_PARAM_I2C_ADDR | SHT3X_I2C_ADDR_2
Measurement mode | SHT3X_PARAM_MODE     | SHT3X_PERIODIC_2_MPS
Repeatability    | SHT3X_PARAM_REPEAT   | SHT3X_HIGH

The default configuration is defined ```sht3x_params.h``` and can be overridden by the application before including ```sht3x_params.h```.

Example:
```
#define SHT3X_PARAM_MODE        (SHT3X_SINGLE_SHOT)
#define SHT3X_PARAM_REPEAT      (SHT3X_MEDIUM)
...
#include "sht3x.h"
#include "sht3x_params.h"
...
```

The ::sht3x_init function has to be called for each SHT3x sensor to initialize the sensor and to check its availability as well as its error state.

```
sht3x_dev_t dev;

if ((res = sht3x_init(&dev, &sht3x_params[0])) != SHT3X_OK) {
    ... /* error handling */
}
```

Parameter ```sht3x_params``` is also defined by default in file ```sht3x_params.h``` and can be overridden by the application before including ```sht3x_params.h```.

Example:
```
...
#include "sht3x.h"
#include "sht3x_params.h"
...
#define SHT3X_PARAMS    { .i2c_dev  = I2C_DEV(0),          \
                          .i2c_addr = SHT3X_I2C_ADDR_1,    \
                          .mode     = SHT3X_PERIODIC_2_MPS, \
                          .repeat   = SHT3X_HIGH           \
                        }, \
                        { .i2c_dev  = I2C_DEV(0),          \
                          .i2c_addr = SHT3X_I2C_ADDR_2,    \
                          .mode     = SHT3X_SINGLE_SHOT,   \
                          .repeat   = SHT3X_LOW            \
                        }, \
...
sht3x_dev_t dev1;
sht3x_dev_t dev2;

if ((res = sht3x_init(&dev1, &sht3x_params[0])) != SHT3X_OK) {
    ... /* error handling */
}

if ((res = sht3x_init(&dev2, &sht3x_params[1])) != SHT3X_OK) {
    ... /* error handling */
}

```
In this example the configuration for two devices is defined.

Once the sensor devices are initialized and configured, the ::sht3x_read function can be used to read measurement results from the sensor.

Example:
```
while (1) {
    int16_t temp;
    int16_t hum;

    if ((res = sht3x_read(&dev, &temp, &hum)) == SHT3X_OK) {
        printf("Temperature [°C]: %d.%d\n"
               "Relative Humidity [%%]: %d.%d\n"
               "+-------------------------------------+\n",
               temp / 100, temp % 100,
               hum / 100, hum % 100);
    }
    else {
        printf("Could not read data from sensor, error %d\n", res);
    }
    xtimer_usleep(1000000);
}

```
*/
