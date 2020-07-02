examples/posix_sockets
======================
This application is a showcase for RIOT's POSIX select support. To
keep things simple this application has only one-hop support and
no routing capabilities.

Usage
=====

Build, flash and start the application:
```sh
export BOARD=your_board
make
make flash
make term
```

The `term` make target starts a terminal emulator for your board. It
connects to a default port so you can interact with the shell, usually
that is `/dev/ttyUSB0`. If your port is named differently, the
`PORT=/dev/yourport` (not to be confused with the UDP port) variable can
be used to override this.


Example output
==============

The application starts 4 UDP servers on a selection of different ports that wait
for input simultaneously:
```
2019-12-17 16:36:45,559 # RIOT select example application
2019-12-17 16:36:45,561 # Started UDP server at [fe80::14ac:fb65:106b:1115]:1350
2019-12-17 16:36:45,562 # Started UDP server at [fe80::14ac:fb65:106b:1115]:4973
2019-12-17 16:36:45,562 # Started UDP server at [fe80::14ac:fb65:106b:1115]:6717
2019-12-17 16:36:45,562 # Started UDP server at [fe80::14ac:fb65:106b:1115]:9673
```

If you do not see any output you might need to reset the node. Either, by
pressing the hardware reset button on the board or by running
```sh
make reset
```

There is no shell in this application. You can use the [`posix_sockets` example]
from another board to send a packet to the node:

```
> udp send fe80::14ac:fb65:106b:1115 6717 "Hello World!"
2019-12-17 16:47:01,789 # udp send fe80::14ac:fb65:106b:1115%6 6717 "Hello World!"
2019-12-17 16:47:01,795 # Success: send 12 byte to fe80::14ac:fb65:106b:1115:6717
```

On the board with the `posix_select` example you will see then something like
this:

```
2019-12-17 16:47:01,796 # Received data from [fe80::589d:9386:2208:6579]:192:
2019-12-17 16:47:01,796 # Hello World!
```

Alternatively, with `native` or if your host also can connect to the board, you
can also use [`netcat`][netcat] to send multiple packets simultaneously. E.g.
when the node is connected to the host via the interface `tapbr0`:

```sh
echo -ne "Hello World!" | nc -6u "fe80::78b9:ecff:fe96:8279%tapbr0" 4973 & \
    echo -ne "Hello Space!" | nc -6u "fe80::78b9:ecff:fe96:8279%tapbr0" 1350
killall nc
```

This is what the `native` node will then show:

```
Received data from [fe80::3ccc:8dff:fe9f:9991]:14279:
Hello World!

Received data from [fe80::3ccc:8dff:fe9f:9991]:58817:
Hello Space!

```

[`posix_sockets` example]: ../posix_sockets
[netcat]: https://www.unix.com/man-page/Linux/1/netcat/
