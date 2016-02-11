Dynamic Linking for RIOT
================

Sample module for dynamic linking in RIOT.

Introduction
=================

The aim of this project is to enable dynamic linking in RIOT, which will allow files to be loaded on the fly.
The motivation behind this is to reduce unnecessary coupling between modules which results in increase in size
of the overall binary. Dynamic linking is becoming a more and more useful for IOT based systems now since it can
easily allow software updates to be sent and used at runtime without the need to turn off the running system.

Current approach
===================
The current approach aims to parse an elf which is placed in-memory at a known location. The code is compiled
position independent (-fPIC) which allows it to be placed anywhere. Once we have the binary, then it can be easily
relocated via the relocation table entries which help to fix the incorrect error reference in .got/.got.plt section or
any such other section. The requirements for the current implementation to work is that a .text,.strtab/.dynstr should
be present. A symtab or dynsym section should also be present in the code. The code should be compiled with -fPIC -shared
flags. The relocations which are listed in the relocation tables are appropriately performed. There are some challenges
which were encountered during the duration of the project which have been briefly described in the later sections.

Challenges faced
==================
There were two major challenges that needed a different workaround which are mentioned here-
* The normal sequence of instruction which results for the generation of a function call in PIC code is as follows:
  1)A branch instruction which points to plt entry corresponding to the function.
  2)The instructions in PLT entry go to the address present in the .got.plt entry (usually resolver), the resolver routine
    is called which finds the address of the missing file.
  3)The .got.plt entry is fixed and the corresponding function is called.

The way we approach this is by relocating the .got.plt entries before calling the functions which ensures that instead of
calling the resolver routine it calls the function directly. However, there is a issue with the .plt entry for a symbol.
(See here https://sourceware.org/bugzilla/show_bug.cgi?id=16017). This bug results in unsupported instructions being generated
for CortexM0/CortexM0+ (like movw,movt) which cause hard faults. There are two workarounds for this issue-

1)Changing the instructions in .plt section (adding support from platforms which do not support thumb-2).However for this the size
of the instructions should be same as the current size.

2)Another approach (that we tried) was to change the address of the bl instruction to directly point to the function. This can be
easily done by having a look at the instruction reference manual and figuring out the bits where the address is stored in the instruction.
This approach has an added benefit of saving reducing the number of instructions to be reduced. However the big drawback for this approach is
that the instructions are pc relative and hence there is a fixed offset more than which jumps can't be made. This causes the core functions not
being used currently since the jump address offset exceeds the current range. However internal function calls are nicely supported.

Support for external functions can be added by adding extra map file like structure (relocations for which can again be done dynamically.
However this will be continued in the future for a better implementation (apart from this project)

Boards Used
==============
The current code has been tried on samr21-xpro (and some parts on arduino-due). Arduino-Due being a cortex-m3 supports the plt instruction set. However the lack of debugging support for it made it difficult to continue on it further.

