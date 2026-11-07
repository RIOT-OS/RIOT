## Forth/Ficl interactive interpreter

### About

This example shows how to run a [Ficl](https://ficl.sourceforge.net/index.html) Read-Eval-Print loop.

### How to run

Type `make all flash` to program your board. The ficl interpreter communicates
via UART (like the shell).

It is not recommended to use `make term` because the default RIOT terminal messes
up the input. Instead, use a real terminal like gtkterm or picocom.

### Using the interpreter

See the [forth tutorial](https://learnxinyminutes.com/forth/) for the syntax of the language.

The word `led0` controls the internal LED0 on the board.
