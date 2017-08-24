examples/blinker
================

This is a basic example that shows how to get an on-board blinking LED
using RIOT.
Some would consider this as a "Hello World" application for an embedded system.

This examples shows several important things provided by RIOT:
* How to load extra modules in an application using the Makefile variable `USEMODULE`.
  Since this application requires delays between each LED state change, we add
  ```
      USEMODULE += xtimer
  ```
  to specify the build system to use `xtimer`.

* A basic use of the `xtimer` API that provides high level timing features to
  RIOT. See [here](http://doc.riot-os.org/group__sys__xtimer.html) for more
  information.

* How to use the pre-defined on-board LED macro `LED0_TOGGLE`. This is defined
  for each supported board that provides an on-board LED.
