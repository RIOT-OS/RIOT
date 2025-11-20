# DTLS sock async test application

This is a test application for DTLS sock `sock_dtls_t` with `sock_async`.

## Testing using RIOT `native`

For testing, we can use two RIOT `native` RIOT instances. For that first we
need to prepare the network interfaces:

```bash
$ ./../../dist/tools/tapsetup/tapsetup --create 2
```

For the server instance:

```
$ PORT=tap0 make all term
[...]
> dtlss start
ifconfig
```

For the client:

```
$ PORT=tap1 make all term
[...]
> dtlsc <server ip address> "DATA to send"
```

## Debug logs

To enable debug logs uncomment `CFLAGS += -DCONFIG_DTLS_DEBUG` in the Makefile.
Tinydtls supports setting the log level. See Makefile for more info.

## Configs and constraints

DTLS sock acts as a wrapper for the underlying DTLS stack and as such, the
constraints that applies specifically to the stack are also applied here.
For tinydtls, please refer to [dtls-echo README][1].

[1]: https://github.com/RIOT-OS/RIOT/blob/master/examples/networking/dtls/dtls-echo/README.md
