/**
@defgroup    boards_nz32-sc151 NZ32-SC151
@ingroup     boards
@brief       Support for the Modtronix nz32-sc151 board.

This board provider is [modtronix](http://modtronix.com). They don't supply data
sheet's but there wiki can be found
[HERE](http://wiki.modtronix.com/doku.php?id=products:nz-stm32:nz32-sc151). The
schematic for there board can be found
[HERE](http://modtronix.com/prod/nz32/nz32-sc151/nz32-sc151-r1.pdf).

## Hardware

![st-
nucleo-l1](http://wiki.modtronix.com/lib/exe/fetch.php?media=products:nz-
stm32:nz32-sc151-r1_pinout_diagram_782.jpg)


### MCU
| MCU        | STM32L151RC |
|:------------- |:--------------------- |
| Family | ARM Cortex-M3     |
| Vendor | ST Microelectronics   |
| RAM        | 32Kb  |
| Flash      | 256Kb             |
| Frequency  | 32MHz|
| FPU        | no                |
| Timers | 11 (8x 16-bit,2x 16-bit basic timers, 2x watchdog timers, 1x 32-bit [TIM5])   |
| ADCs       | 1x 25-channel 12-bit      |
| DACs       | 1x 2-channel 12-bit       |
| USB 2.0    | 1                 |
| UARTs      | 3                 |
| SPIs       | 3                 |
| I2Cs       | 2                 |
| Vcc        | 1.65V - 3.6V          |
| Datasheet  | [Datasheet](http://www.st.com/content/ccc/resource/technical/document/datasheet/2a/6e/97/91/cd/c0/43/8b/DM00048356.pdf/files/DM00048356.pdf/jcr:content/translations/en.DM00048356.pdf) |
| Reference Manual | [Reference Manual](http://www.st.com/content/ccc/resource/technical/document/reference_manual/cc/f9/93/b2/f0/82/42/57/CD00240193.pdf/files/CD00240193.pdf/jcr:content/translations/en.CD00240193.pdf) |
| Programming Manual | [Programming Manual](http://www.st.com/content/ccc/resource/technical/document/programming_manual/5b/ca/8d/83/56/7f/40/08/CD00228163.pdf/files/CD00228163.pdf/jcr:content/translations/en.CD00228163.pdf) |
| Board WIKI | [WIKI nz32-sc151](https://wiki.modtronix.com/doku.php?id=products:nz-stm32:nz32-sc151#schematics)|

### User Interface

2 Button:

| NAME   | BOOT  | RESET     |
|:----- |:----- |:--------- |
| Pin    | BOOT0 | NRST  |

1 LED:

| NAME   | LED   |
| -----  | ----- |
| Color  | red   |
| Pin    | PB2   |

## Using UART

This board doesn't open an UART or serial interface threw the USB
automatically, and the USB device driver hasn't been implemented to RIOT yet.
Therefore, to open an UART interface one hast to connect a usb/ttl converter to
the UART3 pins PB11 (RX3) and PB10 (TX3).

## Flashing the device

The standard method for flashing RIOT to the nz32-sc151 is using dfu. On
linux you must get dfu-util package v.8.0 or newer. You can try:

```
sudo apt-get install dfu-util
```

but most repos install older versions, therefore you should clone from [dfu-
util](https://sourceforge.net/p/dfu-util/dfu-util/ci/master/tree/) and follow building
instructions [HERE](
http://dfu-util.sourceforge.net/build.html).

Once dfu-util is installed you can just flash as a normal board, although
sudo privileges are required.

```
sudo make BOARD=nz32-sc151 flash
```

NOTE: this only works for linux, for windows you can use [ST Microelectronics
DFU Bootloader](http://www.st.com/en/development-tools/stsw-stm32080.html)
although you will need to build the files independently.

## Implementation Status

| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | stm32l1   | partly    | Energy saving modes not implemented |
| Low-level driver | GPIO    | yes       | |
|        | DAC       | yes       | |
|        | UART      | yes           | |
|        | I2C       | yes       |I2C1|
|        | SPI       | yes       | |
|               | ADC           | yes           | Only 5 enabled PC0, PC1, PC2, and two internal ADC channels|
|        | USB       | no        | |
|        | RTT       | no        | |
|        | PWM       | yes       | On PC6, PC7, PC8, PC9|
|        | RTC       | yes       | |
|        | RNG       | no        | |
|        | Timer     | yes           | |
 */
