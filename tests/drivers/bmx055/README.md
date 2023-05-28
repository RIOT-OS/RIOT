# About
This is a test application that is polling all three SAUL devices
(Magnetometer, Accelerometer, Gyroscope) of the Bosch BMX055
9-Axis Sensor.
This test is a copy of the standard saul test. Since it utilizes all
capabilities of the driver it is a sufficient test of the sensor.

# Usage
The application will initialize the modules with default parameters and read
out accel, gyro and compass values each second
default parameters are:
- Magnetometer Sampling Rate: 10 Hz
- Accelerometer Range: 2G
- Gyroscope Scale: 2000 DPS
