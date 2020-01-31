# Test Application for the Sensirion SPS30 Particulate Matter Sensor

## About
This is a simple test application for the SPS30 driver.

## Expected result
If everything is working as expected the following message should be printed at
the end `sps30 test: [SUCCESS]`.

## Details
The test application will execute every function supported by the sps30 driver
and check if it returns successfully.
Some steps in the test execution take a few seconds, but the overall test
should finish within one minute.
Shortly after the start you should hear the fan of the sensor spinning up for a
manual fan cleaning cycle. After that, proper operation of the auto-clean
update is verified and 10 measurements are printed to the console.
