@defgroup    boards_openlabs-kw41z-mini openlabs.co kw41z-mini board
@ingroup     boards
@brief       Support for openlabs-kw41z-mini

### General information

This is an open-source development board shipped by openlabs.co with
source files located at https://openlabs.co/OSHW/kw41z-mini

A driver for the radio transceiver is available in [#12277](https://github.com/RIOT-OS/RIOT/pull/12277)
(802.15.4 only).

@note A version of this board exists that comes with the `M41W8VT4` (256k ROM)
instead of the `M41W9VT4` (512k ROM). The boards are otherwise identical.
See @ref boards_openlabs-kw41z-mini-256kib if you happen to have this version.

### Programming Pinout

Programming and debugging can be done with a Raspberry Pi (or equivalent)
running OpenOCD.

       Pi GPIO Pins       kw41z-mini
    ====================================
                     ||
       GPIO_19 ------||------- RST
                     ||
       GPIO_20 ------||------- SWDCLK
                     ||
       GPIO_21 ------||------- SWDIO
                     ||
         3.3V -------||------- 3.3V
                     ||
          GND -------||------- GND
                     ||

### Compiling and Flashing

# install build deps for openocd

    apt install git build-essential libtool automake

# fetch and build openocd with support for JTAG via RPi or generic GPIO

    git clone https://github.com/beduino-project/openocd.git
    cd openocd
    ./bootstrap
    ./configure --enable-bcm2835gpio --enable-sysfsgpio
    make -j4 && sudo make install

# install arm toolchain

    apt install git gcc-arm-none-eabi gdb-arm-none-eabi

# fetch Riot

    git clone https://github.com/RIOT-OS/RIOT.git

# build and flash the gnrc_networking example

    cd RIOT/examples/networking/gnrc/gnrc_networking
    BOARD=openlabs-kw41z-mini CFLAGS+="-DKW41ZRF_ENABLE_LEDS=1" make -j4 flash

### Debug Uart Pinout

It is also possible to use the Pi for connecting to the debug uart.

       Pi GPIO Pins     kw41z-mini
    ==================================
                    ||
        UART TX ----||------ RXI
                    ||
        UART RX ----||------ TXO
                    ||
          GND ------||------ GND
                    ||

# enable the uart on the Pi

    echo "enable_uart=1" >> /boot/config.txt
    reboot

# install serial terminal

    apt install picocom

# run serial terminal to access debug uart

    picocom -b115200 /dev/serial0

It is recommended (if possible) to switch the Pi to the better-clocked
uart for higher baud rates:

    echo "dtoverlay=disable-bt" >> /boot/config.txt
    reboot

### Notes

To get a working radio, add #12277 and uncomment the lines in
`boards/openlabs-kw41z-mini/Makefile.dep`

The DAC output is on pin `PTB18`.

ADC input A3 needs a hack applied to `cpu/kinetis/periph/adc.c` to remove
`ADC_CFG2_MUXSEL_MASK` from `CFG2` to switch to the correct mux setting.

The PWM peripheral on this board isn't supported in the Kinetis PWM driver yet.

Use external I2C pullup resistors if I2C is used.
