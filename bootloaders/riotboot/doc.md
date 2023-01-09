/**
@defgroup    bootloader_riotboot riotboot
@ingroup     bootloaders

# Overview
This folder contains a simple bootloader called "riotboot".
A header with metadata of length `RIOTBOOT_HDR_LEN` precedes
the RIOT firmware. The header contains "RIOT" as a magic
number to recognize a RIOT firmware image, a checksum, and
the version of the RIOT firmware `APP_VER`.
This bootloader verifies the checksum of the header which is located
at an offset (`ROM_OFFSET`) with respect to  the `ROM_START_ADDR`
defined by the CPU, just after the space allocated for riotboot.

In case of multiple firmware slots, the bootloader iterates through
valid headers and boots the newest image.

riotboot consists of:

  - This application which serves as minimal bootloader,
  - the module "riotboot_hdr" used to recognize RIOT firmware which riotboot
    can boot,
  - the module "riotboot_slot" used to manage the partitions (slots) with a
    RIOT header attached to them,
  - a tool in dist/tools/riotboot_gen_hdr for header generation,
  - several make targets to glue everything together.

## Concept
`riotboot` expects the flash to be formatted in slots: at the `CPU_FLASH_BASE`
address resides the bootloader, which is followed by a slot 0 containing a
RIOT firmware image. If present, a second firmware image (in slot 1) starts just
afterwards.

The bootloader and a RIOT firmware in slot 0 are depicted below:

```
|------------------------------- FLASH -------------------------------------|
|----- RIOTBOOT_LEN ----|----------- RIOTBOOT_SLOT_SIZE (slot 0) -----------|
                        |----- RIOTBOOT_HDR_LEN ------|
 ---------------------------------------------------------------------------
|        riotboot       | riotboot_hdr_t + filler (0) |   RIOT firmware     |
 ---------------------------------------------------------------------------
```

Please note that `RIOTBOOT_HDR_LEN` depends on the architecture of the
MCU, since it needs to be aligned to 256B. This is fixed regardless of
`sizeof(riotboot_hdr_t)`

Also note that, if no slot is available with a valid checksum,
no image will be booted and the bootloader will enter `while(1);` endless loop.

# Requirements
Try to compile and run tests/riotboot:

    $ BOARD=<board> make -C tests/riotboot flash test

If the test succeeds, your board is supported. Else you can try to port
riotboot to your board (see the below porting guide).

When building the bootloader, the global define `RIOTBOOT` is available. You
can use this define to skip certain parts in `board_init()` (or `cpu_init()`)
that should not be executed during boot. Note that this define is different
from `MODULE_RIOTBOOT`, which is also defined when building an application that
utilizes riotboot.

# Single Slot
Just compile your application with `FEATURES_REQUIRED += riotboot`. The header
is generated automatically according to your `APP_VER`, which can be optionally
set (current system time in seconds since 1970 (epoch) by default) in your
makefile.

## Flashing example
If your application is using the riotboot feature, the usual targets (`all`,
`flash`, `flash-only`) will automatically compile and/or flash both the
bootloader and slot0, while ensuring that slot 1 is invalidated so slot 0 will
be booted.

The image can also be flashed using `riotboot/flash` which also flashes
the bootloader. Below a concrete example:

`BOARD=samr21-xpro FEATURES_REQUIRED+=riotboot APP_VER=$(date +%s) make -C examples/hello-world riotboot/flash-combined-slot0`

The above compiles a hello world binary and a bootloader, then flashes the
combined binary comprising of: bootloader + slot 0 header + slot 0 image.
If booted, the device will execute the Hello-World image.

A comprehensive test is available at tests/riotboot (also see below).

# Multi-Slot

When several slots are available, the bootloader iterates through
valid headers and boots the newest image (which has the greater `VERSION`)

Dedicated make targets are available to build and flash several slots:

- `riotboot/slot1`: Builds a firmware in ELF and binary format with
  an offset at the end of slot 0;
- `riotboot/flash-slot1`: builds and flashes a firmware for slot 1;
- `riotboot/flash-extended-slot0` builds + flashes slot 0 and erases (zeroes)
  the metadata of slot 1 (invalidating it);
- `riotboot` builds both slot 0 and 1.

In particular, if one wants to be sure to boot a particular image, using the
target `riotboot/flash-extended-slot0` is the way to go (resulting in only
slot 0 being valid, thus being booted). This is done automatically by `make
flash` if the `riotboot` feature is used.

## Testing riotboot

See <a href="https://github.com/RIOT-OS/RIOT/blob/master/tests/riotboot/README.md">tests/riotboot/README.md</a>.

# Quick riotboot porting guide

Your board/cpu is not supported yet? Try to port riotboot!

## Porting to a board based on an Arm Cortex-M0+/3/4/7/22/33 MCU

Extending riotboot to support another board with a Cortex-M0+/3/4/7/22/33
microcontroller is rather straightforward. You need to:

- Provide the variables `ROM_START_ADDR` and `ROM_LEN` as well as
`RAM_LEN` and `RAM_START_ADDR`.
- Adapt the linker scripts for your cpu to pass the test
[tests/cortexm_common_ldscript](../../tests/cortexm_common_ldscript). This test
ensures that the linker script supports building firmware's with a rom offset
and specific sized firmware's.
- Make the startup code `board_init()`, `cpu_init` idempotent, i.e. whether you
execute it once, or twice, it works all the same. The global define `RIOTBOOT`
can be useful here. (e.g. [cpu/efm32/cpu.c](../../cpu/efm32/cpu.c))
- Make sure (and adapt if needed) that the flasher script can:
    - flash a `.bin`
    - flash at a specified offset
    - flash without performing mass erase, only erase flash sections to be written
  e.g.: [makefiles/tools/edbg.inc.mk](../../makefiles/tools/edbg.inc.mk)
- Declare `FEATURES_PROVIDED += riotboot` for the target `BOARD`.
- Make sure that `RIOTBOOT_LEN` size is such that the remainder of the flash
can be divided by the number slots while staying `FLASHPAGE_SIZE` aligned.
e.g: [cpu/nrf52/Makefile.include](../../cpu/nrf52/Makefile.include)
- Check other specific `BOARD`/`CPU` flash alignment requirements (e.g.:
kinetis vector table must be naturally aligned to the power of two, see
[cpu/kinetis/Makefile.include](../../cpu/kinetis/Makefile.include))

## Porting to a board based on other types of MCUs

For other MCU architectures the following extra requirements must be fulfilled:

- Provide the functions defined in the header
[sys/include/riotboot/slot.h](../../sys/include/riotboot/slot.h), in particular
`riotboot_slot_jump(unsigned slot)` which will allow jumping from the bootloader
to another slot/application.
- Adapt the linker script so that it supports building firmware's with a rom
offset and a defined firmware size. To get an idea look at
[cpu/cortexm_common/Makefile.include](../../cpu/cortexm_common/Makefile.include)

Additional remarks:

- If your are in Big-Endian, you may need to further adapt some part of the code.

*/
