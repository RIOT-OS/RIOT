# About
This is a manual test application for the ST LSM6DSL sensor driver.

# Usage

After successful initialization, the sensor reads the accelerometer, gyroscope,
and temperature values every 500ms and prints them to the STDOUT. If device
initialization fails the tests exits with 1, otherwise it runs forever.

Note: reading sensor values may fail even on successful initialization.
