# About
This is a test application for MPU Common Driver.

# Usage
The application will initialize the MPU motion sensor with the following parameters:
 - Accelerometer: ON
 - Gyroscope: ON
 - Magnetometer: ON
 - Sampling Rate: 200Hz
 - Compass Sampling Rate: 100Hz [MPU9150, not MPU6050]

After initialization, the application reads accel, gyro, [compass] and
temperature values every second and prints them to the STDOUT.
