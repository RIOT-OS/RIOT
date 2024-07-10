# LEDs and basic GPIO example application

## Description

This basic example allows easy, interactive control of internal board LEDs,
and basic GPIO for externally connected simple devices (for e.g. additional
LEDs, relay, motors - via dedicated drivers, etc.).

In particular, this example shows:
- on/off and toggle internal board LEDs.
- initialize GPIO port in output mode.
- set GPIO port state to HIGH or LOW.

## Shell command

The following commands are available:

- `led0`, `led1` ... `led7`: allows switching on/off or toggle internal board
  LEDs. Number of commands depends on internal LEDs number.
- `init`: call before interact with GPIO port to initialize chosen port in
  output mode, which allows setting it to HIGH or LOW state.
- `set`: sets GPIO port state to HIGH.
- `clear`: sets GPIO port stet to LOW.
- `help`: default RIOT command, which shows available commands.

## Example on `native`

- Build and run `LEDs` example application on the `native` target,
as Linux application:

```
$ make term
[...]
RIOT native interrupts/signals initialized.
RIOT native board initialized.
RIOT native hardware initialization complete.

main(): This is RIOT! (Version: 2021.07-devel-10893-gb2e97-example-LEDs)
LEDs, version 1.0.0
>
```

Or run it directly without terminal. Go to `RIOT/examples/LEDs` and run
commands:

```
$ make
[...]
$ ./bin/native/LEDs.elf
RIOT native interrupts/signals initialized.
RIOT native board initialized.
RIOT native hardware initialization complete.

main(): This is RIOT! (Version: 2021.07-devel-10893-gb2e97-example-LEDs)
LEDs, version 1.0.0
>
```

- List the available commands:
```
> help
help
Command              Description
---------------------------------------
init                 GPIO pin initialization
set                  Set GPIO pin to HIGH
clear                Set GPIO pin to LOW
led0                 Switch on/off on-board LED0
led1                 Switch on/off on-board LED1
```

- Enable internal LED0 and LED1 (the `native` target has two virtual LEDs):

```
> led0 on
led0 on
LED_RED_ON

> led1 on
led1 on
LED_GREEN_ON
```
## Example on sample board - stm32f469i-disco

- Build and flash `LEDs` example application on sample board, for example
 `stm32f469i-disco` target, which has 4 internal LEDs:

```
$ make BOARD=stm32f469i-disco flash term
[...]
main(): This is RIOT! (Version: 2021.07-devel-10894-g2ad22b9-example-LEDs)
LEDs, version 1.0.0
> help
help
Command              Description
---------------------------------------
init                 GPIO pin initialization
set                  Set GPIO pin to HIGH
clear                Set GPIO pin to LOW
led0                 Switch on/off on-board LED0
led1                 Switch on/off on-board LED1
led2                 Switch on/off on-board LED2
led3                 Switch on/off on-board LED3
```

- Switch on/off internal LEDs using commands `led0`, `led1`, `led2` or `led3`.