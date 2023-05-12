# `gnrc_dhcpv6_client_6lbr` test

This test utilizes [scapy] to test the DHCPv6 client configuration for a 6LoWPAN
border router.

To test, compile and flash the application to any board of your liking (since
`ethos` is used to communicate with non-native boards it really doesn't matter
as long as the application fits).

```
make flash
```

And run the tests using

```
sudo make test-as-root
```

Note that root privileges are required since `scapy` needs to construct Ethernet
frames to properly communicate over the TAP interface.

The tests succeeds if you see the string `SUCCESS`.

If any problems are encountered (i.e. if the test prints the string `FAILED`),
set the echo parameter in the `run()` function at the bottom of the test script
(tests-as-root/01-run.py) to `True`. The test script will then offer a more detailed
output.

[scapy]: https://scapy.readthedocs.io/en/latest/
