# dtls_wolfssl example

This example shows how to use DTLS with wolfSSL

## SOCK vs. Socket

This example is configured to use socks instead of sockets (over GNRC).
It's possible to use sockets, which give a more similar approach to the
UNIX version of wolfSSL.

## Fast configuration (Between RIOT instances)

### Prepare the bridge interface linking two tuntap

```bash
    ./../../dist/tools/tapsetup/tapsetup --create 2
```

## Testing

### Run the server
```bash
$ make all; PORT=tap1 make term
> ifconfig
```
*copy the server address*

```bash
> dtlss
```
### Run the client
```bash
$ PORT=tap0 make term
> dtlsc <IPv6's server address[%netif]>
```
### Testing against host endpoints

Riot-to-host can be tested against the DTLS examples provided in the [wolfSSL-examples](https://github.com/wolfSSL/wolfssl-examples/tree/master/dtls) repository.

## Boards

Boards that do not support the `../gnrc_networking` example are included
in the `BOARD_INSUFFICIENT_MEMORY`, plus the board `cc2650stk`.
