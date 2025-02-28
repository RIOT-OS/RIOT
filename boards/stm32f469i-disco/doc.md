@defgroup    boards_stm32f469i-disco STM32F469I-DISCO
@ingroup     boards
@brief       Support for the STM32F469I-DISCO board

## Table of Contents:

1. Overview
2. Hardware
3. Connectors and Pinout
4. Buttons, LEDS and other devices
5. Working with this Dev-Kit.

## Overview
The STM32F469I Discovery kit is a complete development platform for powered by ARM Cortex-M4 core-based
STM32F469NIH6 MCU.
The MCU presents a large list of peripherals available, but specifically this development board exposes:

## Hardware

| MCU               | STM32F469Ixx            |
|:----------------- |:------------------------|
| Family            | ARM Cortex-M4F          |
| Vendor            | ST Microelectronics     |
| RAM               | 384+4 KiB               |
| Flash             | 2 MiB                   |
| Frequency         | 180 MHz                 |
| FPU               | Yes                     |
| Timers            | 12                      |
| ADCs              | 3 12-bit (16 channels each) |
| DACs              | 2 12-bit                |
| USARTs            | 4                       |
| UARTs             | 4                       |
| SPIs              | 6                       |
| I2Cs              | 3                       |
| I2Ss              | 2                       |
| CANs              | 2                       |
| USBs              | 1 Full Speed            |
| Ethernet MAC      | Yes                     |
| RNG               | Yes                     |
| SDRAM             | Yes (16 MByte)          |
| Vcc               | 1.7V - 3.6V             |
| Datasheet         | [MCU Datasheet](https://www.st.com/resource/en/datasheet/stm32f469ae.pdf) (pdf) |
| Reference Manual  | [Dev-Kit Reference Manual](https://www.st.com/resource/en/user_manual/um1932-discovery-kit-with-stm32f469ni-mcu-stmicroelectronics.pdf) (pdf)|

## Connectors and Pinout

- 1 USB Mini-B used by the On-board ST-LINK debugger (Ref.CN1)
- 1 USB Micro-AB Full speed (Ref. CN13)
- Arduino(R) Uno expansion connectors (Refs. CN5/6/7/8)
- microSD(tm) card receptacle.
- Audio 3.5 mm stereo jack

Through 2 connector we have available:

### CN11

- I2C1
- Reset

Also provides power pins, 3V3 and GND

### CN12

This extension connector gives access to:

- CAN2
- USART6 (Tx, Rx)
- I2S2
- SPI1
- 7 timers channels
- Speaker output (1W)

Also provides some system signals and power.

**Important!** note than some pins are shared between peripherals.

## Buttons, LEDS and other devices

- 1 Button for reset
- 1 User button
- 4 LEDs
- 3 Digital microphones
- 1 LCD Color 4 inches

## Working with this Dev-Kit

To start the **gcc-arm-embedded** toolchain have to be installed,
we can follow the usual process [here](https://github.com/RIOT-OS/RIOT/wiki/Family:-ARM).

## Flashing the device

### Flashing the Board using OpenOCD

The start install **OpenOCD**, also we can follow the usual process
[here](https://github.com/RIOT-OS/RIOT/wiki/OpenOCD)

Once everything is installed and ready to work just connect the board through
the USB Mini-B connector try to compile and flash some code, type:

```
make flash BOARD=stm32f469i-disco
```

### Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=stm32f469i-disco PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware update
s
could be found on [this STM webpage](https://www.st.com/en/development-tools/sts
w-link007.html).
