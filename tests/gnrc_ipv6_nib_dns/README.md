# Overview

This test utilizes [scapy] to test GNRC's RDNSS option implementation for
router advertisements

To test, compile and flash the application to any board of your liking (since
`ethos` is used to communicate with non-native boards it really doesn't matter
as long as the application fits and the board is supported).

    make flash

And run the tests using

```
sudo make test
```

Note that root privileges are required since `scapy` needs to construct Ethernet
frames to properly communicate over the TAP interface.

The test passes on success, on failure an exception is shown.

[scapy]: https://scapy.readthedocs.io/en/latest/
