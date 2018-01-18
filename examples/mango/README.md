### About

This example shows how to use the [Mango Virtual Machine](https://github.com/ektrah/mango) with RIOT. The functions that mango should use to interact with RIOT must be known in the c file, which starts mango. In this case the Mango VM should turn a light on, when the value of a simulated sensor drops under 150. In `main.c` is the backend for the functions `i32 sysgetsensorvalue()` and `void syssetlight(i32)` defined. These are mapped to Mango system calls with value 100 and 101, as you can see in `program.mango`.

### How to adapt

Adapt/Replace the Mango assembler program `program.mango` to change the behaviour of the VM. The Mango assembler code must compiled to mango bytecode, before the VM can execute it. To do this, use the [Mango Compiler](https://github.com/ektrah/mango-compiler). To add more system functions like `syssetlight` declare them in your assembler file, and check for the corresponding system calls in `main.c`.
