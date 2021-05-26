# About
This is a manual test application for the HCSR-04 distance ultrasonic sensor.

# Usage
There are three parameters to configure:
- trigger_pin of the sensor
- echo_pin of the sensor
- temperature of the environment in milli-Celsius

Temperature setting is optional and is used to calculate more precisely the distance, taking 
into acount the variation of air speed.

The module exposes saul API from which measurements can been taken with minimum code.

Finally the module is capable of detecting error such as surpassing the maximum allowed read frequency,
or detecting that there may be a connection issue, due to the missing of a echo response from the sensor.

