# About
This is a test application for the TI bq27441-G1 lithium battery gauge
# Usage
Just enter the `make BOARD=??? flash` command in the `tests/driver_bq27441/` folder.
Make sure the `BQ27441_INT_PIN` is set in your boards periph_conf.h
# Results
The sensor will test the device id and the remaining capacity.
