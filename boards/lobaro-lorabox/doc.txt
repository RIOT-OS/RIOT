/**
@defgroup    boards_lobaro_lorabox Lobaro Lorabox
@ingroup     boards
@brief       Support for the Lobaro LoraBox with stm32l151cb-a

## Hardware

![LoraBox](https://www.lobaro.com/wp/wp-content/uploads/2017/03/Lobaro_wMBUS_LoRaWAN_Bridge.jpg)


### MCU
| MCU                   | stm32l151cb-a                             |
|:--------------------- |:----------------------------------------- |
| Family                | ARM Cortex-M3                             |
| Vendor                | ST Microelectronics                       |
| RAM                   | 16KiB                                     |
| Flash                 | 128KiB                                    |
| Frequency             | 32MHz (no external oscillator connected)  |
| FPU                   | no                                        |
| Timers                | 10 (8x 16-bit, 2x watchdog timers)        |
| ADCs                  | 1x 24-channel 12-bit                      |
| UARTs                 | 3                                         |
| SPIs                  | 2                                         |
| I2Cs                  | 2                                         |
| Vcc                   | 1.65V - 3.6V                              |
| Datasheet             | [Datasheet](https://www.st.com/resource/en/datasheet/stm32l151cb-a.pdf) |
| Reference Manual      | [Reference Manual](https://www.st.com/content/ccc/resource/technical/document/reference_manual/cc/f9/93/b2/f0/82/42/57/CD00240193.pdf/files/CD00240193.pdf/jcr:content/translations/en.CD00240193.pdf) |
| Programming Manual    | [Programming Manual](https://www.st.com/content/ccc/resource/technical/document/programming_manual/5b/ca/8d/83/56/7f/40/08/CD00228163.pdf/files/CD00228163.pdf/jcr:content/translations/en.CD00228163.pdf) |
| Board Manual          | [Board Manual](https://www.lobaro.com/download/7250/)|

### User Interface

1 LEDs:

| NAME   | LED0   |
| -----  | ----- |
| Color  | green |
| Pin    | P1   |


## Flashing
### Connections
To flash using the STM32 ROM bootloader on the board, use the provided UART-USB
bridge and connect it to the *Config* port. The *Config* port pinout is the
following:

```
---------------     ---------------
| 1 2 3 4 5 6 |     | x x x x x x |
---------------     ---------------
    Config               Addon

1: RST
2: VCC
3: RX1
4: TX1
5: Boot0
6: GND
```

### STM32 Loader
To flash RIOT on the board, after connection the UART-USB bridge, just run:
```
BOARD=lobaro-lorabox make flash
```
This uses the stm32loader script to erase the memory and flash it interfacing
with the STM32 ROM bootloader.

### Lobaro Tool
Another way of interfacing with the STM32 ROM bootloader is using the **Lobaro Maintenance Tool** provided
[online](https://www.lobaro.com/lobaro-maintenance-tool/) for free for Linux,
Mac & Windows. It allows flashing and accessing the UART.

![LobaroTool](https://www.lobaro.com/wp/wp-content/uploads/2018/03/Lobaro_Tool_FirmwareUpdateFeature.png)

## Connecting via Serial
The default UART port is the USART1, the same that is used for flashing, so it
is available on the *Config* port. The default port is /dev/ttyUSB0. To access
the port run:
```
BOARD=lobaro-lorabox make term
```
**Note:** If you want to access the port with a different application please
keep in mind that RTS must be set to '0' and DTR to '1' as the provided UART-USB
bridge seems to invert this lines.
 */

## SX1272 radio
Please note that the board has a Semtech SX1272 radio. This means that when the
semtech-loramac package or the sx127x driver are used the correct driver version
(sx1272) must be selected.
