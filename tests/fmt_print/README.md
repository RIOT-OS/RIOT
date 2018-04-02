Expected result
===============
This application Test the formatter.

Example:

```
RIOT native interrupts/signals initialized.
LED_RED_OFF
LED_GREEN_ON
RIOT native board initialized.
RIOT native hardware initialization complete.

main(): This is RIOT! (Version: 2018.04-devel-796-g49347-josua-VBox-fmt)
Testing 'print_str' if you can read this it works.

Testing 16 bit to char conversion:
1 = 1
65 535 = 65535

Testing 32 bit to char conversion:
1 000 000 = 65536
4 294 967 295 = 4294967295

Testing 64 bit to char conversion:
Error length 1 != 10
4 294 967 296 = 0
Error length 10 != 19
18 446 744 073 709 551 615 = 4294967295

More testing has to be implemented jet.
```