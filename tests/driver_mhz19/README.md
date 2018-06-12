## About
This is a test application for the MH-Z19 CO2 sensor, which measured
the parts per million CO2 content in the air up to 2000ppm.

## Usage
The application initializes the device and goes into an infinite loop of
measurements. One measurement can take up to sligtly more than 2 seconds,
as the sensor outputs its measurements via pulse width modulation,
with pulses of 1004ms +-5%, so if a measurement is started just after
the start of a cycle, it takes the time of two cycles for an accurate
measurement (see the [datasheet][1] for more information).

After initialization, the application will take and display measurements
every two seconds.

## Overruling default parameters
The default pin for an MH-Z19 device is `GPIO_PIN(0, 0)`. If you want
to use another pin, build this application as follows:

    export CFLAGS='-DMHZ19_PARAM_PIN=\(GPIO_PIN\(1,2\)\)'
    make -C tests/driver_mhz19 BOARD=...


For more information see the datasheet:
[1]: http://www.winsen-sensor.com/d/files/PDF/Infrared%20Gas%20Sensor/NDIR%20CO2%20SENSOR/MH-Z19%20CO2%20Ver1.0.pdf
