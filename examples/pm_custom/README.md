Custom power management
=======================

RIOT's scheduler will, should no task be running, schedule the idle thread.
The idle thread is running, it calls "pm_set_lowest()" in a loop.

For most platforms, a module called "pm_layered" is responsible for selecting
the lowest possible low power mode depending on which peripherals are in use.
Some platforms do not use "pm_layered" but implement their own power management
scheme.

It might be desirable for some applications to override these mechanics.
One option to do so is the "pm_custom" module.

If selected, pm_layered or any per-platform "pm_set_lowest()" will be disabled,
and the application has to provide this function. It will be called as usual by
the idle thread.

This example application implements an example "pm_set_lowest()" that will just
print "sleeping" every (2**24 - 1)th time it is called and otherwise do no
power management at all.
