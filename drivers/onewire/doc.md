@defgroup   drivers_onewire 1-Wire
@ingroup    drivers
@brief      1-Wire Buses and Devices


@defgroup   drivers_onewire_buses 1-Wire Bus Drivers
@ingroup    drivers_onewire
@brief      1-Wire Bus Interface and Drivers

This is RIOT's driver interface for Dallas Semiconductor Corp (now Maxim
Integrated) specified 1-Wire Buses. 1-Wire slave device drivers should use it
to access the buses (and slave hardware). Drivers implementing 1-Wire bus
master functionality should expose this functionality via this interface so
that slave drivers can operate independent of the bus master hardware and its
driver.

The pseudomodule `onewire_oneslave`, when enabled, permits the assumption
that each 1-wire bus instance will only ever have a single slave device
connected. This turns calls to acquire and release the bus into noops.

The pseudomodule `onewire_multidriver` enables support for multiple types of
bus masters. Without this enabled, multiple busses are supported, but all
instances must use the same driver, and therefore be of the same type of
hardware.

@defgroup   drivers_onewire_devs 1-Wire Device Drivers
@ingroup    drivers_onewire
@brief      1-Wire Slave Device Drivers


