/**
@defgroup    boards_nrf52840dongle nRF52840-Dongle
@ingroup     boards
@brief       Support for the nRF52840-Dongle

### Quick start

- Plug into a USB port.
- `$ make BOARD=nrf52840dongle -C examples/saul flash term`
  - See [Flash the board](#nrf52840dongle_flash) if anything goes wrong.
- `> saul write 2 10 40 10`
  - The LED glows in a soft turquise.

### General information

The nRF52840-Dongle is a bare USB-stick shaped device that houses barely
anything than the nRF52840 itself, which offers BLE and 802.15.4 and USB
connectivity.

Unlike similar sticks (like the @ref boards_nrf52840-mdk), it features no
dedicated programmer hardware but relies on direct USB communication with a
built-in bootloader.

The board features two LEDs (LD1: green, LD2: RGB), a user (SW1) and a
reset button as well as 15 configurable external pins.

### Links

- [nRF52840 web page](https://www.nordicsemi.com/?sc_itemid=%7BCDCCA013-FE4C-4655-B20C-1557AB6568C9%7D)
- [documentation and hardware description](https://infocenter.nordicsemi.com/topic/ug_nrf52840_dongle/UG/nrf52840_Dongle/intro.html?cp=3_0_5)

### <a name=nrf52840dongle_flash> Flash the board </a>

The board is flashed using its on-board boot loader; the
[nrfutil](https://github.com/NordicSemiconductor/pc-nrfutil) program needs to
be installed. That can turn the binary into a suitable zip file and send it to the
bootloader.

The process is automated in the usual `make flash` target.

If RIOT is already running on the board, it will automatically reset the CPU and enter
the bootloader.
If some other firmware is running or RIOT crashed, you need to enter the bootloader
manually by pressing the board's reset button.

Readiness of the bootloader is indicated by LD2 pulsing in red.

#### nrfutil installation

On systems with Python 2 available, `pip install nrfutil` works.

On systems with Python 3, a recent version of pip is required to install all dependencies;
you may need to run `pip install --upgrade pip` before being able to run `pip install nrfutil` successfully.

### Accessing STDIO

The usual way to obtain a console on this board is using an emulated USB serial port (CDC-ACM).
This is available automatically using the `stdio_cdc_acm` module,
unless any other stdio module is enabled.

On Linux systems with ModemManager < 1.10 installed,
`make term` will, in some setups, fail for a few seconds after the device has come up.
This is [fixed in its 1.12.4 version](https://gitlab.freedesktop.org/mobile-broadband/ModemManager/issues/164),
but should not be more than a short annoyance in earlier versions.

To ease debugging,
pins 0.13 and 0.15 are configured as RX and TX, respectively.
They provide stdio if no CDC-ACM is disabled,
and can be used as a custom UART otherwise.

 */
