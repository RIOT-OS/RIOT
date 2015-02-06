Tests transceiver module with nativenet through netapi
=========================================================

This test is equivalent to `tests/nativenet` in all but the fact that it uses
the netapi transceiver adapter `TRANSCEIVER_NETAPI` instead of
`TRANSCEIVER_NATIVENET`.

To build the sender it *needs* to be build first. So to test this use

```bash
make clean sender
make all test
```
