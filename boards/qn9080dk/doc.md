/**
@defgroup    boards_qn9080dk QN9080DK base board
@ingroup     boards
@brief       Support for the QN9080DK base board.

### General information

The QN9080DK is the developer board reference from NXP for the QN908x CPUs.
The developer kit comes with two boards: a larger PCB with a QFN "module"
including a QN9080DHN soldered onto it, altogether referred as the "base
board"; and separately a much smaller USB dongle with another QN9080 cpu.
This board module supports the "base board" only, including the definitions
of the gpio, buttons and LEDs specific to this board.

This board packs a few peripherals and many GPIOs:

 * MMA8652FC, a 12-bit accelerometer
 *

The board also features a dedicated programmer hardware implemented using NXP's
LPC4322, with its dedicated USB port for connecting to a computer on one end
and the SWD port and one UART RX/TX pair connected to the QN9080 on the other
end. The LPC4322 also has a fast (50 ksps) ADC ADC122S021 connected to a
current sense amplifier to monitor the QN9080 current profile directly on
the board.

For more information visit NXP's product description page for the
[QN9080-DK](https://www.nxp.com/QN9080DK) where you can find the User's
Guide.

### User Interface

Buttons:

| Label | RIOT OS macro | MCU PIN  | Function  |
|:----- |:------------- |:-------- |:--------- |
| SW1   | BTN1_PIN      | PA24     | User defined |
| SW2   | BTN2_PIN      | PA19     | User defined |
| SW3   |               | nRESET   | QN9080 reset |


One RGB led, controlled by three GPIOs

| Label | RIOT OS macro | MCU PIN  |
|:----- |:------------- |:-------- |
| red   | LED_RED_PIN   | PA31     |
| green | LED_GREEN_PIN | PA25     |
| blue  | LED_BLUE_PIN  | PA13     |

### Pinout

Button1:

### Flash the board

The board can be flashed using the internal LPC4322 JTAG/SWD debugger or an
external debugger connected to P1, selected using JP2.

OpenOCD support for the QN908x is experimental and available as pending
[patch](http://openocd.zylin.com/#/c/5584/).

 */
