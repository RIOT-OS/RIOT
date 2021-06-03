## About
This is a test application for the Bosch BNO055 9-axis sensor.
The BNO055 includes a MCU that performs sensor fusion.
It uses the three sensors (Accelerometer, Gyroscope, and Magnetometer)
to calculate the position of the sensor.

The sensor fusion outputs the following formats:

 - Quaternions rotation values
 - Euler rotation angles
 - Gravitation vector that contains the acceleration due to the gravitational field.
 - Linear Acceleration vector that contains the acceleration due to change of velocity.

And it is possible to directly read the raw measurement data from the sensors.

## Usage
The application will initialize the device and read the following outputs every second:
 - Quaternions: unitless
 - Euler rotation: rad
 - Gravitation Vector: m/s^2
 - Linear Acceleration: m/s^2
 - Magnetometer: μT
 - Accelerometer: m/s^2
 - Gyroscope: rad/s
