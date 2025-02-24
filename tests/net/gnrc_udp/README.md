UDP test
========
This test is largely based on the [`gnrc_networking`][1] example but uses a
modified `udp` command: Instead of providing data in a string format a number of
bytes is given. A packet of that size is then send, its content consisting of an
incrementing repeating byte.
The server is also modified as it outputs the number of currently received
packets when a new packet is received instead of the content of the new packet.
This counter can be reset using `udp reset`

[1]: https://github.com/RIOT-OS/RIOT/tree/master/examples/networking/gnrc/gnrc_networking
