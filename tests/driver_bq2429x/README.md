# BQ2429x driver test application

This is the test application for the BQ2429x device driver, it provides various
commands to set and get the battery charge parameters and to check the device
status. Type `help` on the serial console to see available commands.

It can be flashed normally with:

```
make flash -C tests/driver/bq2429x BOARD=...
```
