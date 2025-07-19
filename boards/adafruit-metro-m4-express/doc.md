@defgroup    boards_adafruit-metro-m4-express Adafruit Metro M4 Express
@ingroup     boards
@brief       Support for the Adafruit Metro M4 Express

General information
===================

@image html https://cdn-learn.adafruit.com/assets/assets/000/085/521/large1024/adafruit_products_metro-m4-top.jpg "Picture of the Adafruit Metro M4 Express"

The main features of the board are:
- ATSAMD51 Cortex M4 running at 120 MHz
- Hardware DSP and floating point support
- 512 MiB Flash
- 192 KiB KiB RAM
- external 2 MiB QSPI Flash storage
- WS281x RGB LED (NeoPixel)
- 32-bit, 3.3V logic and power
- native USB

Pinout
------

@image html https://cdn-learn.adafruit.com/assets/assets/000/111/183/original/adafruit_products_Adafruit_Metro_M4_Express_Pinout.png "Official Pinout of the Metro M4 Express" width=100%

Links
=====

- [Official Board Documentation](https://learn.adafruit.com/adafruit-metro-m4-express-featuring-atsamd51)

Schematics
==========

@image html https://cdn-learn.adafruit.com/assets/assets/000/053/093/original/adafruit_products_schem.png "Original Board Schematics"

Arduino Compatibility
=====================

The board is compatible with Shields for the Arduino UNO / Arduino Zero.

@warning    The SPI bus on D11/D12/D13 is only available when `periph_uart` is
            not used, as this pins are connected to the same SERCOM peripheral.
            This is board design flaw that we cannot work around unless
            resorting to desperate means such as bit-banging either the SPI bus
            or the UART interface.

Flash the board
===============

The board is flashed using its on-board
[boot loader](https://github.com/adafruit/uf2-samdx1).

The process is automated in the usual `make flash` target.

If RIOT is already running on the board, it will automatically reset the CPU
and enter the bootloader. If some other firmware is running or RIOT crashed,
you need to enter the bootloader manually by double tapping the board's reset
button.

Readiness of the bootloader is indicated by LED pulsing in red.

@note   You may need to pass `MOST_RECENT_PORT=0` and manually pass the `PORT`
        if you enter the bootloader by hand.

Accessing STDIO
---------------

The usual way to obtain a console on this board is using an emulated USB serial
port.
