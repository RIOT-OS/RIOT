# About
This is a manual test application for the INA226 current and power monitor driver.

# Usage
This test application initializes the sensor with continous measuring mode for bus voltage and shunt voltage.
Both adc conversion times are set to 1.1 ms which are then averaged over 256 Samples by the INA226 averaging mechanism.
This configuration leads to updated values ~two times per second (every 563.2 ms). 
To print new measurements this example takes advantage of the INA226 alert pin to fire an interrupt 
whenever a new measuremt value is available (see test_ina226_callback definition for reference).
To change this default configuration make use of the defines under "GENERAL CONFIGURATION PARAMETERS" in main.c
The value for the calibration register is set acording to the datasheet including an example on how to 
calculate a corrected calibration register value to compensate hardware related variations eg. shunt resistor 
tolerance (see "REFERENCE MEASUREMENT CALIBRATION PARAMETERS" in main.c).