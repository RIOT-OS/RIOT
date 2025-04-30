@defgroup       sys_stdio STDIO abstraction
@ingroup        sys

@brief          Simple standard input/output (STDIO) abstraction for RIOT

# Abstract

STDIO in RIOT is split into two parts: An interface to the stdio transports
consisting mainly of @ref stdio_read and @ref stdio_write provided by RIOT,
and the standard C stdio functions (such as `printf()`, `puts()`, `scanf()`)
provided by the standard C library.

## Configuring STDIO

While the standard features for input and output are enabled by default, some
additional features have to be enabled explicitly due to higher memory
consumption. This includes the following features:

| Module               | Features                                      |
|:-------------------- |:--------------------------------------------- |
| `printf_float`       | Support for printing floats/doubles           |
| `printf_long_long`   | Support for printing (unsigned) long long     |
| `stdin`              | Support for input (default is output only)    |

The additional features can be enabled in the application Makefile:
```Makefile
USEMODULE += printf_float
```

@note   Modules that depend on a certain stdio feature will depend on that. E.g.
        when using the @ref sys_shell module, the module `stdin` is
        automatically used, as it is a dependency.

## Enable STDIO Backends

The various transports supported by RIOT are enabled by selecting the
corresponding modules, such as @ref sys_stdio_uart, @ref usbus_cdc_acm_stdio,
or @ref sys_stdio_rtt. All available options can are shown as mdoules in the
list below.

As with the additional features, you can specify the STDIO backend to be used in your application Makefile:

```Makefile
USEMODULE += stdio_cdc_acm
```
