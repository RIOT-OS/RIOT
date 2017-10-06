Overivew
========
This tool can be used for automated generation of interrupt vector tables for
Cortex-M based MCUs. For this the tool parses a list of CMSIS vendor headers,
parses the interrupt line information, and compacts this information for all
CPUs of the targeted family into a single vector table.

Usage
=====
To use this tool, you simply call it with the path to the CPU families
`cpu_conf.h` file as argument. E.g.
```bash
$ dist/tools/isrvecgen/isrvecgen.py cpu/stm32f4/include/cpu_conf.h
```

The tool then outputs the vector table as well as the weak function definitions
of the interrupt functions to STDIO. As of now, you have to manually copy+paste
the output in the corresponding `vectors.c` file.
