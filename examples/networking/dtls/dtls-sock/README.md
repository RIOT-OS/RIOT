# DTLS sock example

This example shows how to use DTLS sock `sock_dtls_t`.

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

[1]: https://github.com/RIOT-OS/RIOT/blob/master/examples/dtls-echo/README.md

### Cipher suite
You can select between `TLS_PSK_WITH_AES_128_CCM_8` (Pre-Shared Key) and
`TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8` (Raw Public Key - ECC) either by setting
the correspondent options in `make menuconfig` or via `CFLAGS`. See the `Makefile`
for more information.

## Credentials
### Pre-Shared Key
When using Pre-Shared Key (PSK), the client registers two keys to `credman` and
makes them available to the sock. The client registers a PSK callback function,
which allows the application to specify which credential to use with a
particular sock and endpoint, depending on the hint sent by the server. As per
the sock DTLS documentation, if the application callback fails to determine
which credential should be used, an Identity Hint
(https://tools.ietf.org/html/rfc4279#section-5.2) match is attempted.
`credential1` is assigned an Identity Hint, defined as `PSK_DEFAULT_HINT` in
`tinydtls_keys.h`. This hint is used by sock to select the credential, in case
the DTLS server sends such a hint. Finally, if none of the above attempts
succeed in determining which credential to use, sock DTLS will pick the first
valid credential registered in the sock.

The behaviour above can be tested, for example, by removing the hint from the
server (`sock_dtls_set_server_psk_id_hint`). As `psk_credential_0` is the first
registered credential in the client, it will be chosen. As the server does not
have this credential, the handshake will fail.

### ECC
When using ECC Raw Public Key (RPK), the server registers two private keys to
`credman` and makes them available to the sock. It also registers an RPK
callback function, which allows the application to specify which credential to
use. The used credential can be changed at runtime by calling `dtlss ecc <0|1>`.
As the client also knows both public keys from the server, both options will
work.
