@defgroup pkg_decadriver Decadriver for Qorvo DW3000
@ingroup  pkg
@brief    RIOT adaptation of br101's adaptation of Qorvo's decadriver

This package implements the adaptation layer of the
[decadriver adaptation by br101](https://github.com/br101/dw3000-decadriver-source).
This adaptation allows only *one DW3000* chip per system, with the benefit of
using less resources.

@see https://forum.qorvo.com/uploads/short-url/xD3TlXKvkujjdUaJWXv2b4E7GQN.pdf
     for documentation of what the driver implements

Similar to drivers, a `dw3000_params.h` includes the pin connection definition
and can be overloaded just like any other driver, however only a single params
definition can be added, as multiple DW3000 are not supported.

@note The driver seems to also be compatible with DW3720, however no hardware
      was available to verify this and specification and datasheets of the
      DW3720 are also quite difficult to find.

The DW3000 device is assumed by default using the package `decadriver`, but the
DW3720 driver could be used instead by additionally specifying the
`PSEUDOMODULE += decadriver_dw3720`.
