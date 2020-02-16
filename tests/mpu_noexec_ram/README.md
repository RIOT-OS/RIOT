# mpu_noexec_ram

Tests for the `mpu_noexec_ram` pseudomodule. As this module is currently
only supported on Cortex M devices, the test case is a bit ARM specific.

## Output

With `USEMODULE += mpu_noexec_ram` in `Makefile` this application should
execute a kernel panic from the `MEM MANAGE HANDLER`. Without this
pseudomodule activated, it will run into a hard fault.
