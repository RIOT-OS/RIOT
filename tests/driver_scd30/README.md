## About
This is a test application for the Sensirion SCD30 CO2 concentration,
temperature and relative humidity sensor

## Usage
This test application will initialize the SCD30 sensor and print TEST_ITERATIONS
number of following values first in continuous mode and then triggered mode every
second:

* CO2 concentration
* Temperature
* Relative humidity

The user can specify the number of iterations by setting the variable
`TEST_ITERATIONS` (default value is 10) from commandline as follows:
```
make BOARD=... TEST_ITERATIONS=... -C tests/driver_scd30
```
