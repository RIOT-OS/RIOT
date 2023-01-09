/**
@defgroup    boards_feather-nrf52840 Adafruit Feather nRF52840 Express
@ingroup     boards
@brief       Support for the Adafruit Feather nRF52840 Express

### General information

[The Feather nRF52840 Express][feather-nrf52840] is a development board
from Adafruits Feather board family. It provides native USB support, Bluetooth
Low Energy and IEEE 802.15.4 support via the nRF52840 MCU.

![top-down view on feather-nrf52840][top-down view]

[feather-nrf52840]: https://learn.adafruit.com/introducing-the-adafruit-nrf52840-feather/
[top-down view]: https://cdn-learn.adafruit.com/assets/assets/000/068/578/medium800/circuitpython_Screenshot_2019-01-02_at_12.04.27.png

### Flash the board

The board is flashed using its on-board UF2 boot loader by default.
The boot loader will present a mass storage device that has to be mounted to /media/MDK-DONGLE so
`uf2conv.py` can find it. If you have an auto-mounter installed this will happen automatically.

The rest of the process is automated in the usual `make flash` target.

If RIOT is already running on the board, it will automatically reset the CPU and enter
the bootloader.
If some other firmware is running or RIOT crashed, you need to enter the bootloader
manually by pressing the board's reset button while plugging the device into the

Otherwise See the **Flashing** section in @ref boards_common_nrf52. The easiest way is to
use an external Segger J-Link Programmer connected to the [SWD Connector].

[SWD Connector]: https://learn.adafruit.com/introducing-the-adafruit-nrf52840-feather/pinouts#swd-connector-3-12

#### Flashing the uf2 bootloader

To flash the uf2 bootloader (if its no longer present on your BOARD) then with
a jlink attached to your BOARD and [`nrfjprog`][nrfjprog] installed:

~~~~~~~~~~~~~{.sh}
git clone git@github.com:adafruit/Adafruit_nRF52_Bootloader.git
cd Adafruit_nRF52_Bootloader
# only download what is needed, use
#     `git submodule update --init --recursive`
# to download everything
git submodule update --init
git -C lib/tinyusb submodule update --init hw/mcu/nordic/nrfx/
pip3 install --user adafruit-nrfutil
make BOARD=feather_nrf52840_express all
make BOARD=feather_nrf52840_express flash
~~~~~~~~~~~~~

[nrfjprog]: https://www.nordicsemi.com/Products/Development-tools/nRF-Command-Line-Tools

### Terminal
To connect a terminal to the Feather, RIOT chooses `stdio_cdc_acm` per default.
This lets you access the Feather directly over USB.

You have several alternative possibilities to connect to the board.

1. With
   ~~~~~~~~~~~~~~~~~~~~~ {.mk}
   USEMODULE += stdio_uart
   ~~~~~~~~~~~~~~~~~~~~~
   and an FTDI adapter connected to the Feather's RX and TX ports you can use
   UART-based terminals to connect to the feather
2. With
   ~~~~~~~~~~~~~~~~~~~~~ {.mk}
   USEMODULE += stdio_rtt
   ~~~~~~~~~~~~~~~~~~~~~
   you can use the Segger J-Link Programmer as a serial interface to the device.
*/
