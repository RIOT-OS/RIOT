# OpenWSN on RIOT

This port of OpenWSN to RIOT is based on current OpenWSN upstream providing a
BSP with RIOT's interfaces. Currently supported are iotlab_m3 and fox. More
boards will follow through improvements in netdev radio driver interface.

# Usage

A test can be found in tests/openwsn providing a shell command to initialise
as root or listening node. And providing a sample Makefile.

Build using
```Bash
$> export BOARD=iotlab_m3
$> export PORT=/dev/ttyTHEPORTOFYOURIOTLAB
$> make -B clean flash term
```

To use OpenWSN with RIOT it has to be added to the used packages variable
```Makefile
USEPKG += openwsn
```

On the first build the archive will be fetched, patched and built.
**WARNING** A call of `make clean` also cleans the OpenWSN tree right now so
changes to the source code will be lost in the next build.
