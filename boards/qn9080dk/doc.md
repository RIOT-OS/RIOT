@defgroup    boards_qn9080dk QN9080DK base board
@ingroup     boards
@brief       Support for the QN9080DK base board.

### General Information

@image html "https://lv.farnell.com/productimages/large/en_GB/2775176-40.jpg" "Top View of the QN9080DK V1.2" width=80%

The QN9080DK is the developer board reference from NXP for the QN908x CPUs.
The developer kit comes with two boards: a larger PCB with a QFN "module"
including a QN9080DHN soldered onto it, altogether referred as the "base
board"; and separately a much smaller USB dongle with another QN9080 cpu.
This board module supports the "base board" only, including the definitions
of the gpio, buttons and LEDs specific to this board.

This board packs a few peripherals and many GPIOs:
- MMA8652FC, a 12-bit accelerometer

The board also features a dedicated programmer hardware implemented using NXP's
LPC4322, with its dedicated USB port for connecting to a computer on one end
and the SWD port and one UART RX/TX pair connected to the QN9080 on the other
end. The LPC4322 also has a fast (50 ksps) ADC ADC122S021 connected to a
current sense amplifier to monitor the QN9080 current profile directly on
the board.

For more information visit NXP's product description page for the
[QN9080-DK](https://www.nxp.com/QN9080DK) where you can find the User's
Guide.

### Pinout

#### Buttons:

| Label | RIOT OS macro | MCU PIN  | Function  |
|:----- |:------------- |:-------- |:--------- |
| SW1   | BTN1_PIN      | PA24     | User defined |
| SW2   | BTN2_PIN      | PA19     | User defined |
| SW3   |               | nRESET   | QN9080 reset |


#### One RGB led, controlled by three GPIOs

| Label | RIOT OS macro | MCU PIN  |
|:----- |:------------- |:-------- |
| red   | LED_RED_PIN   | PA31     |
| green | LED_GREEN_PIN | PA25     |
| blue  | LED_BLUE_PIN  | PA13     |

#### SPI Flash

| Name  | MCU Pin   |
|:----- |:--------- |
| MISO  | PA05      |
| MOSI  | PA04      |
| SSEL  | PA03      |
| SCK   | PA30      |

#### USB

| Name  | MCU Pin   |
|:----- |:--------- |
| D+    | PA26      |
| D-    | PA27      |

#### I2C / MMA8652FC G-Sensor

@note   The jumper JP11 needs to be in place to connect the I2C sensor to the
        pins. This is the default when freshly unboxed.

| Name  | MCU Pin   |
|:----- |:--------- |
| SCL   | PA06      |
| SDA   | PA07      |

The MMA8652 sensor has I2C address 0x1d.

### Flashing the Board

The integrated programmer by default comes with an "LPC-LINK2" firmware that
provides a CMSIS DAP compatible interface. The programmer can be flashed with
a Segger J-Link firmware via DFU, see the [LPCSCrypt User Guide][LPCScrypt] for
details. It seems that the "LPC-LINK2" firmware the board is shipped with is
unreliable. Updating that as also described in the
[LPCSCrypt User Guide][LPCScrypt] or  switching to the J-Link firmware is
recommended.

@warning    The update software is quite flaky. Do not connect other USB
            device providing a serial (e.g. CDC ACM) to not confuse it. You may
            need to adapt the shell script to grep for the correct vid/pid pair
            using hex values prefixed with `0x` when using a `dfu-util` in a
            more recent version than the script expects.

@note       The Windows version of the software is at least equally frustrating.
            So you may was well use the Linux variant and fix the
            shell script as needed rather than spinning up a Windows VM in the
            hope to safe pain in the ass.

[LPCScrypt]: https://web.archive.org/web/20220225151231/https://www.nxp.com/docs/en/user-guide/LPCScrypt_User_Guide.pdf

#### Using the Internal Programmer with Default Firmware

In the application directory, run:

```
make BOARD=qn9080dk flash
```

OpenOCD support for the QN908x flash is as of June 2023 not yet part of any
release, but has been merged upstream. It is expected to be included first in
the 0.13 release of OpenOCD. Until then, compiling a version from the current
git source is the needed.

#### Using the Internal Programmer with J-Link Firmware

In the application directory, run:

```
make BOARD=qn9080dk QN9080DK_JLINK=1 flash
```

@note Instead of passing `QN9080DK_JLINK=1` every time, it can also be exported
      as environment variable.

#### Using an External Programmer

By setting the jumper at `JP2` to `EXT` the internal LPC4322 debugger/programmer
is disconnect and an external programmer can be connected at header `P1` using
a standard 10 pin 1.27 mm debug connector.
