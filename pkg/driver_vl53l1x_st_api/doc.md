@defgroup pkg_driver_vl53l1x_st_api VL53L1X Driver API package
@ingroup  pkg
@brief    Driver API package for the ST VL53L1X Time-of-Flight (ToF) ranging sensor
@see      drivers_vl53l1x
@see      [VL53L1X API user manual](https://www.st.com/resource/en/user_manual/dm00474730.pdf)

This package provides ST's STSW-IMG007 VL53L1X API as part of the
Pololu AVL53L1X library for Arduino. It is required for the
VL53L1X Time-of-Flight (ToF) ranging sensor driver, see @ref drivers_vl53l1x.

ST's STSW-IMG007 VL53L1X API gives the application access to the complete
functionality of ST VL53L1X Time-of-Flight (ToF) ranging sensors in
addition to the functionality realized by the \ref drivers_vl53l1x driver
module.

For details on how to use this API, please refer
[the users manual](https://www.st.com/resource/en/user_manual/dm00474730.pdf).

ST's STSW-IMG007 VL53L1X API is very complex and therefore large.

@note To use the ST VL53L1X Time-of-Flight (ToF) ranging sensor,
the \ref vl53l1x_st_api driver module has to be used,
see @ref drivers_vl53l1x.
