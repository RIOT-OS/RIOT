# LEDs and basic GPIO example application

## Description

The application `leds_shell` is a basic example, which allows easy, interactive
control of internal board LEDs, and basic GPIO for externally connected simple
devices (for e.g. additional LEDs, relay, motors - via dedicated drivers, etc.)
via the shell.

In particular, this example shows:
- on/off and toggle internal board LEDs.
- initialize GPIO port in output mode.
- set GPIO port state to HIGH or LOW.

## Shell command

The following commands are available:

- `led`: allows switching on/off or toggle internal board LEDs.
- `gpio`: allows initialization of GPIO port and set state to HIGH/LOW.
- `help`: default RIOT command, which shows available commands.

## Usage on `BOARD=native`

- Build and run `leds_shell` example application on the `native` target,
as Linux application:

```
$ make all term
[...]
RIOT native interrupts/signals initialized.
RIOT native board initialized.
RIOT native hardware initialization complete.

main(): This is RIOT! (Version: 2021.07-devel-10893-gb2e97-example-leds_shell)
This board has 2 LEDs
>
```

- List the available commands:
```
> help
help
Command              Description
---------------------------------------
gpio                 GPIO pin initialization and set port state HIGH/LOW
led                  Switch on/off or toggle on-board LEDs
```

- Enable internal LED0 and LED1 (the `native` target has two virtual LEDs):

```
> led 0 on
led 0 on
LED_RED_ON

> led 1 on
led 1 on
LED_GREEN_ON
```
## Usage on actual hardware - `BOARD=stm32f469i-disco`

- Build and flash `leds_shell` example application on sample board, for example
 `stm32f469i-disco` target, which has 4 internal LEDs:

```
$ make all BOARD=stm32f469i-disco flash term
[...]
main(): This is RIOT! (Version: 2021.07-devel-10894-g2ad22b9-example-leds_shell)
This board has 4 LEDs
> help
help
Command              Description
---------------------------------------
gpio                 GPIO pin initialization and set port state HIGH/LOW
led                  Switch on/off or toggle on-board LEDs
```

- Switch on/off internal LEDs using `led` command and appropriate LED id.
- Initialize GPIO port using `gpio init`.
- Change state of GPIO port using `gpio set` and `gpio clear`.
