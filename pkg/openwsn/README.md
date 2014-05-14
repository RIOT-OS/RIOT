# OpenWSN on RIOT

This port of OpenWSN to RIOT is based on release RB-1.4 with backported support
for the iot-lab_M3 board from the develop branch.

# Compatibility

This port of the openwsn stack is compatible with the iot-lab_M3 board only for now.
The TelosB board support was temporarily dropped but will return soon again.
The hardware dependency will be reduced in the future and thous running on more
hardware platforms.

# Usage

A test can be found in the [projects repository](https://github.com/RIOT-OS/projects/)
named ```test_openwsn_pkg``` with an example ```Makefile```.

Build using
```Bash
$> export BOARD=iot-lab_M3
$> export PORT=/dev/ttyTHEPORTOFYOURIOTLAB
$> make -B clean flash
```

To use OpenWSN with RIOT it has to be added to the used packages variable
```Makefile
USEPKG += openwsn
```

On the first build the archive will be fetched, patched and built.
**WARNING** A call of `make clean` also cleans the OpenWSN tree right now so
changes to the source code will be lost in the next build.
