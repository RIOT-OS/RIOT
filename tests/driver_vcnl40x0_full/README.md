# About

This is a manual test application for the Vishay VCNL4010 proximity and
ambient light sensor driver.

# Usage

The test application demonstrates the use of the VCNL40X0 driver. It uses a
VCNL4010, VCNL4020, or VCNL3020 sensor connected to I2C_DEV(0) with
following configuration parameters:

- Ambient light measurement with
     - a rate of 4 per second (#VCNL40X0_ALS_RATE_4) and
     - averaging over 32 conversions (#VCNL40X0_ALS_AVG_32)

- Proximity measurement with
     - a rate of 1.95 per seconds (#VCNL40X0_PRX_RATE_1_95) and
     - an IR LED current of 20 mA.

The application uses data ready interrupts to fetch the data. Furthermore,
it configures and enables threshold interrupts that are triggered when
ambient light exceed the thresholds.
