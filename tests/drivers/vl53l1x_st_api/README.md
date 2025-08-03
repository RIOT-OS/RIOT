# ST VL53L1X API Driver Test

## About

The test application provides a direct test for the ST VL53L1X API
driver module `vl53l1x_st_api`, which supports the full functionality
of the sensor. Although the ST VL53L1X API driver could also be tested
with `tests/drivers/vl53l1x`, this test application allows the use of the
ST VL53L1X API driver without additional definitions and is thus covered
by CI compilation.

## Usage

Compile the test application using command:
```
    make flash -C tests/driver_vl53l1x_st_api BOARD=...
```

If the configuration parameter VL53L1X_PARAM_PIN_INT is defined, interrupts
are used to get data instead of polling for new data. Additionally,
threshold interrupts are configured in this case.

In all cases, the sensor is configured with following default parameters:

- timing budget of 50 ms
- intermeasurement period of 100 ms
