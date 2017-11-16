# About
This is a manual test application for the INA226 current and power monitor driver.

# Usage
This test application initializes the sensor with continuous measuring mode for bus voltage and shunt voltage.
Both ADC conversion times are set to 1.1 ms which are then averaged over 256 Samples by the INA226 averaging mechanism.
This configuration leads to updated values ~two times per second (every 563.2 ms). 
To print new measurements this example takes advantage of the INA226 alert pin to fire an interrupt 
whenever a new measurement value is available (see test_ina226_callback definition for reference).
To change this default configuration make use of the defines under "GENERAL CONFIGURATION PARAMETERS" in main.c
The value for the calibration register is set according to the data sheet including an example on how to 
calculate a corrected calibration register value to compensate hardware related variations eg. shunt resistor 
tolerance (see "REFERENCE MEASUREMENT CALIBRATION PARAMETERS" in main.c).

To run the test with your specific hardware setup and get correct measurements following steps are needed:

1) in main.c under 'GENERAL CONFIGURATION PARAMETERS' section adjust the following defines according to your platform:
    TEST_INA226_I2C: I2C bus that is connected to the ina226 device (eg. I2C_0)
    TEST_INA226_ADDR: I2C address of the ina226 that is set by external pin configuration of A0 and A1 pin
    TEST_INA226_ALERT_PIN: GPIO pin that is connected to the ina226 alert pin (eg. GPIO_PIN(0,6))
    TEST_INA226_SHUNT_RESISTOR_MILLI_OHM: value of the used shunt resistor in milliohm
2) connect a static load to the ina226 circuit, build and run the test application
3) read the reported measurements of the test application from the console
4) perform a reference measurement with an external ampere meter 
5) if values of 3) and 4) don't differ to much for your application you are done. Otherwise continue with 6)
6) set 'TEST_INA226_CAL_REPORTED_CURRENT' in main.c to the value from step 3
7) set 'TEST_INA226_CAL_AMM_MEAS_CURRENT' in main.c to the value from step 4
8) uncomment the line '#define TEST_INA226_REF_MEASUREMENTS_DONE' in main.c to use the corrected calibration value
