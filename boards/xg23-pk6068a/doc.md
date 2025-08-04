@defgroup   boards_xg23-pk6068a Silicon Labs xG23-PK6068A starter kit
@ingroup    boards
@brief      Support for the xG23-PK6068A board.

## Overview
The EFR32xG23 (xG23) Pro Kit is designed to support the development of Wireless
IoT devices based on Sub-GHz wireless protocols. The Pro Kit includes a radio
board that provides a complete reference design for the EFR32xG23 Wireless SoC,
with the matching network for +20 dBm covering 868 to 915 MHz.

The main board contains an onboard J-Link debugger with a Packet Trace
Interface and a Virtual COM port, enabling application development and
debugging of the attached radio board as well as external hardware through an
expansion header.

## Hardware

### MCU
| MCU             | EFR32ZG23                                                                                        |
|-----------------|--------------------------------------------------------------------------------------------------|
| Family          | ARM Cortex-M33                                                                                   |
| Vendor          | Silicon Labs                                                                                     |
| Vendor Family   | EFM32 Wireless Gecko                                                                             |
| RAM             | 64.0 KiB                                                                                         |
| Flash           | 512.0 KiB                                                                                        |
| EEPROM          | no                                                                                               |
| Frequency       | up to 78 MHz                                                                                     |
| FPU             | yes                                                                                              |
| MPU             | no                                                                                               |
| DMA             | 8 channels                                                                                       |
| Timers          | 4x 16-bits, 1x 32-bits                                                                           |
| ADCs            | 16-bit ADC                                                                                       |
| UARTs           | 1x USART, 3x EUSART                                                                              |
| SPIs            | 1x USART                                                                                         |
| I2Cs            | 2x                                                                                               |
| Vcc             | 1.71 V - 3.8 V                                                                                   |
| Datasheet       | [EFR32ZG23](https://www.silabs.com/documents/public/data-sheets/efr32zg23-datasheet.pdf)         |
| Manual          | [EFR32xG23](https://www.silabs.com/documents/public/reference-manuals/efr32xg23-rm.pdf)          |
| Board Manual    | [xG23](https://www.silabs.com/documents/public/user-guides/ug507-brd4210a-user-guide.pdf)        |
| Board Schematic | [Main Board BRD4001A](https://www.silabs.com/documents/public/schematic-files/BRD4001A-A01-schematic.pdf), [EFR32ZG23 Module BRD4210A](https://www.silabs.com/documents/public/schematic-files/BRD4210A-A01-schematic.pdf) |

### Pinout

This is the pinout of the expansion header on the right side of the board.
PIN 1 is the bottom-left contact when the header faces  you horizontally.

|      | PIN | PIN |      |
|------|-----|-----|------|
| 3V3  | 20  | 19  | RES  |
| 5V   | 18  | 17  | RES  |
| PC7  | 16  | 15  | PC5  |
| PA9  | 14  | 13  | PA7  |
| PA8  | 12  | 11  | PA6  |
| PC0  | 10  | 9   | PD2  |
| PC3  | 8   | 7   | PA5  |
| PC2  | 6   | 5   | PA0  |
| PC1  | 4   | 3   | PA10 |
| VMCU | 2   | 1   | GND  |


### Peripheral mapping
| Peripheral | Number  | Hardware        | Pins                           | Comments                         | Min. Power Mode |
|------------|---------|-----------------|--------------------------------|----------------------------------|-----------------|
| I2C        | 0       | I2C0            | SDA: PC7,  CLK: PC5            |                                  | EM1             |
| SPI        | 0       | USART0          | MOSI: PC1, MISO: PC2, CLK: PC3 | PC4 used as CS for Serial Flash  | EM1             |
| Timer      | 0       | TIMER0          |                                | Default ztimer backend           | EM1             |
|            | 1       | LETIMER0        |                                |                                  | EM3             |
| UART       | 0       | EUSART1         | RX: PA9, TX: PA8               | Default STDIO output             | EM1             |
| ADC_LINE   | 0       | IADC0           | IN+: PA10, IN-: GND            | Single-ended, V_ref: 2 * 1.21V   | EM1             |
|            | 1       | IADC0           | IN+: PA0, IN-:PA5              | Differential, V_ref: 2 * 1.21V   | EM1             |

### User interface
| Peripheral | Mapped to | Pin  | Comments   |
|------------|-----------|------|------------|
| Button     | PB0       | PB1  |            |
|            | PB1       | PB3  |            |
| LED        | LED0      | PB2  | Yellow LED |
|            | LED1      | PD3  | Yellow LED |

## Implementation Status
| Device                        | ID          | Supported | Comments                                                       |
|-------------------------------|-------------|-----------|----------------------------------------------------------------|
| MCU                           | EFR32ZG     | yes       | Power modes supported                                          |
| Low-level driver              | ADC         | no        |                                                                |
|                               | Flash       | yes       |                                                                |
|                               | GPIO        | yes       | Interrupts are shared across pins (see reference manual)       |
|                               | HW Crypto   | partially | Only hwrng                                                     |
|                               | I2C         | yes       |                                                                |
|                               | PWM         | no        |                                                                |
|                               | RTC         | no        | As RTT or RTC                                                  |
|                               | SPI         | partially | Only master mode                                               |
|                               | Timer       | yes       |                                                                |
|                               | UART        | yes       |                                                                |
| LCD driver                    | LS013B7DH03 | yes       | Sharp Low Power Memory LCD via the U8g2 package                |
| Temperature + humidity sensor | Si7021      | yes       | Silicon Labs Temperature + Humidity sensor                     |

## Flashing the device
To flash, [SEGGER JLink](https://www.segger.com/jlink-software.html) is
required.

Flashing is supported by RIOT-OS using the command below:

```
make flash
```

To run the GDB debugger, use the command:

```
make debug
```

Or, to connect with your own debugger:

```
make debug-server
```

## License information
Silicon Labs' EMLIB: zlib-style license (permits distribution of source).
