/**

@defgroup  drivers_lsm303agr LSM303AGR 3D accelerometer/magnetometer
@ingroup   drivers_sensors
@ingroup   drivers_saul
@brief     Device driver for the LSM303AGR 3D accelerometer/magnetometer

This driver is written in Rust,
and based on the externally maintained [lsm303agr] crate.

This means that:

- it is only available on platforms supported by Rust,
- it needs Rust installed on the build machine as described in @ref using-rust, and
- it downloads additional Rust code from crates.io (in versions pinned by RIOT) when first used.

[lsm303agr]: https://crates.io/crates/lsm303agr

## Usage

When configured on a board, the devices are initialized at a fixed data acquisition rate and the chip's default range of +-2g.
Data values are obtained on demand
whenever queried through @ref drivers_saul.

For each device, two SAUL entries are registered labelled "LSM303AGR accelerometer" and "LSM303AGR magnetometer",
which produces 3-axis values in units of g and Tesla, respectively.
Accelerometer values are always scaled to milli-g (they come that way and don't exceed the i16 range of SAUL phydats);
magnetometer readings are dynamically downscaled from their original i32 Nanotesla readings to fit in a phydat.

The driver is configured for a board by placing an `lsm303agr-config.rs` file
in the board's include directory, which lists the I2C device(s) on which an accelerometer should be found:

```
const I2C_DEVICES: &[u8] = &[0];
```

## Limitations

- Advanced features of the sensor
  (adjusting acquisition rate or resolution, free-fall detection, interrupts etc.)
  are not exposed.

- The driver accepts some memory overhead (roughly, two mutexes) to avoid unsafe code and enhance readability.
  The unsafe code would be sound based on the assertion that the initialization code is only called once
  (and in particular is not reentrant).

*/
