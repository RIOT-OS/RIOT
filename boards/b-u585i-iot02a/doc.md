@defgroup    boards_b-u585i-iot02a STM32 B-U585I-IOT02A
@ingroup     boards
@brief       Support for the STM32 B-U585I-IOT02A

## Flashing the device

The ST B-U585I-IOT02A board includes an on-board ST-LINK programmer and can be
flashed using OpenOCD.
@note The latest release (v0.11) of OpenOCD doesn't contain yet support for this board,
so the source code version from upstream master must be cloned and built:

```shell
git clone https://git.code.sf.net/p/openocd/code openocd
cd openocd
./bootstrap
./configure --prefix=<installation directory>
make -j
sudo make install
```

Once the patched OpenOCD is built and installed, you can flash the board simply
by typing

```shell
BOARD=b-u585i-iot02a make flash
```

and debug via GDB by simply typing

```shell
BOARD=b-u585i-iot02a make debug
```

**Important notice:** With GCC 10, the `tests/sys/malloc` application doesn't work.
To use malloc, prefer GCC 9 which seems to work better.
