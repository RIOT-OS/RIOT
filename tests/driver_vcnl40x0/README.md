# About

This is a manual test application for the Vishay VCNL4010 proximity and
ambient light sensor driver.

# Usage

The test application demonstrates the use of the VCNL40X0 driver. It uses a
VCNL4010, VCNL4020, or VCNL3020 sensor connected to I2C_DEV(0) with
following configuration parameters:

- Ambient light measurement with
     - a rate of 10 per second (#VCNL40X0_ALS_RATE_10) and
     - averaging over 32 conversions (#VCNL40X0_ALS_AVG_32)

- Proximity measurement with
     - a rate of 31.25 per seconds (#VCNL40X0_PRX_RATE_31_25) and
     - an IR LED current of 20 mA.

The application fetches data periodically every 100 ms.
