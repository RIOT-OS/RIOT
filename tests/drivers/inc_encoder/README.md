# About
This is a manual test application for a generic incremental rotary encoder.

# Usage
Connect a sensor - like a magnetic rotary encoder found on some dc motors - to your mcu.
The test will periodically print the measured RPM and number of revolutions
since the last readout.
See [drivers/inc_encoder/include/inc_encoder.h][inc_encoder]
for details on how to connect your sensor.

[inc_encoder]: ../../drivers/inc_encoder/include/inc_encoder.h