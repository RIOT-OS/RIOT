driver_sgp30
============

This is a manual test application for the SGP30 driver. It shows how the
sensor can be used for periodic polling.

Usage
=====

The test application demonstrates the use of the SGP30. By default the
`sgp30_strict` module is used, which means that periodic readings are
issued every 1s, and a warm-up period of 15s is enforced, before this
`sgp30_read_measurements` returns with `-EAGAIN`.

`sgp30_strict` can be disabled by compiling with `DISABLE_MODULE=sgp30_strict`.
With this when `sgp30_read_measurements` is called during the first 15s
constant values of 400ppm CO2eq and 0ppb TVOC.
