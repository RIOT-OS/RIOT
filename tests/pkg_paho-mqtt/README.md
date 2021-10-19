# Overview

This is a test application for the Eclipse Paho MQTT client. The accompanying
test script in `tests-as-root` checks for basic functionality of the MQTT
functions (connect, subscribe, publish, unsubscribe and disconnect).

To run the tests, setup of a tap interface is required. This can be done using
`dist/tools/tapsetup/tapsetup` or by running:

```
sudo ip tuntap add tap0 mode tap user ${USER}
sudo ip link set tap0 up
```

The tests can then be run with:

```
BOARD=<your choice> NETWORK_STACK=<your choice> make flash test-as-root
```

The supported network stack options are `lwip` and `gnrc`.
