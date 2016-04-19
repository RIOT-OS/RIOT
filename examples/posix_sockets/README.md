examples/posix_sockets
======================
This application is a showcase for RIOT's POSIX socket support. To
keep things simple this application has only one-hop support and
no routing capabilities.

Usage
=====

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
`PORT=/dev/yourport` (not to be confused with the UDP port) variable can
be used to override this.


Example output
==============

The shell commands come with online help. Call `help` to see which commands
exist and what they do.


udp send fe80::1 1337 uiaeue
2015-09-22 14:55:30,686 - INFO # > udp send fe80::1 1337 uiaeue
2015-09-22 14:55:30,690 - INFO # Success: send 6 byte to fe80::1:1337

Running the `help` command on an iotlab-m3:
```
2015-09-22 14:54:54,442 - INFO # help
2015-09-22 14:54:54,443 - INFO # Command              Description
2015-09-22 14:54:54,444 - INFO # ---------------------------------------
2015-09-22 14:54:54,446 - INFO # udp                  send data over UDP and listen on UDP ports
2015-09-22 14:54:54,447 - INFO # reboot               Reboot the node
2015-09-22 14:54:54,449 - INFO # ps                   Prints information about running threads.
2015-09-22 14:54:54,451 - INFO # mersenne_init        initializes the PRNG
2015-09-22 14:54:54,453 - INFO # mersenne_get         returns 32 bit of pseudo randomness
2015-09-22 14:54:54,454 - INFO # ifconfig             Configure network interfaces
2015-09-22 14:54:54,455 - INFO # txtsnd               Sends a custom string as is over the link layer
2015-09-22 14:54:54,457 - INFO # ncache               manage neighbor cache by hand
2015-09-22 14:54:54,459 - INFO # routers              IPv6 default router list
```

Running the `ps` command on an iotlab-m3:

```
2015-09-22 14:54:57,134 - INFO # > ps
2015-09-22 14:54:57,139 - INFO # 	pid | name                 | state    Q | pri | stack ( used) | location
2015-09-22 14:54:57,143 - INFO # 	  1 | idle                 | pending  Q |  15 |   256 (  136) | 0x200001cc
2015-09-22 14:54:57,157 - INFO # 	  2 | main                 | pending  Q |   7 |  1536 (  620) | 0x200002cc
2015-09-22 14:54:57,164 - INFO # 	  3 | 6lo                  | bl rx    _ |   3 |  1024 (  404) | 0x20003ef8
2015-09-22 14:54:57,169 - INFO # 	  4 | ipv6                 | bl rx    _ |   4 |  1024 (  436) | 0x20001cc0
2015-09-22 14:54:57,172 - INFO # 	  5 | udp                  | bl rx    _ |   5 |  1024 (  268) | 0x20004660
2015-09-22 14:54:57,177 - INFO # 	  6 | at86rfxx             | bl rx    _ |   3 |  1024 (  320) | 0x20001888
2015-09-22 14:54:57,183 - INFO # 	    | SUM                  |            |     |  5888 ( 2184)
```

Start a UDP server with `udp server start <udp_port>`:

```
2015-09-22 14:55:09,563 - INFO # > udp server start 1337
2015-09-22 14:55:09,564 - INFO # Success: started UDP server on port 1337
```

Send a UDP package with `udp send <dst_addr> <dst_port> <data>`:

```
2015-09-22 14:55:30,686 - INFO # > udp send fe80::3432:4833:46d4:9e06 1337 test
2015-09-22 14:55:30,690 - INFO # Success: send 4 byte to [fe80::3432:4833:46d4:9e06]:1337
```

You can get the IPv6 address of the destination by using the `ifconfig` command on the receiver:

```
2015-09-22 14:58:10,394 - INFO # ifconfig
2015-09-22 14:58:10,397 - INFO # Iface  6   HWaddr: 9e:06  Channel: 26  NID: 0x23  TX-Power: 0dBm  State: IDLE CSMA Retries: 4
2015-09-22 14:58:10,399 - INFO #            Long HWaddr: 36:32:48:33:46:d4:9e:06
2015-09-22 14:58:10,400 - INFO #            AUTOACK  CSMA  MTU:1280  6LO  IPHC
2015-09-22 14:58:10,402 - INFO #            Source address length: 8
2015-09-22 14:58:10,404 - INFO #            Link type: wireless
2015-09-22 14:58:10,407 - INFO #            inet6 addr: ff02::1/128  scope: local [multicast]
2015-09-22 14:58:10,415 - INFO #            inet6 addr: fe80::3432:4833:46d4:9e06/64  scope: local
2015-09-22 14:58:10,416 - INFO #
```
