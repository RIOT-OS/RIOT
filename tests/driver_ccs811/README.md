# About
This is a manual test application for the CCS811 driver. It shows how the
sensor can be used for periodic polling as well as with interrupts.

# Usage

The test application demonstrates the use of the CCS811. It uses the default
configuration parameters, that is, the measurement mode
```CCS811_MODE_1S``` with one measurement per second.

The application can use both approaches to wait for new data:

1. using the data-ready interrupt (```CCS811_INT_DATA_READY```):
    ```
    #define USE_CSS811_DATA_READY_INT  (1)
    ```

2. using the data-ready status function (```ccs811_data_ready```)
    ```
    #define USE_CSS811_DATA_READY_INT  (0)
    ```

If data-ready interrupts are used, the default configuration parameter for the
interrupt pin can be overriden by ```CCS811_PARAM_INT_PIN``` before
```ccs811_params.h``` is included.
```
#define CCS811_PARAM_INT_PIN     (GPIO_PIN(0, 12))
```
