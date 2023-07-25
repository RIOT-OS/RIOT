Peripheral STM32 FMC Test Application
=====================================

This application tests basic STM32 FMC functionality:

- 8-bit, 16-bit and 32-bit write and read access
- availability of the whole memory

Configuration
-------------

The bank to be used is defined by environment variable `FMC_BANK`. For example,
if the board configures two banks with RAM, the second bank can be tested by
specifying the `FMC_BANK` variable as follows:
```
FMC_BANK=1 BOARD=... make -j8 -C tests/periph/fmc flash test
```

Expected Output on Success
--------------------------

    main(): This is RIOT! (Version: <INSERT VERSION HERE>)
    ...
    [SUCCESS]
