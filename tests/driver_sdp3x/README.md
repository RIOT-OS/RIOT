## About
This is a test application for the SDP3x temperature and differential pressure sensor.

## Usage
This test application will initialize the SDP3x sensor and print TEST_ITERATIONS number
of following values first in continuous mode and then triggered mode every second:

* Temperature
* Differential pressure

The user can specify the number of iterations by setting the variable `TEST_ITERATIONS`
(default value is 10) from commandline as follows:
```
make BOARD=... TEST_ITERATIONS=... -C tests/driver_sdp3x
```

##Features
The sensor has an IRQn pin which indicates data ready on the sensor. This pin can be
connected on the GPIO ports and configured in the variable `SDP3X_PARAM_IRQ_PIN`. This
helps getting measurements as soon as they are ready instead of the 46ms wait (which is
the maximum time the sensor might take the get the measurement ready).

When not using irq pin, instead of `USEMODULE = sdp3x_irq`, use `USEMODULE = sdp3x`.
