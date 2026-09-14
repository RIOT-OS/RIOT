# About
This is a manual test application for the QST QMA6100P 3-axis accelerometer
driver.

# Usage
The application exercises the driver against the data-ready interrupt.

It runs 4 tests, printing PASS/FAIL to the STDOUT:
- init test: verifies the device initializes correctly,
- rate sweep: configures each ODR (12.5 Hz to 100 Hz) and checks the measured
  interrupt rate matches,
- ULPS test: verifies interrupts stop in low-power state and resume at the
  configured rate after wake,
- interrupt-driven streaming test: prints each acceleration sample as it
  arrives and checks that no sample was already pending, which wouldn't make
  sense for a data-ready interrupt.
