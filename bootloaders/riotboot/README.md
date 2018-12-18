# Overview
This folder contains a simple bootloader called "riotboot".
A header with metadata of length `RIOTBOOT_HDR_LEN` precedes
the RIOT firmware. The header contains "RIOT" as a magic
number to recognise a RIOT firmware image, a checksum, and
the version of the RIOT firmware `APP_VER`.
This bootloader verifies the checksum of the header which is located
at an offset (`ROM_OFFSET`) with respect to  the `ROM_START_ADDR`
defined by the CPU, just after the space allocated for riotboot.

riotboot consists of:

  - This application which serves as minimal bootloader,
  - the module "riotboot_hdr" used to recognise RIOT firmware which riotboot
    can boot,
  - the module "riotboot_slot" used to manage the partitions (slots) with a
    RIOT header attached to them,
  - a tool in dist/tools/riotboot_gen_hdr for header generation,
  - several make targets to glue everything together.

## Concept
`riotboot` expects the flash to be formatted in slots: at CPU_FLASH_BASE
address resides the bootloader, which is followed by a slot 0 with a
RIOT firmware.

A RIOT firmware in a single slot is composed by:

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

The bootloader will, on reset, verify the checksum of the first slot header,
then boot it. If the slot doesn't have a valid checksum, no image will be
booted and the bootloader will enter `while(1);` endless loop.

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

# Usage
Just compile your application using the target `riotboot`. The header
is generated automatically according to your `APP_VER`, which can be
optionally set (0 by default) in your makefile.

## Flashing
The image can be flashed using `riotboot/flash` which also flashes
the bootloader.

e.g. `BOARD=samr21-xpro FEATURES_REQUIRED+=riotboot APP_VER=$(date +%s) make -C examples/hello-world riotboot/flash`

The command compiles both the hello-world example and riotboot,
generates the header and attaches it at the beginning of the example
binary.

A comprehensive test is available at tests/riotboot.
