Flashing via RIOT's Build System                                    {#flashing}
================================

[TOC]

General Approach                                            {#flashing-general}
================

In general, flashing a board from RIOT is as straight forward as typing in a
shell (with the application directory as current working directory):

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
make BOARD=<BOARD-TO-FLASH> flash
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This will **rebuild** ***AND*** **flash** the application in the current working
directory for board `<BOARD-TO-FLASH>`, using its default programming tool. If
you want to use an alternative programming tool, say `stm32flash`, use:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
make BOARD=<BOARD-TO-FLASH> PROGRAMMER=stm32flash flash
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To flash without rebuilding use `flash-only` as target instead of `flash`.


Supported Tools                                     {#flashing-supported-tools}
===============

RIOT supports plenty of flashing tools, that are below grouped into general
flashing tools that support multiple MCU families, and specialized tools that
only support one platform.

Note that some programmers require additional configuration on a per board
level or rely on features only available on some boards. Hence, a given board
may not be supported by a programmer listed as supported for the platform of
the board due to a missing board feature, bootloader, or similar.

To ease use the programmers are given by the value to pass via
`PROGRAMMER=<value>`, rather than the official spelling of the programmer.

Compatibility Matrix of Generic Tools       {#flashing-supported-tools-generic}
--------------------

<!-- Note: Add flashers that theoretically support multiple platforms here,
     even if RIOT does only have integration for one platform. The missing
     integration may be added later on. -->


MCU Family     |  `bmp` | `dfu-util` | `jlink` | `openocd` | `pyocd` | `uf2conv`
---------------|--------|------------|---------|-----------|---------|----------
ATmega         |  ✗     | ✗          | ✗       | ✗         | ✗       | ✗
ATXmega        |  ✗     | ✗          | ✗       | ✗         | ✗       | ✗
CC13xx / C26xx |  ✗     | ✗          | ✓       | ✓ (1)     | ✗       | ✗
CC2538         |  ✗     | ✗          | ✓       | ✗         | ✗       | ✗
EFM32          |  ✗     | ✗          | ✓       | ✓         | ✗       | ✗
ESP8266        |  ✗     | ✗          | ✗       | ✗         | ✗       | ✗
ESP32 (Xtensa) |  ✗     | ✗          | ✗       | ✗         | ✗       | ✗
ESP32 (RISC-V) |  ✗     | ✗          | ✗       | ✗         | ✗       | ✗
FE310          |  ✗     | ✗          | ✓       | ✓         | ✗       | ✗
GD32V          |  ✗     | ✗          | ✗       | ✓ (1)     | ✗       | ✗
Kinetis        |  ✗     | ✗          | ✓       | ✓         | ✗       | ✗
LPC1768        |  ✗     | ✗          | ✓       | ✓         | ✗       | ✗
LPC23xx        |  ✗     | ✗          | ✗       | ✗         | ✗       | ✗
MIPS32r2       |  ✗     | ✗          | ✓       | ✗         | ✗       | ✗
MSP430         |  ✗     | ✗          | ✗       | ✗         | ✗       | ✗
nRF51          |  ✗     | ✗          | ✓       | ✓         | ✓       | ✗
nRF52          |  ✓     | ✗          | ✓       | ✓         | ✓       | ✓
RP2040         |  ✗     | ✗          | ✓       | ✓         | ✗       | ✗
SAM            |  ✗     | ✗          | ✓       | ✓         | ✗       | ✗
Stellaris      |  ✗     | ✗          | ✓       | ✓         | ✗       | ✗
STM32          |  ✓     | ✓          | ✓       | ✓         | ✗       | ✗

Remarks:

1. Requires a patched version of the programmer tool

Specialized Flashing Tools Per Platform     {#flashing-supported-tools-special}
---------------------------------------

The following list only contains single-platform flashing tools. Tools that
support multiple platforms are given in section
\ref flashing-supported-tools-generic above.

### AVR

- `avrdude`

### CC13xx / CC26xx

- `uniflash`

### CC2538

- `cc2538-bsl`

### ESP8266 / ESP32 (Xtensa) / ESP32 (RISC-V)

- `esptool`

### LPC23xx

- `lpc2k_pgm`

### MSP430

- `mspdebug`
- `goodfet`

### nRF52

- `adafruit-nrfutil`, `uf2conv` (requires Adafruit bootloader), see @ref boards_common_adafruit-nrf52-bootloader
- `nrfutil` (required nRF bootloader)
- `nrfjprog` (requires a separate J-Link debugger)

### RP2040

- `picotool`

### SAM

- `bossa`
- `edbg`

### STM32

- `stm32flash`
- `stm32loader`
- `cpy2remed` (requires integrated ST-Link programmer, e.g. Nucleo boards)
- `robotis-loader` (requires robotis bootloader, only one board supported)


Programmer Configuration                              {#flashing-configuration}
========================

This section will list additional configuration options to control the behavior
of a programming tool, such as selecting the hardware adapter used for
programming.

OpenOCD Configuration                         {#flashing-configuration-openocd}
---------------------

### OPENOCD_DEBUG_ADAPTER

`OPENOCD_DEBUG_ADAPTER` can be set via command line or as environment variable
to use non-default flashing hardware.

### OPENOCD_RESET_USE_CONNECT_ASSERT_SRST

`OPENOCD_RESET_USE_CONNECT_ASSERT_SRST` can be set via command line or as
environment variable to `0` to disable resetting the board via the `SRST` line.
This is useful when the `SRST` signal is not connected to the debug adapter or
when using cheap ST-Link V2 clones with broken `SRST` output. Note that it may
not be possible to attach the debugger while the MCU is in deep sleeping mode.
If this is set to `0` by the user, the user may need a carefully timed reset
button press to be able to flash the board.

### OPENOCD_PRE_FLASH_CMDS

`OPENOCD_PRE_FLASH_CMDS` can be set as environment variable to pass additional
commands to OpenOCD prior to flashing, e.g. to disable flash write protection.

### OPENOCD_PRE_VERIFY_CMDS

`OPENOCD_PRE_VERIFY_CMDS` can be set as environment variable to pass additional
flags to OpenOCD prior to verifying the flashed firmware. E.g. this is used
in the `pba-d-01-kw2x` to disable the watchdog to prevent it from disrupting
the verification process.

### OPENOCD_PRE_FLASH_CHECK_SCRIPT

`OPENOCD_PRE_FLASH_CHECK_SCRIPT` can be set via command line or as
environment variable to execute a script before OpenOCD starts flashing. It is
used for Kinetis boards to prevent bricking a board by locking the flash via
magic value in the flash configuration field protection bits.

The script is expected to exit with code `0` if flashing should resume, or with
exit code `1` if flashing should be aborted.

### OPENOCD_CONFIG

`OPENOCD_DEBUG_ADAPTER` can be set via command line or as environment variable
to use non-default OpenOCD configuration file.

### OPENOCD_TRANSPORT

`OPENOCD_TRANSPORT` can be set via command line or as environment variable to
select a non-default transport protocol. E.g. to use JTAG rather than SWD for a
board that defaults to SWD use:

```
make PROGRAMMER=openocd OPENOCD_TRANSPORT=jtag
```

Note that OpenOCD configuration file of a given board may only support SWD or
JTAG. Also JTAG requires more signal lines to be connected compared to SWD and
some internal programmers only have the SWD signal lines connected, so that
JTAG will not be possible.

stm32flash Configuration                  {#flashing-configuration-stm32flash}
------------------------

It is possible to automatically boot the STM32 board into the in-ROM bootloader
that `stm32flash` communicates with for flashing by connecting the RST pin to
DTR and the BOOT pin (or BOOT0 for STM32 MCU families with BOOT0 and BOOT1 pins)
to RTS of the TTL adapter. In addition, set `STM32FLASH_RESET` to `1` via
environment or command line to actually issue a reset with BOOT (or BOOT0)
pulled high prior flashing to enter the bootloader, and a second reset with BOOT
(or BOOT0) pulled low to reboot into the application. `STM32FLASH_RESET`
defaults to `0` as of know, as with `PROGRAMMER=stm32flash STM32FLASH_RESET=1`
additional terminal flags are set, so that `make term` doesn't accidentally
keeps the reset signal pulled low or boot the board into the bootloader.

The TTL adapter this was tested with had inverted RTS and DTR signal. By setting
`STM32FLASH_RESET_INVERT` to `1` RIOT will assume RTS and DTR signals to be
inverted, by setting it to `0` non-inverted signals will be generated. As of
now, `STM32FLASH_RESET_INVERT` is by default `1`. This may change if it
becomes evident that non-inverted TTL adapters are in fact more common than
inverted adapters.

MSPDEBUG Configuration                         {#flashing-configuration-mspdebug}
----------------------

All options can be passed as environment variables or as make arguments.
All options except for `DEBUGSERVER_PORT` apply to both debugging and flashing
alike.

`MSPDEBUG_PROGRAMMER` is used to set the hardware programmer/debugger to use
for programming and debugging. See `mspdebug --help` or `man mspdebug` for a
list of programmers.

`MSPDEBUG_PROTOCOL` is used to specify the debugger protocol. It is typically
set by the board used. Only JTAG and Spi-Bi-Wire are supported.

`MSPDEBUG_TTY` is used to connect via TTY interface instead of directly via
USB to the debugger. Usually, this is not required.

`DEBUG_ADAPTER_ID` is used to select the debugger/programmer by its serial. If
not set, `mspdebug` will select the first device with matching vendor and
product ID. Unless multiple debuggers of the same type are connected, this
options is typically not needed.

`DEBUGSERVER_PORT` is used to specify the TCP port to listen for GDB to
connect to. It defaults to 2000.

Handling Multiple Boards With UDEV-Rules                {#multiple-boards-udev}
========================================

When developing and working with multiple boards the default `PORT`
configuration for a particular board might not apply anymore so `PORT` will need
to be specified whenever calling `make term/test`. This can also happen if
multiple `DEBUGGERS/PROGRAMMERS` are present so `DEBUG_ADAPTER_ID` will also
need to be passed. Keeping track of this will become annoying.

One way of handling this is to use `udev` rules to define `SYMLINKS` between the
boards serial port (`riot/tty-<board-name>`) and the actual serial port
(dev/ttyACM* or other). With this we can query the rest of the boards serial
`dev` information (`DEBUG_ADAPTER_ID`, `PORT`, etc.) to always flash and open a
terminal on the correct port.

Procedure:

- use `udevadm info /dev/ttyACM0` to query the udev database for information on
  device on port `/dev/ttyACM0`.

  or use `udevadm info --attribute-walk --name /dev/ttyACM0` for more detailed
  output when the first level of information isn't enough

- create a udev rule with information of the device and one parent to create a
  matching rule in `/etc/udev/rules.d/70-riotboards.rules`.

~~~~~~~~~~~~~~~~~~~
    # samr21-xpro
    SUBSYSTEM=="tty", SUBSYSTEMS=="usb", ATTRS{idVendor}=="03eb", \
    ATTRS{idProduct}=="2111", ATTRS{manufacturer}=="Atmel Corp.", \
    ATTRS{serial}=="ATML2127031800004957", SYMLINK+="riot/tty-samr21-xpro"
~~~~~~~~~~~~~~~~~~~

- reload rules: `udevadm control --reload-rules`

- Boards `PORT` are symlinked to /dev/riot/tty-`board-name`.

- Create a `makefile.pre` that will query the real `PORT` and the
  `DEBUG_ADAPTER_ID` from the `SYMLINK` info

~~~~~~~~~~~~~~~~~~~
    PORT = /dev/riot/tty-$(BOARD)
    DEBUG_ADAPTER_ID = $(\
        shell udevadm info -q property $(PORT) |\
        sed -n ’/ID_SERIAL_SHORT/ {s/ID_SERIAL_SHORT=//p}’)
~~~~~~~~~~~~~~~~~~~

- You can now add `makefile.pre` to `RIOT_MAKEFILES_GLOBAL_PRE` as an environment
  variable or on each `make` call:

~~~~~~~~~~~~~~~~~~~
    $ RIOT_MAKEFILES_GLOBAL_PRE=/path/to/makefile.pre make -C examples/basic/hello-world flash term
~~~~~~~~~~~~~~~~~~~

@note if set as an environment variable it would be a good idea to add a
      variable to enable/disable it, e.g:

~~~~~~~~~~~~~~~~~~~
ifeq (1,$(ENABLE_LOCAL_BOARDS))
    PORT = /dev/riot/tty-$(BOARD)
    DEBUG_ADAPTER_ID = $(\
        shell udevadm info -q property $(PORT) |\
        sed -n ’/ID_SERIAL_SHORT/ {s/ID_SERIAL_SHORT=//p}’)
endif
~~~~~~~~~~~~~~~~~~~

Handling Multiple Versions of the Same BOARD
--------------------------------------------

The above procedure works fine when handling different boards, but not
multiple times the same board, e.g: multiple `samr21-xpro`.

An option for this would be to add an identifier of that board to the mapped
`riot/tty-*`, there are multiple ways of handling this but in the end it means
having a way to identify every copy.

Another way would be to map the `DEBUG_ADAPTER_ID` in the name:

~~~~~~~~~~~~~~~~~~~
SYMLINK+="riot/node-$attr{serial}
~~~~~~~~~~~~~~~~~~~

But it will require to know in advance the serial number of each board you want
to use. Another option would be to add some kind of numbering and defining
multiple symlinks for each board. e.g. for `samr21-xpro` number `n`:

~~~~~~~~~~~~~~~~~~~
    # samr21-xpro
    SUBSYSTEM=="tty", SUBSYSTEMS=="usb", ATTRS{idVendor}=="03eb", \
    ATTRS{idProduct}=="2111", ATTRS{manufacturer}=="Atmel Corp.", \
    ATTRS{serial}=="ATML2127031800004957", SYMLINK+="riot/tty-samr21-xpro", \
    SYMLINK+="riot/tty-samr21-xpro-n"
~~~~~~~~~~~~~~~~~~~

Then, when flashing, the number can be specified and the parsing adapted:

~~~~~~~~~~~~~~~~~~~
    ifneq(,$(BOARD_NUM))
      PORT = /dev/riot/tty-$(BOARD)-$(BOARD_NUM)
    else
      PORT = /dev/riot/tty-$(BOARD)
    endif
    DEBUG_ADAPTER_ID = $(\
      shell udevadm info -q property $(PORT) |\
      sed -n ’/ID_SERIAL_SHORT/ {s/ID_SERIAL_SHORT=//p}’)
~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~
    BOARD=samr21-xpro BOARD_NUM=n make flash term
~~~~~~~~~~~~~~~~~~~

In the end, this would be the same as using the serial, but a simple number might
be easier to handle.

Notes
-----
Udev only parses SUBSYSTEM and one parent. For others, we will rely on ENV
variables defined by 60-serial.rules

So the current filename should be higher than 60-serial.rules

If for some reason re-writing the serial is needed there is a windows tool:
  https://remoteqth.com/wiki/index.php?page=How+to+set+usb+device+SerialNumber


Documentation:
--------------
* The whole documentation
  http://reactivated.net/writing_udev_rules.html#udevinfo
* Udev manpage
  http://manpages.ubuntu.com/manpages/eoan/en/man7/udev.7.html

Handling Multiple Boards Without UDEV-Rules           {#multiple-boards-no-udev}
===========================================

This is a simpler approach to the above mentioned issue. The solution here only
uses a makefile script for selecting the debugger and serial port. No
administrative privileges (e.g. to configure Udev) are required.

One of the limitations of the solution described here is that it currently
doesn't work with multiple boards of the same type. This limitation is a
limitation of the script and not of the mechanism used, it is possible to adapt
the script to support multiple boards of the same type. This modification is
left as an exercise to the reader.

The following Make snippet is used:

~~~~~~~~~~~~~~~~~~~
    LOCAL_BOARD_MAP ?= 1

    # Adapt this list to your board collection
    SERIAL_nucleo-f103rb ?= 066BFF343633464257254156
    SERIAL_same54-xpro ?= ATML2748051800005053
    SERIAL_samr21-xpro ?= ATML2127031800008360
    SERIAL_nrf52dk ?= 000682223007

    ifeq (1,$(LOCAL_BOARD_MAP))

      # Retrieve the serial of the selected board
      BOARD_SERIAL = $(SERIAL_$(BOARD))

      # Check if there is a serial for the board
      ifneq (,$(BOARD_SERIAL))

        # Set the variables used by various debug tools to the selected serial
        SERIAL ?= $(BOARD_SERIAL)
        DEBUG_ADAPTER_ID ?= $(BOARD_SERIAL)
        JLINK_SERIAL ?= $(BOARD_SERIAL)

        # Use the existing script to grab the matching /dev/ttyACM* device
        PORT ?= $(shell $(RIOTTOOLS)/usb-serial/ttys.py --most-recent --format path --serial $(SERIAL))
      endif
    endif
~~~~~~~~~~~~~~~~~~~

The array of board serial numbers has to be edited to match your local boards.
The serial numbers used here is the USB device serial number as reported by
the debugger hardware. With the `make list-ttys` it is reported as the 'serial':

~~~~~~~~~~~~~~~~~~~
$ make list-ttys
path         | driver  | vendor                   | model                                | model_db              | serial                   | ctime
-------------|---------|--------------------------|--------------------------------------|-----------------------|--------------------------|---------
/dev/ttyUSB0 | cp210x  | Silicon Labs             | CP2102 USB to UART Bridge Controller | CP210x UART Bridge    | 0001                     | 15:58:13
/dev/ttyACM1 | cdc_acm | STMicroelectronics       | STM32 STLink                         | ST-LINK/V2.1          | 0671FF535155878281151932 | 15:58:04
/dev/ttyACM3 | cdc_acm | Arduino (www.arduino.cc) | EOS High Power                       | Mega ADK R3 (CDC ACM) | 75230313733351110120     | 15:59:57
/dev/ttyACM2 | cdc_acm | SEGGER                   | J-Link                               | J-Link                | 000683475134             | 12:41:36
~~~~~~~~~~~~~~~~~~~

When the above make snippet is included as `RIOT_MAKEFILES_GLOBAL_PRE`, the
serial number of the USB device is automatically set if the used board is
included in the script. This will then ensure that the board debugger is used
for flashing and the board serial device is used when starting the serial
console.

It supports command line parameters to filter by vendor name, model name, serial
number, or driver. In addition, the `--most-recent` argument will only print the
most recently added interface (out of those matching the filtering by vendor,
model, etc.). The `--format path` argument will result in only the device path
being printed for convenient use in scripts.

Handling Multiple Boards: Simplest Approach            {#multiple-boards-simple}
===========================================

Passing `MOST_RECENT_PORT=1` as environment variable or as parameter to
make will result in the most recently connected board being preferred over the
default PORT for the selected board.

For some boards `TTY_BOARD_FILTER` is provided, which filters TTYs e.g. by
vendor or model to only considered TTYs that actually may belong to the selected
board. E.g. for Nucleo boards this is `--model 'STM32 STLink'`, as they all use
an integrated STLink as programmer. As long as only one TTY is provided from an
STLink, this will reliably select the correct TTY for an Nucleo regardless of
which TTY was most recently connected. Some boards even provide info that
allows to always reliably identify them correctly (e.g. the firmware on the
ATmega16U2 used as USB to UART converted on Arduino Mega2560 will provide
identification data unique to that board).

Adding Board Filters
--------------------

After connecting as many variants of the board you target (and maybe some others
to test that the filter actually filters out non-matching boards). Then first
run `./dist/tools/usb-serial/ttys.py` without arguments and study the output.
When a genuine Arduino Mega 2560, a genuine Arduino Mega ADK (a variant of the
Mega 2560),a cheap Arduino Mega 2560 clone, a BBC micro:bit v2, and a
Nucleo F767-ZI are connected, the following output is shown:

path         | driver  | vendor                   | model                                | model_db                                             | serial                                           | ctime    | iface_num
-------------|---------|--------------------------|--------------------------------------|------------------------------------------------------|--------------------------------------------------|----------|----------
/dev/ttyACM0 | cdc_acm | Arduino (www.arduino.cc) | 0042                                 | Mega 2560 R3 (CDC ACM)                               | 857353134333519002C1                             | 12:13:55 | 0
/dev/ttyACM1 | cdc_acm | Arduino (www.arduino.cc) | EOS High Power                       | Mega ADK R3 (CDC ACM)                                | 75230313733351110120                             | 15:59:57 | 0
/dev/ttyACM2 | cdc_acm | STMicroelectronics       | STM32 STLink                         | ST-LINK/V2.1                                         | 0670FF535155878281123912                         | 10:00:39 | 2
/dev/ttyACM3 | cdc_acm | Arm                      | BBC micro:bit CMSIS-DAP              | ARM mbed                                             | 99053602000528334c41b84da1f2f09d000000006e052820 | 12:21:03 | 1
/dev/ttyUSB0 | cp210x  | Silicon Labs             | CP2102 USB to UART Bridge Controller | CP2102/CP2109 UART Bridge Controller [CP210x family] | 0001                                             | 16:57:27 | 0

Now we add arguments to the invocation of `ttys.py` to filter the list e.g.
by model, vendor etc. (note: as regex!) ideally until only the target boards
are listed. Some boards do not provide enough information to e.g. tell them
apart from other boards using the same USB to UART bridge or the same debugger.
In that case we have to live with some "bycatch".

In the case of the Arduino Mega 2560 the parameters
`--vendor 'Arduino' --model-db 'Mega 2560|Mega ADK'` will narrow down the
list to only show the genuine Arduino Mega versions. Se we add to the
`Makefile.include` in `boards/arduino-mega2560`:

```Makefile
TTY_BOARD_FILTER := --vendor 'Arduino' --model-db 'Mega 2560|Mega ADK'
```

Note that also matching the `R3` in `Mega 2560 R3` would prevent matching older
or newer revisions than R3, so we don't add that to the regex.

Advances Board Filters
----------------------

In most cases, just adding a simple `TTY_BOARD_FILTER` is sufficient. If we
however have wildly different flavors of the same board (e.g. genuine Arduino
Mega 2560 with an ATmega16U2 and clones with a cheap USB to UART bridge) that we
all want to support, we have to instead provide a `TTY_SELECT_CMD` that prints
the path to and the serial of the TTY (separated by a space) and exists with
`0` if a TTY was found, or that exists with `1` and prints nothing when no TTY
was found. We can still use the `ttys.py` script to detect all Arduino Mega
2560 versions: We first try to detect a genuine Arduino Mega and fall back to
selecting cheap USB UART bridges when that fails using the `||` shell operator:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  TTY_SELECT_CMD := $(RIOTTOOLS)/usb-serial/ttys.py \
                    --most-recent \
                    --format path serial \
                    --vendor 'Arduino' \
                    --model-db 'Mega 2560|Mega ADK' || \
                    $(RIOTTOOLS)/usb-serial/ttys.py \
                    --most-recent \
                    --format path serial \
                    --driver 'cp210x'
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
