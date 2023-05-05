# Arduino peripheral GPIO Test

This test is designed for testing the Arduino to RIOT pin mapping
on boards with Arduino support.  It is much like tests/periph_gpio
but for Arduino pin numbers.

In this test, pins are specified by integer pin numbers.
It uses a single integer to indicate the arduino digital pin.

At this point the Analog pins are not supported.

## Commands

`init_out <pin>`

   init as output (push-pull mode)

`init_in <pin>`

   init as input w/o pull resistor

`init_in_pu <pin>`

   init as input with pull-up

`read <pin>`

   read pin status

`set <pin>`

   set pin to HIGH

`clear <pin>`

   set pin to LOW

`toggle <pin>`

   toggle pin

## Example

Blink the Arduino UNO led once.

```
2018-08-06 09:00:32,673 - INFO # main(): This is RIOT! (Version: 2018.10-devel-338-g2c8a2-z400-periph_gpio_arduino)
2018-08-06 09:00:32,722 - INFO # Arduino GPIO peripheral driver test. ? for help
> ?
2018-08-06 09:00:38,347 - INFO #  help
2018-08-06 09:00:38,383 - INFO # Command              Description
2018-08-06 09:00:38,424 - INFO # ---------------------------------------
2018-08-06 09:00:38,478 - INFO # init_out             init as output (push-pull mode)
2018-08-06 09:00:38,535 - INFO # init_in              init as input w/o pull resistor
2018-08-06 09:00:38,584 - INFO # init_in_pu           init as input with pull-up
2018-08-06 09:00:38,621 - INFO # read                 read pin status
2018-08-06 09:00:38,662 - INFO # set                  set pin to HIGH
2018-08-06 09:00:38,699 - INFO # clear                set pin to LOW
2018-08-06 09:00:38,732 - INFO # toggle               toggle pin
> init_out 13
2018-08-06 09:00:45,363 - INFO #  init_out 13
> toggle 13
2018-08-06 09:00:50,897 - INFO #  toggle 13
> toggle 13
2018-08-06 09:00:53,568 - INFO #  toggle 13
> /exit
```
