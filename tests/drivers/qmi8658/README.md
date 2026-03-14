# About
This is a manual test application for the QMI8658 IMU driver.

# Usage

After successful initialization, the current configuration is dumped to STDOUT and the 
operating mode is set to enable both accelerometer and gyroscope. The sensor reads 
the accelerometer, gyroscope, and temperature values every 500ms and prints them to 
the STDOUT. If device initialization fails the tests exits with 1, otherwise it runs forever.
