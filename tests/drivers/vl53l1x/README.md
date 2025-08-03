# VL53L1x Driver Test

## About

The test application demonstrates the usage of different functions
dependent on the used driver variant:

| Driver Module    | Description                                       |
|------------------|---------------------------------------------------|
| `vl53l1x`        | Standard driver with most functionality           |
| `vl53l1x_basic`  | Basic driver with only very basic functionality   |
| `vl53l1x_st_api` | ST VL53L1X API driver with complete functionality |

## Usage

The driver variant used is defined by my variable `DRIVER`, which is set to
`vl53l1x` by default. In this case, it is not necessary to specify the `DRIVER`
variable in the make command:
```
    make flash -C tests/driver_vl53l1x BOARD=...
```
To use other driver variants, module `vl53l1x_st_api` or module `vl53l1x_basic`
have to be specified at make command line
```
    DRIVER=vl53l1x_st_api make flash -C tests/driver_vl53l1x BOARD=...
```
or
```
    DRIVER=vl53l1x_basic make flash -C tests/driver_vl53l1x BOARD=...
```
If the configuration parameter `VL53L1X_PARAM_PIN_INT` is defined, interrupts
are used to get data instead of polling for new data. In the case of driver
variant `vl53l1x_st_api`, threshold interrupts are configured. Otherwise
only data interrupts are used.

In all cases, the sensor is configured with following default parameters:

- timing budget of 50 ms
- intermeasurement period of 100 ms