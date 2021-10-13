# Regression test for forwarding IPv6 packets with a sniffing subscriber

This only tests if a router can sniff its own forwarded IPv6 traffic. It is
**not** a full IPv6 test suite.

## Usage

```
BOARD='<your choice>' make flash test
```
