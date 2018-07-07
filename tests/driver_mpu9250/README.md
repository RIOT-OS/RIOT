# About
This is a test application for the MPU-9250 Nine-Axis Driver.

# Usage
The application will initialize the MPU-9250 motion sensor with the following parameters:
 - Accelerometer: ON
 - Gyroscope: ON
 - Magnetometer: ON
 - Sampling Rate: 200Hz
 - Compass Sampling Rate: 100Hz

After initialization, the application reads accel, gyro, compass and temperature values every second and prints them to the STDOUT.
The test does not cover interrupt generation mode, as there is no board agnostic way of defining an interrupt pin.
