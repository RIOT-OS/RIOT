# `gnrc_ipv6_ext` test

This test utilizes [scapy] to test the GNRC's RPL source routing header
handling.

To test, compile and flash the application to any board of your liking (since
`ethos` is used to communicate with non-native boards it really doesn't matter
as long as the application fits).

```
make flash
```

And run the tests using

```
sudo make test
```

Note that root privileges are required since `scapy` needs to construct Ethernet
frames to properly communicate over the TAP interface.

The tests succeeds if you see the string `SUCCESS`.

If any problems are encountered (i.e. if the test prints the string `FAILED`),
set the echo parameter in the `run()` function at the bottom of the test script
(tests/01-run.py) to `True`. The test script will then offer a more detailed
output.

It might be that due to `scapy`'s sniffer not picking up an expected packet
sometimes that the test application hangs for a while and then issues `FAILED`.
Just restart the test in that case.

[scapy]: https://scapy.readthedocs.io/en/latest/
