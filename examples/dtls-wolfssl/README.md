# dtls_wolfssl example

This example shows how to use DTLS with wolfSSL

## SOCK vs. Socket

This example is configured to use socks instead of sockets (over GNRC).
It's possible to use POSIX sockets, which give a more similar approach to the
UNIX version of wolfSSL. POSIX sockets are supported by RIOT-OS via lwIP, but
no example is available at this time.

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

### Certificate/key

Test certificate and key arrays are provided in `cert.c`. You can generate your own arrays starting from existing certificate and key in .der format using `xxd -i`.

### Testing against host endpoints

Riot-to-host can be tested against the DTLS examples provided in the [wolfSSL-examples](https://github.com/wolfSSL/wolfssl-examples/tree/master/dtls) repository.

## Boards

Boards that due to insufficient memory are not able to support GNRC are included
in the `BOARD_INSUFFICIENT_MEMORY`.
