/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
@defgroup    drivers_sht4x   SHT4x Humidity and Temperature Sensor Series
@ingroup     drivers_sensors
@ingroup     drivers_saul
@brief       Device Driver for Sensirion SHT40/SHT41/SHT45 Humidity and
             Temperature Sensors

# Driver for SHT4x Digital Temperature and Humidity Sensors

The driver is for use with [RIOT-OS](https://github.com/RIOT-OS/RIOT). It
supports the Sensirion SHT40, SHT41, and SHT45 digital temperature and humidity
sensors.

## About the sensors

SHT4x are digital temperature and humidity sensors that use an I2C interface.
They only support a **single shot data acquisition mode**: each measurement
command triggers exactly one measurement, after which the sensor returns to its
low-power idle state.

They can operate with **three levels of repeatability** (#sht4x_repeat_t):

- low,
- medium, and
- high.

SHT4x sensors also feature an integrated heater that can be used to drive off
condensation or check sensor functionality. The heater can be operated with
**three power levels** (#sht4x_heater_power_t) and **two durations**
(#sht4x_heater_duration_t).

## Measurement process

Once a SHT4x sensor is initialized using ::sht4x_init, it can be used for
measurements.

The sensor always measures both temperature and humidity in a single I2C
transaction. Depending on the configured repeatability, the measurement takes
up to 8.3 ms. The process is separated into steps to avoid blocking the system
longer than necessary:

1. Send a measurement command to the sensor to start one measurement.
2. Wait for the sensor to complete the measurement.
3. Fetch and validate the results.

For convenience, the high-level function ::sht4x_read performs all three steps.
It rounds the wait time up to whole milliseconds, so it blocks the calling task
for up to 9 ms.

## Measurement results

Once a measurement is triggered, ::sht4x_read:

- fetches raw sensor data,
- checks the CRC checksums,
- converts the raw data to measured values, and
- returns the measured data.

The temperature is returned in hundredths of a degree Celsius, and relative
humidity in whole percent. Either the \p temp or \p hum parameter may be `NULL`
if only one value is needed.

## Error handling

All driver functions return 0 on success or a negative `errno` value on error.
I2C errors are reported as `-EIO`, and checksum errors as `-EBADMSG`.

## Repeatability

The SHT4x sensor supports three levels of repeatability (low, medium, and
high). Higher repeatability reduces measurement noise but increases
measurement duration.

Repeatability | Maximum Measurement Duration
:-------------|:----------------------------:
high          | 8.3 ms
medium        | 4.5 ms
low           | 1.6 ms

Possible repeatability levels are defined by #sht4x_repeat_t.

## On-chip heater

SHT4x sensors have an integrated heater that can be enabled for one measurement
using ::sht4x_read_with_heater. Configure the heater power and duration with
::sht4x_set_heater_power and ::sht4x_set_heater_duration before reading.

@note Using the heater is intended for diagnostic purposes (e.g.
de-condensation). Temperature and humidity readings taken while the heater is
active are not representative of ambient conditions.

## Usage

Before using the SHT4x driver, define the configuration and call ::sht4x_init
to initialize the sensor device of type #sht4x_dev_t.

The configuration structure #sht4x_params_t consists of the following
parameters:

Parameter        | Symbol               | Default
:----------------|:---------------------|:------------------
I2C device       | SHT4X_PARAM_I2C_DEV  | I2C_DEV(0)
I2C address      | SHT4X_PARAM_I2C_ADDR | SHT4X_I2C_ADDR_1
Repeatability    | SHT4X_PARAM_REPEAT   | SHT4X_HIGH

The default configuration is defined in `sht4x_params.h`. Applications can
override it before including that header.

```c
#define SHT4X_PARAM_REPEAT (SHT4X_MEDIUM)

#include "sht4x.h"
#include "sht4x_params.h"
```

Call ::sht4x_init for each SHT4x sensor to initialize it and check its
availability:

```c
sht4x_dev_t dev;

if ((res = sht4x_init(&dev, &sht4x_params[0])) != 0) {
    /* error handling */
}
```

The `sht4x_params` array is also defined in `sht4x_params.h` and can be
overridden by the application. For example, the following configuration sets
up two devices:

```c
#include "sht4x.h"
#include "sht4x_params.h"

#define SHT4X_PARAMS    { .i2c_dev  = I2C_DEV(0),          \
                          .i2c_addr = SHT4X_I2C_ADDR_1,   \
                          .repeat   = SHT4X_HIGH          \
                        }, \
                        { .i2c_dev  = I2C_DEV(0),          \
                          .i2c_addr = SHT4X_I2C_ADDR_2,   \
                          .repeat   = SHT4X_LOW           \
                        }

sht4x_dev_t dev1;
sht4x_dev_t dev2;

if ((res = sht4x_init(&dev1, &sht4x_params[0])) != 0) {
    /* error handling */
}

if ((res = sht4x_init(&dev2, &sht4x_params[1])) != 0) {
    /* error handling */
}
```

Once initialized, use ::sht4x_read to retrieve measurement results:

```c
while (1) {
    int16_t temp;
    int16_t hum;

    if ((res = sht4x_read(&dev, &temp, &hum)) == 0) {
        printf("Temperature [°C]: %d.%02d\n"
               "Relative Humidity [%%]: %d\n"
               "+-------------------------------------+\n",
               temp / 100, abs(temp % 100),
               hum);
    }
    else {
        printf("Could not read data from sensor, error %d\n", res);
    }
    ztimer_sleep(ZTIMER_MSEC, 1000);
}
```
*/
