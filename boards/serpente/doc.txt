/**
@defgroup    boards_serpente Serpente R2
@ingroup     boards
@brief       Support for the Serpente board.

### General information

The [Serpente boards](https://serpente.solder.party/) are low-cost development boards designed
to be used with Adafruit’s CircuitPython.

Three versions of the board board exist, but they all the same except for the USB connector.
The standard Serpente board contains a female USB Type-C connector, the Serpente Plug C
contains a male USB Type-C connector, and the Serpente Plug uses the board itself as a Type-A USB plug.

If you are familiar with the Digispark boards, you may notice some similarities.
This fact is of course not incidental, the Serpente boards are inspired by the Digispark,
both in form-factor as well as use-cases. The Serpente boards are meant to be used as quick
and dirty, yet flexible, prototyping tools.

### Pinout

\image html https://serpente.solder.party/r2/pinout.png width=66%

### Flash the board

Use `BOARD=serpente` with the `make` command.<br/>
Example with `micropython` application:
```
     make BOARD=serpente -C examples/micropython flash
```

RIOT will automatically trigger a reset to the bootloader, but this only works if RIOT is still
running on the board.
If your application has crashed or got erased, `make flash` will not be able to trigger a bootloader reset.

To manually enter the bootloader, quickly press the Reset button twice.
You know you've successfully entered the bootloader when the LED begins flashing blue.

Sometimes flashing fails and the board gets stuck in the bootloader.
In this case, just run `make flash` again when the device is not busy anymore.

### Accessing STDIO via UART

STDIO of RIOT is directly available over the USB port.
 */
