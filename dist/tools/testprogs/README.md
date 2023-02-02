Test Programs
=============

This folder contains a collection of test programs for use by the build system,
e.g. to check for compiler features.

`minimal_linkable.c`
--------------------

This is a minimal C program that is expected to compile and link with all
supported versions of GCC and newlib.

Note: We currently even link with GCC when compilation is done with LLVM. It is
expected to compile and link fine with LLVM as well, but may require more some
additional flags to find the embedded C lib to link against.
