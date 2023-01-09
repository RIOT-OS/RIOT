/**
@defgroup    boards_mega-xplained Mega1284P-Xplained
@ingroup     boards
@brief       Support for the Mega1284P-Xplained board.

### General information

The [Mega1284P-Xplained](http://www.microchip.com/DevelopmentTools/ProductDetails.aspx?PartNO=atmega1284p-xpld)
is an evaluation kit by Atmel (now Microchip) for their ATmega1284P microcontroller.

### Flash the board

You can change the fuses and flash the board using an SPI ISP programmer such
as the Bus Pirate, or using a JTAG programmer such as the Atmel-ICE with a
JTAG adapter.

If using the Atmel-ICE as a JTAG programmer, connect the adapter with the
square pin on the adapter connecting with the square pin on the board.

1. First, make sure the default fuse settings are correct.<br/>

   WARNING: setting the fuses incorrectly can brick your board!

   If using the Buspirate:
```
     avrdude -p m1284p -c buspirate -P /dev/ttyUSB0 -U efuse:w:0xFF:m
     avrdude -p m1284p -c buspirate -P /dev/ttyUSB0 -U hfuse:w:0x99:m
     avrdude -p m1284p -c buspirate -P /dev/ttyUSB0 -U lfuse:w:0xE2:m
```

   If using the Atmel-ICE:
```
     avrdude -p m1284p -c atmelice -U efuse:w:0xFF:m
     avrdude -p m1284p -c atmelice -U hfuse:w:0x99:m
     avrdude -p m1284p -c atmelice -U lfuse:w:0xE2:m
```

2. After the fuses are set, flash the application.

   If using the Buspirate:
```
     BOARD=mega-xplained make -C examples/hello-world flash
```

   If using the Atmel-ICE:
```
     BOARD=mega-xplained PROGRAMMER=atmelice make -C examples/hello-world flash
```

### Accessing STDIO via UART

STDIO can be accessed through the USB connector. The on-board UART-USB
adapter is not affected by flashing. It shows up as /dev/ttyACM0 on Linux.
It will be used automatically with `make term`:
```
     BOARD=mega-xplained make -C examples/hello-world term
```

### Pin Change Interrupts

More pins can be used for hardware interrupts using the Pin Change
Interrupt feature. See @ref boards_common_atmega for details.

 */
