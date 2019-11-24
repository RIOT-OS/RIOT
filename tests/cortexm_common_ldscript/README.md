Cortexm-common ldscript
=======================

This test checks the support for building firmwares with a rom offset and
specific sized firmwares using `cortexm-common` ldscript.

When building `all`, the `compile-tests` target is also build and executes build
time compilation tests checking different firmwares configurations verified by
inspecting the result elf files.


Test output
-----------

For a board that does not have a `ROM_OFFSET` variable configured by default
(== a board without bootloader installed), it produces the following output
after the normal compilation:

```
make all BOARD=iotlab-m3
...
Test rom offset 1 byte overflow detection: [OK]
Test rom offset subtracted from rom length in elffile: [SKIP](Reason: board does not have a ROM_OFFSET configured)
Test compilation with offset 0x1000: [OK]
Test compilation with offset 0x2000: [OK]
Test compilation with half ROM length: [OK]
Test ROM overflow detection (too_big_for_rom): [OK]
Test ROM overflow detection (offset_and_romlen): [OK]
```

For a board that have a `ROM_OFFSET` variable configured by default (== a board
with bootloader installed), it produces the following output after the normal
compilation:

```
make BOARD=bluepill  PROGRAMMER=dfu-util
...
Test rom offset 1 byte overflow detection: [OK]
Test rom offset subtracted from rom length in elffile: [OK]
Test compilation with offset 0x1000: [OK]
Test compilation with offset 0x2000: [OK]
Test compilation with half ROM length: [OK]
Test ROM overflow detection (too_big_for_rom): [OK]
Test ROM overflow detection (offset_and_romlen): [OK]
```
