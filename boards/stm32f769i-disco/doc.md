@defgroup    boards_stm32f769i-disco STM32F769I-DISCO board
@ingroup     boards
@brief       Support for the STM32F769I-DISCO board

## Overview

The ST [STM32F769I-DISCO](https://www.st.com/en/evaluation-tools/32f769idiscovery.html)
is a development board from ST's Discovery family built around the ARM
Cortex-M7 STM32F769NI MCU, with a 4 inches 800x472 touch display, Ethernet,
USB OTG HS, audio, microSD and 16 MiB of SDRAM.

The kit includes the MB1225 main board and the MB1166 display daughterboard

## Hardware

| MCU               | STM32F769NI                       |
|:----------------- |:--------------------------------- |
| Family            | ARM Cortex-M7                     |
| Vendor            | ST Microelectronics               |
| RAM               | 512+16+4 KiB                      |
| Flash             | 2 MiB                             |
| Frequency         | 216 MHz                           |
| FPU               | Yes (double precision)            |
| Timers            | 15 (13 16-bit, 2 32-bit)          |
| ADCs              | 3 12-bit (24 channels)            |
| DACs              | 2 12-bit                          |
| USARTs            | 4                                 |
| UARTs             | 4                                 |
| SPIs              | 6                                 |
| I2Cs              | 4                                 |
| SAIs              | 2                                 |
| CANs              | 3                                 |
| USBs              | 1 Full Speed, 1 High Speed (ULPI) |
| Ethernet MAC      | Yes                               |
| RNG               | Yes                               |
| SDRAM             | Yes (16 MByte)                    |
| Vcc               | 1.7V - 3.6V                       |
| Datasheet         | [MCU Datasheet](https://www.st.com/resource/en/datasheet/stm32f769ni.pdf) (pdf) |
| Reference Manual  | [RM0410](https://www.st.com/resource/en/reference_manual/rm0410-stm32f76xxx-and-stm32f77xxx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) (pdf) |
| User Manual       | [UM2033](https://www.st.com/resource/en/user_manual/um2033-discovery-kit-with-stm32f769ni-mcu-stmicroelectronics.pdf) (pdf) |

## Connectors and Pinout

- 1 USB Micro-B for the on-board ST-LINK/V2-1 debugger and its virtual COM port (CN16)
- 1 USB Micro-AB High Speed, through an external ULPI PHY (CN15)
- 1 Ethernet RJ45 (CN10)
- Arduino Uno V3 expansion connectors (CN11, CN14, CN13 and CN9)
- Wi-Fi module (ESP-01) and I2C extension connector (CN2)
- microSD card connector (CN5)
- MIPI DSI connector (CN1), for the display daughterboard
- Audio line in (CN6) and line out (CN7) jacks, stereo speaker outputs (JP2, JP3)
- SPDIF RCA connectors: input (CN12, white) and output (CN8, yellow)

The Arduino pins D0-D15 and A0-A5 are mapped in `arduino_iomap.h`.

## Buttons, LEDs and other devices

- B1 (blue): user button on PA0, `BTN0`
- B2 (black): reset button
- LD1 (red, PJ13) and LD2 (green, PJ5): user LEDs, `LED0` and `LED1`
- LD3 (green, PA12): Arduino D13 LED, `LED2`, shared with SPI2 SCK
- LD4 (red): USB OTG HS overcurrent, driven by PD4, which must not be used as
  an output
- 4 inches 800x472 capacitive touch display, with an FT6x06 touch controller on
  I2C4
- WM8994 audio codec on I2C4 and four digital MEMS microphones on DFSDM1

## Current hardware support

| Feature                 | Support | Configuration                                         |
|:----------------------- |:-------:|:----------------------------------------------------- |
| UART                    | X       | USART1 on PA10/PA9 (stdio, ST-LINK virtual COM port)  |
|                         |         | UART5 on PD2/PC12 (Wi-Fi connector CN2)               |
|                         |         | USART6 on PC7/PC6 (Arduino D0/D1)                     |
| I2C                     | X       | I2C1 on PB8/PB9 (Arduino D15/D14)                     |
|                         |         | I2C4 on PD12/PB7 (touch panel and audio codec)        |
| SPI                     | X       | SPI2 on Arduino D13/D12/D11                           |
| PWM                     | X       | Arduino D3, D5, D6, D9, D10 and D11                   |
| ADC                     | X       | Arduino A0-A5 and VBAT                                |
| DAC                     | X       | DAC channel 1 on Arduino A1                           |
| DMA                     | X       |                                                       |
| RTC and RTT             | X       |                                                       |
| Ethernet                | X       | RMII, LAN8742A PHY                                    |
| PTP clock               | X       |                                                       |
| USB OTG HS              | X       | ULPI PHY on CN15, see the note below                  |
| External SDRAM          | X       | FMC bank 5 at 0xc0000000                              |
| SD card interface       | X       | SDMMC2, card detect on PI15                           |
| Touch panel             | X       | FT6x06 on I2C4                                        |
| LCD                     | -       |                                                       |
| Audio                   | -       |                                                       |
| Digital microphones     | -       |                                                       |
| External Quad-SPI flash | -       |                                                       |

The USB OTG HS port works with TinyUSB; USBUS stack does not work on
it yet.

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the STM32F769I-DISCO is `stm32f769i-disco`.

## Accessing the serial console

The stdio is available on USART1, which is connected to the virtual COM port of
the on-board ST-LINK. Use `make BOARD=stm32f769i-disco term` to open a terminal.
