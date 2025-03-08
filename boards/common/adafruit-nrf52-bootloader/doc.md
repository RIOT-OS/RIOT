@defgroup  boards_common_adafruit-nrf52-bootloader Adafruit nRF52 Bootloader Common
@ingroup   boards_common
@brief     Common Flashing Description for the Adafruit nRF52 Bootloader

## Flashing the Board

The board is flashed using its on-board UF2 boot loader by default.
The boot loader will present a mass storage device that has to be mounted to /media/MDK-DONGLE so
`uf2conv.py` can find it. If you have an auto-mounter installed this will happen automatically.

The rest of the process is automated in the usual way with e.g. `BOARD=feather-nrf52840`.

Example with `hello-world` application:
```
     make BOARD=feather-nrf52840 -C examples/basic/hello-world flash term
```

If RIOT is already running on the board, it will automatically reset the CPU and enter
the bootloader.
If some other firmware is running or RIOT crashed, you need to enter the bootloader
manually by double-tapping the board's reset button while the device is connected.

Otherwise see the **Flashing** section in @ref boards_common_nrf52. The easiest way is to
use an external Segger J-Link Programmer connected to the [SWD Connector].

[SWD Connector]: https://learn.adafruit.com/introducing-the-adafruit-nrf52840-feather/pinouts#swd-connector-3-12

## (Re-)Flashing the UF2 Bootloader

To flash the UF2 bootloader (if it is no longer present on your board),
you need to have [`nrfjprog`][nrfjprog] installed,
connect a jlink to your BOARD and execute the following commands.

@note The Adafruit nRF52 Bootloader uses other board names than RIOT. The
      `feather-nrf52840` for example is called `feather_nrf52840_express`.
      Be sure to check the `boards/` subfolder in the bootloader directory
      to find out the correct board name.

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

## Updating Old Bootloaders

In some cases the bootloader may be too old to even mount on startup.
Enter the bootloader mode (usually by double tapping the reset button for
nRF52840 based boards) and check the `INFO_UF2.TXT` for bootloader information.
If the version is less than `0.4.0` then one can use the
[Adafruit_nRF52_Bootloader](https://github.com/adafruit/Adafruit_nRF52_Bootloader)
tool to update.

For example, one can run the following if `arm-none-eabi-gcc` and
`adafruit-nrfutil` are installed. Make sure to select the correct board for your
application.
```
git clone https://github.com/adafruit/Adafruit_nRF52_Bootloader.git
cd Adafruit_nRF52_Bootloader
git submodule update --init
make BOARD=feather_nrf52840_sense SERIAL=/dev/ttyACM0 flash-dfu
```

Further information can be found on the
[Adafruit Website](https://learn.adafruit.com/adafruit-feather-sense/update-bootloader).
