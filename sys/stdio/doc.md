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
or @ref sys_stdio_rtt. All available options are shown as modules in the
list below:

| Module                  | Description                                              |
|:----------------------- |:-------------------------------------------------------- |
| `stdio_cdc_acm`         | USB CDC ACM STDIO, see @ref usbus_cdc_acm_stdio          |
| `stdio_ethos`           | Ethernet-over-serial STDIO (legacy, see `ethos`)         |
| `stdio_fb`              | Framebuffer STDIO                                        |
| `stdio_native`          | Native board STDIO, forwarded to the host terminal       |
| `stdio_nimble`          | Bluetooth LE STDIO on top of NimBLE                      |
| `stdio_null`            | Discards all output, no input support                    |
| `stdio_rtt`             | Segger RTT STDIO, see @ref sys_stdio_rtt                 |
| `stdio_semihosting`     | ARM/RISC-V semihosting STDIO                             |
| `stdio_slipdev`         | SLIP STDIO                                               |
| `stdio_telnet`          | Telnet STDIO                                             |
| `stdio_tinyusb_cdc_acm` | USB CDC ACM STDIO on top of the tinyUSB stack            |
| `stdio_uart`            | UART STDIO, see @ref sys_stdio_uart                      |
| `stdio_udp`             | UDP STDIO                                                |
| `stdio_usb_serial_jtag` | USB Serial/JTAG STDIO (e.g. on some ESP32 variants)      |

As with the additional features, you can specify the STDIO backend to be used
in your application Makefile:

```Makefile
USEMODULE += stdio_cdc_acm
```

@note   More than one backend can be selected at the same time. In that
        case, `stdio_dispatch` is pulled in automatically. It forwards
        `stdio_write()` to every selected backend and merges the input of
        all of them into a single stream. A maximum of 9 backends can be
        selected at the same time.

@note   Not selecting any of the above backends will result in `stdio_uart`
        being selected as a fallback, so that a usable STDIO is always
        available. This happens in a two-stage dependency selection mechanism
        where first `stdio_default` is selected, and if no other backend is
        selected in the second round of dependency resolution, `stdio_uart` is
        selected as a fallback. Refer to `makefiles/stdio.inc.mk` for the
        details.

## Additional features

### stdio_available

The pseudomodule `stdio_available` exists to mark that the selected STDIO
backend supports @ref stdio_available, the function used to query how many
bytes are currently buffered for reading.
