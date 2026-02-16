### About

This example shows how to write IoT applications using Forth.

### How to extend

In the function `ficlCompilePlatform` registers all C defined words
with `dictAppendWord`.

### How to use

Put your Forth code into "main.f" (check the example). The file will
be included in your application as an ASCII byte array (see BLOBS in the
Makefile).

The script will then be run immediately after RIOT has started up.

### How to run

Type `make flash term`
