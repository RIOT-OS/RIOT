## Lua interactive interpreter

### About

This example shows how to run a [Lua](https://www.lua.org/) Read-Eval-Print loop.
It works in a similar way to the lua shell that comes with your operating
system's default lua installation.


### How to run

Type `make all flash` to program your board. The lua interpreter communicates
via UART (like the shell).

It is not recommended to use the default RIOT terminal from `make term` because it
up the input and output and the REPL needs multi-line input. Instead, use something
like `miniterm` from `pyserial` by setting the `RIOT_TERMINAL` environment
variable:

```
RIOT_TERMINAL=miniterm BOARD=... make term
```

By default only some of the builtin modules are loaded, to preserve RAM. See
the definition of `BARE_MINIMUM_MODS` in main.c.

### Using the interpreter

See the [Lua manual](https://www.lua.org/manual/5.3/) for the syntax of the language.

Each piece of single or multi-line input is compiled as a chunk and run. For this
reason, issuing "local" definitions may not work as expected: the definitions
will be local to that chunk only.
