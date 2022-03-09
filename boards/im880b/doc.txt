/**
@defgroup    boards_im880b im880b
@ingroup     boards
@brief       Support for the im880b with stm32l151cb-a

## Hardware

![LoraBox](https://www.wireless-solutions.de/images/stories/products/im880a.png)


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
| Board Manual          | [Board Manual](https://cdn.sos.sk/productdata/29/eb/a68245ed/im880b-l-lorawan.pdf)|

### User Interface

## Flashing
As no usb connector is present on the device an external programmer must
be connected to the board for flashing. You can use JTAG or STLINK programmer.

 *  JTAG:
 *    - JTDO: PB_3 / P39
 *    - JTDI: PA_15 / P38
 *    - JTCK: PA_14 / P37
 *    - JTMS: PA_13 / P34

 *  STLINK:
 *    - NRST: NRST / P7
 *    - SWCLK: PA_14 / P37
 *    - SWDAT: PA_13 / P34

### STM32 Loader
To flash RIOT on the board, after connecting the UART-USB bridge, just run:
```
BOARD=im880b make flash
```
This uses the stm32loader script to erase the memory and flash it interfacing
with the STM32 ROM bootloader.

## Connecting via Serial
The default UART port is the USART1, there is no usb connection a USB/TTL
converter must be used to connect to the board TX & RX pins. The default
port is /dev/ttyUSB0. The pin connections are:

 *  USART1:
 *    - TX: PA_9 / P30
 *    - RX: PA_10 / P31

```
BOARD=im880b make term
```
 */

## SX1272 radio
Please note that the board has a Semtech SX1272 radio. This means that when the
semtech-loramac package or the sx127x driver are used the correct driver version
(sx1272) must be selected.
