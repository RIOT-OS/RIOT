# EFM32 CPU Information

## Introduction

Every EFM32 family has a `efm32-info.mk` file in `cpu/efm32/families`, which
contains one entry per supported CPU model. The memory map from that database
ends up in the linker script, and the peripheral flags decide which features
the CPU provides.

The database is generated separately from the vendor headers of the Gecko SDK
package, so the two can drift apart. A wrong entry is not noticed until an
application fails at runtime.

This test application compares the database against the vendor headers of the
CPU that is being built for, and against the addresses that the linker actually
used.

## Test cases

The following entries are covered:

* The series, against `_SILICON_LABS_32B_SERIES`.
* The flash and SRAM base addresses and sizes, against `FLASH_BASE`,
  `FLASH_SIZE`, `SRAM_BASE` and `SRAM_SIZE`.
* The cryptographic accelerator, the true random number generator and the
  radio, against the peripheral counts of the vendor headers.

Only the CPU models that have a board in the tree can be checked this way,
which is a small subset of the database.

## Expected result

The test application compiles for EFM32-based boards. A mismatch between the
database and the vendor headers is reported as a compile error.

When run, the application reports the memory map, and verifies that the linker
placed the firmware in the flash that the database describes, and that the
linker used the described SRAM.

The test application ends with `[SUCCESS]`.
