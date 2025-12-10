@defgroup    boards_bitcraze_crazyflie21_main bitcraze Crazyflie 2.1 Main Application MCU
@ingroup     boards
@brief       Support for the main application MCU of the bitcraze Crazyflie 2.1 board.

### General information

The Crazyflie 2.1 is an open source flying development platform produced by bitcraze.
It has two onboard microcontrollers, an nRF51822 used for radio communication and power management and
an STM32F405 (this board definition) used for running the main application.

### Pinout

The pinout is part of the [Board Datasheet][board-datasheet].
The [Board Schematics][board-schematics] show which pins are connected to the motors, LEDs, etc.

[board-datasheet]: https://www.bitcraze.io/documentation/hardware/crazyflie_2_1/crazyflie_2_1-datasheet.pdf
[board-schematics]: https://www.bitcraze.io/documentation/hardware/crazyflie_2_1/crazyflie_2.1_schematics_rev.b.pdf

### MCU

| MCU                  | STM32F405RG                                 |
|:---------------------|:--------------------------------------------|
| Family               | ARM Cortex-M4                               |
| Vendor               | ST Microelectronics                         |
| Package              | LQFP64                                      |
| RAM                  | 192 KiB (128 KiB RAM + 64 KiB CCMRAM)       |
| Flash                | 1024 KiB                                    |
| Frequency            | up to 168 MHz (running at 168 MHz)          |
| FPU                  | yes                                         |
| Timers               | 14    (12× 16bit, 2× 32bit [TIM2 + TIM5])   |
| ADCs                 | 3× 16 channel 6 to 12-bit                   |
| UARTs                | 6                                           |
| SPIs                 | 3                                           |
| I2Cs                 | 3                                           |
| Vcc                  | 2.0 V - 3.6 V                               |
| MCU Datasheet        | [MCU Datasheet][mcu-datasheet]              |
| MCU Reference Manual | [MCU Reference Manual][mcu-ref]             |

[mcu-datasheet]: https://www.st.com/resource/en/datasheet/stm32f405rg.pdf
[mcu-ref]: https://www.st.com/resource/en/reference_manual/dm00031020-stm32f405-415-stm32f407-417-stm32f427-437-and-stm32f429-439-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf

### Flash the board
Prerequisites:
- the Crazyflie NRF firmware runs on the NRF [instructions](https://www.bitcraze.io/documentation/repository/crazyflie2-nrf-firmware/master/build/build/)
- the original Crazyflie bootloader runs on the STM32 [instructions](https://www.bitcraze.io/documentation/repository/crazyflie-firmware/master/development/dfu/#bootloader-recovery)
- `dfu-util` is installed
- the Crazyflie's battery is disconnected

To flash the board:
- navigate to the folder of the app you want to flash
- Hold down the power button on the Crazyflie
- connect via usb to your computer
- keep holding down the button for four seconds until the blink rate changes (the mcu is now in DFU mode)
- flash the board by running `BOARD=bitcraze-crazyflie21-main make all flash`

### STDIO
By default, STDIO is implemented via the native USB interface.
In order to use STDIO over UART connect your UART adapter as follows:

| PIN board | UART adapter |
|:----------|:-------------|
| PC10 (TX) | RX           |
| PC11 (RX) | TX           |
| GND       | GND          |

Now flash the board with stdio over UART:
```
USEMODULE=stdio_uart BOARD=bitcraze-crazyflie21-main make all flash
```

### Known Issues
 - `timer_set()`: the callback is never called for timeouts of only one tick.
