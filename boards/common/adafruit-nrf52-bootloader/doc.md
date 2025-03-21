@defgroup  boards_common_adafruit-nrf52-bootloader Adafruit nRF52 Bootloader Common
@ingroup   boards_common
@brief     Common Flashing Description for the Adafruit nRF52 Bootloader

## Introduction

The Adafruit nRF52 Bootloader is used by many Adafruit and Seeedstudio boards
that are based on the Nordic Semiconductor nRF52 microcontroller family.
It features a Mass Storage Device emulation, so that UF2 files can be uploaded
via Drag and Drop. It is also backwards compatible to the BOSSA bootloader.
With the Adafruit nRF52 Bootloader, no external debugger such as a J-Link is
necessary for the normal usage.

In some cases the bootloader may be too old to even mount on startup.
Enter the bootloader mode (usually by double tapping the reset button for
nRF52840 based boards) and check the `INFO_UF2.TXT` for bootloader information.
If the version is less than `0.4.0`, please refer to @ref ada-nrf52-update.

Until and including RIOT Release 2025.01, the SoftDevice on `feather-nrf52840`
and `feather-nrf52840-sense` boards got erased when flashing RIOT. To restore
the ability of using i.a. Arduino and CircuitPython, the SoftDevice has to be
flashed again. Please also refer to @ref ada-nrf52-update.

If the bootloader is not present on your board at all, an external debugger
such as the J-Link has to be used. Please refer to @ref ada-nrf52-reflashing.

## Flashing the Board with RIOT

The board is flashed using its on-board UF2 bootloader by default.
The bootloader will present a mass storage device that has to be mounted so
the RIOT buildsystem can find it. If you have an auto-mounter installed this
will happen automatically.

The rest of the process is automated in the usual way with e.g.
`BOARD=feather-nrf52840`.

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

## Updating Old Bootloaders and Restoring the SoftDevice {#ada-nrf52-update}

If the Adafruit nRF52 Bootloader is still present on the board but outdated
or the SoftDevice got erased (the `INFO_UF2.TXT` file will show
`SoftDevice: not found`), you can flash the bootloader via DFU.

As prerequisites, the `arm-none-eabi-gcc` compiler (which is usually already
present if you compiled something for your nRF52 board) and the
`adafruit-nrfutil` have to be installed.

Details on the installation process can be found in [Adafruit's GitHub
repository](https://github.com/adafruit/Adafruit_nRF52_Bootloader).

@note The Adafruit nRF52 Bootloader uses other board names than RIOT. The
      `feather-nrf52840` for example is called `feather_nrf52840_express`.
      Be sure to check the `boards/` subfolder in the bootloader directory
      to find out the correct board name.

With the following commands, the bootloader with SoftDevice will be compiled
and flashed. For MacOS, you might have to select a different serial port.

```
git clone https://github.com/adafruit/Adafruit_nRF52_Bootloader.git
cd Adafruit_nRF52_Bootloader
git submodule update --init
pip3 install --user adafruit-nrfutil
make BOARD=feather_nrf52840_sense SERIAL=/dev/ttyACM0 flash-dfu
```

Sometimes the process has to be done twice until the SoftDevice is successfully
restored.

Further information about the update process can be found on the
[Adafruit Website](https://learn.adafruit.com/adafruit-feather-sense/update-bootloader).

## (Re-)Flashing the UF2 Bootloader with an External Debugger {#ada-nrf52-reflashing}

To flash the UF2 bootloader (if it is no longer present on your board),
you need to have the original [`nrfjprog`][nrfjprog] installed and a compatible
debugger, such as a J-Link. Make sure to select the right board (see the note
in @ref ada-nrf52-update).

~~~~~~~~~~~~~{.sh}
git clone https://github.com/adafruit/Adafruit_nRF52_Bootloader
cd Adafruit_nRF52_Bootloader
git submodule update --init
nrfjprog --recover
make BOARD=feather_nrf52840_express flash
make BOARD=feather_nrf52840_express flash-sd
~~~~~~~~~~~~~

More information about how to flash nRF52 microcontrollers can be found in the
**Flashing** section in @ref boards_common_nrf52. Some variants of the
Adafruit Feather feature an [SWD Connector], to which an external debugger can
be connected. Other boards such as the Seeedstudio Xiao nRF52840 only have
solder pads.

[SWD Connector]: https://learn.adafruit.com/introducing-the-adafruit-nrf52840-feather/pinouts#swd-connector-3-12

[nrfjprog]: https://www.nordicsemi.com/Products/Development-tools/nRF-Command-Line-Tools
