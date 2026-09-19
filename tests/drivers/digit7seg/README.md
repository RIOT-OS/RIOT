## About

This is a test application designed for the [5461AS](http://www.topliteusa.com/uploadfile/2014/0825/A-5461AS.pdf),
but can be applied to every less than 5 digits with 7 segments display without IC.

## Connecting the device

The display needs to be connected as follows:

- Connect all the pins `X` to the board `DIGIT7SEG_PARAM_X_PIN`
- Connect `DP` to the board `DIGIT7SEG_PARAM_DP_PIN`
- Connect all the pins `DIGX` to the board `DIGIT7SEG_PARAM_DIGX_PIN`

For each digit pin (`DIGX`), you need to use a transistor and link it to `GND`.

@note T represent a NPN transistor
```
       .----.----.----.----.
       |    |    |    |    |-- a
       |    |    |    |    |-- b
       |    |    |    |    |-- c
       |    |    |    |    |-- d
       |    |    |    |    |-- e
       |    |    |    |    |-- f
       |    |    |    |    |-- g
       |    |    |    |    |-- dp
       '----'----'----'----'
         |    |    |    |
         T-------------------- dig1
         |    |    |    |
         |    T--------------- dig2
         |    |    |    |
         |    |    T---------- dig3
         |    |    |    |
         |    |    |    T----- dig4
         |    |    |    |
         └──--└──--└──--└──--- GND
```
