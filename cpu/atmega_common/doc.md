@defgroup   cpu_atmega_common Atmel ATmega CPU: common files
@brief      AVR Atmega specific code
@ingroup    cpu

### Pin Change Interrupts

Pin Change Interrupts (PCINTs) can be enabled using pseudo modules. To provide
a low-memory overhead implementation, the PCINTs are grouped into **banks**.
Each banks corresponds to one PCINT on the ATmega (PCINT0, ..., PCINT3).

To enable only a specific bank, simply add `USEMODULE += atmega_pcintN` to your
Makefile. To enable all interrupts you can use `USEMODULE += atmega_pcint`.

In case you want to add a new CPU, simply provide an `atmega_pcint.h` with your
CPU and adapt your Makefile.dep and Makefile.features files.
