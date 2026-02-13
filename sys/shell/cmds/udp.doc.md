@defgroup   sys_shell_commands_udp UDP shell command
@ingroup    sys_shell_commands
@brief      Generic UDP shell command for any supported network stack
@{

```
> udp [send|server]
```

This command can be used with any network stack in RIOT that offers a @ref net_sock_udp
interface. Examples include @ref net_gnrc and @ref pkg_lwip.

# UDP Client

Module. Add `USEMODULE += shell_cmd_udp` to your application Makefile.

It is automatically selected with `USEMODULE += sys_shell_cmds_default`
if @ref net_sock_udp (`USEMODULE += sock_udp`) is available in your build.

## Send UDP Datagrams

```
> udp send [-b] <addr>:<port> <data> [<num> [<delay>]]
```

Send `<num>` UDP datagrams to `<addr>:<port>` at the interval specified in `<delay>`
(in microseconds).
If the optional parameter `-b` is given, `<data>` is interpreted as hexadecimal digits,
otherwise `<data>` is interpreted as a text string.

`<num>` and `<delay>` are only supported if @ref sys_ztimer is available
(e.g., via `USEMODULE += ztimer_usec` or `USEMODULE += ztimer_msec`).

Note that the shell buffer size may limit the input length (`SHELL_DEFAULT_BUFSIZE`
defaults to 128 bytes).


# UDP Server

Module. Add `USEMODULE += shell_cmd_udp_server` to your application Makefile.

## Start UDP Server

```
> udp server start <port>
```

Start a UDP server listening on `<port>` on any interface in a separate thread.

# Stop UDP server

```
> udp server stop
```

Stop running the UDP server that was started with `udp server start`.

@}
