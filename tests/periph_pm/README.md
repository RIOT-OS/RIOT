Expected result
===============
You should be presented with the RIOT shell, providing you with commands to test the power
management implementation by forcing the CPU to set a certain power mode. An external power
measurement device is needed to observe the effect.

Some power modes may require a certain event to wake up. Reset the CPU if needed.

If a RTC peripheral is available, an additional command to temporarily unblock a power mode is
available (provided that the CPU can wake-up from given power mode).

Background
==========
Test the functionality of the platform's power management implementation.
