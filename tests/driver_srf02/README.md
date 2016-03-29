# About
This is a manual test application for the SRF02 ultrasonic ranger driver.

# Usage
First you always need to initialize the sensor using the `init` shell command
and the devices address in right-aligned, decimal format (as specified in the
reference manual), e.g.: `init 224`.

If successful (the shell will tell you...), you can sample distance data from
the device, either once (`shoot`) or continuously every second (`sample`).

This test application also allows to re-program the I2C address of SRF02
devices. For this initialize the device with its current address and then change
to the new address using the `addr` shell command (e.g. `addr 228`). After
calling the `addr` command, this application will automatically re-initialize
the device with the new address, so it is usable right away. Refer to the
datasheet for more information on usable addresses.

The following sequence shows how to re-program a device. The initial address is
`224`, the new address after the sequence is `228`:

```
2016-03-21 15:53:34,049 - INFO # > init 224
2016-03-21 15:53:34,054 - INFO # Initializing SRF02 sensor at I2C_DEV(0), address is 224
2016-03-21 15:53:34,079 - INFO # ... [Ok]
2016-03-21 15:53:34,079 - INFO #
shoot
2016-03-21 15:53:37,339 - INFO # > shoot
2016-03-21 15:53:37,411 - INFO # distance =  94 cm
addr 228
2016-03-21 15:53:41,468 - INFO # > addr 228
2016-03-21 15:53:41,472 - INFO # Set address to 228
shoot
2016-03-21 15:53:42,454 - INFO # > shoot
2016-03-21 15:53:42,527 - INFO # distance =  94 cm
init 224
2016-03-21 15:53:48,933 - INFO # > init 224
2016-03-21 15:53:48,938 - INFO # Initializing SRF02 sensor at I2C_DEV(0), address is 224
2016-03-21 15:53:48,962 - INFO # ... [Failed]
shoot
2016-03-21 15:53:50,568 - INFO # > shoot
2016-03-21 15:53:50,641 - INFO # distance = 29703 cm
init 228
2016-03-21 15:53:53,084 - INFO # > init 228
2016-03-21 15:53:53,089 - INFO # Initializing SRF02 sensor at I2C_DEV(0), address is 228
2016-03-21 15:53:53,113 - INFO # ... [Ok]
2016-03-21 15:53:53,113 - INFO #
shoot
2016-03-21 15:53:54,201 - INFO # > shoot
2016-03-21 15:53:54,274 - INFO # distance =  94 cm
```
