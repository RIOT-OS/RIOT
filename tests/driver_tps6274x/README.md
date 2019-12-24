## About
Tests the tps6274x step-down converter

## Usage
Build the test by using the `make BOARD=??? flash` command in the `test/driver_tps6274x/` folder.

## Results
The stepdown converter will step from 1.8V to 3.3V in 100mV steps.
Each voltage will be set for 3 seconds and can be measured at the VOUT Pin.
After 1 second also the LOAD output pin will be enabled for 2 seconds
until the next voltage is set.
