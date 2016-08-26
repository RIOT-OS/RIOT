examples/udp_echo_server
========================
This application showcases a simple UDP echo server using RIOT's connectivity
API `conn`.

It listens on port 61616 for incoming UDP packets and returns them to the
original sender.

Usage
=====

Setup
-----
Build, flash and start the application:
```
export BOARD=your_board
make
make flash
make term
```

The `term` make target starts a terminal emulator for your board. It
connects to a default port so you can interact with the shell, usually
that is `/dev/ttyUSB0`. If your port is named differently, the
`PORT=/dev/yourport` variable can be used to override this.


Sending a packet to the server
------------------------------

The `ifconfig` command will help you to configure and review all available
network configuration. On an `samr21-xpro` it will print something like the
following. The global IPv6 address is available if you set-up a [border
router][6lbr]:

```
2016-06-08 17:29:57,780 - INFO # ifconfig
2016-06-08 17:29:57,785 - INFO # Iface  7   HWaddr: 76:0e  Channel: 26  Page: 0
NID: 0x23
2016-06-08 17:29:57,791 - INFO #            Long HWaddr: 5a:5a:5c:5c:87:92:76:0e
2016-06-08 17:29:57,796 - INFO #            TX-Power: 0dBm  State: IDLE  max.  Retrans.: 3  CSMA Retries: 4
2016-06-08 17:29:57,802 - INFO #            ACK_REQ  CSMA  MTU:1280  HL:64  6LO  IPHC
2016-06-08 17:29:57,805 - INFO #            Source address length: 8
2016-06-08 17:29:57,816 - INFO #            Link type: wireless
2016-06-08 17:29:57,819 - INFO #            inet6 addr: ff02::1/128  scope: local [multicast]
2016-06-08 17:29:57,821 - INFO #            inet6 addr: fe80::585a:5c5c:8792:760e/64  scope: local
2016-06-08 17:29:57,825 - INFO #            inet6 addr: ff02::1:ff92:760e/128 scope: local [multicast]
2016-06-08 17:29:57,830 - INFO #            inet6 addr: ff02::1a/128  scope: local [multicast]
2016-06-08 17:29:57,821 - INFO #            inet6 addr: 2001:db8::585a:5c5c:8792:760e/64  scope: global
```

If you now send a UDP message to one of the IPv6 addresses given with port 61616,
the node will print out this information and return a UDP packet to the sender:

```
2016-06-08 17:34:22,417 - INFO # Message "Hello!" receive from [fd00:dead:beef::1]:TODO
```

To send a UDP message using e.g. [netcat](http://nc110.sourceforge.net/)
through a [border router][6lbr] or when using the `native` `BOARD`:

```bash
echo "Hello\!" | nc -6 -u 2001:db8::585a:5c5c:8792:760e 61616
```

Note however, that some versions of `netcat` do not have IPv6 support and might
yield error messages. Because of that, we recommend the BSD version of `netcat`.

Alternatively you can send with e.g. the [`gnrc_networking`
example][gnrc_networking]:

```
udp send fe80::585a:5c5c:8792:760e 61616 "Hello!"
2016-06-08 17:46:09,116 - INFO # > udp send fe80::585a:5c5c:8792:760e 61616 "Hello!"
2016-06-08 17:46:09,124 - INFO # Success: send 6 byte to [fe80::585a:5c5c:8792:760e]:61616
```

  [6lbr] ../gnrc_border_router/README.md
  [gnrc_networking] ../gnrc_networking/README.md
