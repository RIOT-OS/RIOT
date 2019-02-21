# Overview
This folder contains a simple bootloader called "riotboot".
A header with metadata of length `RIOTBOOT_HDR_LEN` precedes
the RIOT firmware. The header contains "RIOT" as a magic
number to recognise a RIOT firmware image, a checksum, and
the version of the RIOT firmware `APP_VER`.
This bootloader verifies the checksum of the header which is located
at an offset (`ROM_OFFSET`) with respect to  the `ROM_START_ADDR`
defined by the CPU, just after the space allocated for riotboot.

In case of multiple firmware slots, the bootloader iterates through
valid headers and boots the newest image.

riotboot consists of:

  - This application which serves as minimal bootloader,
  - the module "riotboot_hdr" used to recognise RIOT firmware which riotboot
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
A board capable to use riotboot must meet the following requirements:

  - Embed a Cortex-M0+/3/4/7 processor
  - Provide the variables `ROM_START_ADDR` and `ROM_LEN`
  - Use cpu/cortexm_common/ldscripts/cortexm.ld ld script
  - Pass the cortexm_common_ldscript test in tests/
  - Being able to execute startup code at least twice (`board_init()`)
  - Declare `FEATURES_PROVIDED += riotboot` to pull the rigth dependencies
  - Being able to flash binary files, if integration with the build
    system is required for flashing

The above requirements are usually met if the board succeeds to execute
the riotboot test in tests/.

# Single Slot
Just compile your application using the target `riotboot`. The header
is generated automatically according to your `APP_VER`, which can be
optionally set (0 by default) in your makefile.


## Flashing example
The image can be flashed using `riotboot/flash` which also flashes
the bootloader. Below a concrete example:

`BOARD=samr21-xpro FEATURES_REQUIRED+=riotboot APP_VER=$(date +%s) make -C examples/hello-world flash-combined-slot0`

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
  - `riotboot/flash-slot1`: builds and flash a firmware for slot 1;
  - `riotboot/flash-extended-slot0` builds + flashes slot 0 and erases (zeroes)
    the metadata of slot 1 (invalidating it);
  - `riotboot` builds both slot 0 and 1.

In particular, if one wants to be sure to boot a particular image, using the
target `riotboot/flash-extended-slot0` is the way to go (resulting in only
slot 0 being valid, thus being booted).

## Flashing examples

The following sequence of commands tests building, flashing and booting slot 0,
then slot 1. tests/riotboot prints out the current booted slot in the shell.

To test building, flashing and booting the first slot:

`BOARD=samr21-xpro APP_VER=$(date +%s) make -C tests/riotboot/ riotboot/flash-combined-slot0 test`

For the second slot:

`BOARD=samr21-xpro APP_VER=$(date +%s) make -C tests/riotboot/ riotboot/flash-slot1 test`
