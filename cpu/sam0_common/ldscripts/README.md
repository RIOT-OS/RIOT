### Atmel SAM0 linker scripts notes

This folder contains SAM0 CPU specific linker scripts that are used to generate
the final binary firmware.

There are 2 kinds of scripts:

* &lt;name of cpu&gt;.ld: used to generate a firmware that starts at the
beginning of the flash memory. The firmware is copied to the flash memory
using [OpenOCD](https://github.com/ntfreak/openocd).

* &lt;name of cpu&gt;\_arduino\_bootloader.ld: used to generate a firmware
that starts after a preflashed Arduino bootloader. The firmware is copied to
the flash memory using [Bossa](https://github.com/shumatech/BOSSA).
This is the kind of configuration used with Arduino MKR and Adafruit Feather
M0 boards.
