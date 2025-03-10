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

The board definitions with RIOT retain the SoftDevice firmware blob shipped with
the original Adafruit nRF52 Bootloader that is used by i.a. Arduino and
CircuitPython but not used by RIOT. If you want to override the SoftDevice
to avoid having closed source software on your microcontroller or if you need
the additional space, you can specify the `UF2_SOFTDEV=DROP` option in your
`make` command. **Be aware that you have to reflash the SoftDevice if you want
to use it in the future!**

```
     make UF2_SOFTDEV=DROP BOARD=feather-nrf52840 -C examples/basic/hello-world flash term
```

If your board features a non-standard SoftDevice version, you can specify it
with the `UF2_SOFTDEV` option as well. Currently supported are i.a. `SD611`
for SoftDevice Version 6.1.1 and `SD730` for Version 7.3.0.
**Setting the wrong SoftDevice version might corrupt the SoftDevice and
make a reflash of the SoftDevice necessary. Do not change it unless you know
what you're doing!**

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
make BOARD=feather_nrf52840_express flash-sd
~~~~~~~~~~~~~

[nrfjprog]: https://www.nordicsemi.com/Products/Development-tools/nRF-Command-Line-Tools

## Updating Old Bootloaders and Restoring the SoftDevice

In some cases the bootloader may be too old to even mount on startup.
Enter the bootloader mode (usually by double tapping the reset button for
nRF52840 based boards) and check the `INFO_UF2.TXT` for bootloader information.
If the version is less than `0.4.0` then one can use the
[Adafruit_nRF52_Bootloader](https://github.com/adafruit/Adafruit_nRF52_Bootloader)
tool to update.

Until and including RIOT Release 2025.01, the SoftDevice on `feather-nrf52840` and
`feather-nrf52840-sense` boards got erased when flashing RIOT. To restore the
ability of using i.a. Arduino and CircuitPython, the SoftDevice has to be
flashed again. This can be done in the same way as updating the bootloader.
Sometimes the process has to be done twice until the SoftDevice is successfully
restored.

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
