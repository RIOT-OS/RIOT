LWMAC test application
======================
This application is a showcase for testing LWMAC communications. Using it
for your board, you should be able to interactively use any hardware
that is supported for communications among devices based on LWMAC.

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
`PORT=/dev/yourport` variable can be used to override this.


Example output
==============

The `ifconfig` command will help you to configure all available network
interfaces. On an samr21 board it will print something like:
```
2015-09-16 16:58:37,762 - INFO # ifconfig
2015-09-16 16:58:37,766 - INFO # Iface  4   HWaddr: 9e:72  Channel: 26  NID: 0x23  TX-Power: 0dBm  State: IDLE CSMA Retries: 4
2015-09-16 16:58:37,768 - INFO #            Long HWaddr: 36:32:48:33:46:da:9e:72
2015-09-16 16:58:37,769 - INFO #            AUTOACK  CSMA
2015-09-16 16:58:37,770 - INFO #            Source address length: 2
```

The `txtsnd` command allows you to send a simple string directly over the link
layer (here, it is LWMAC) using unicast or broadcast. The application will also automatically print
information about any received packet over the serial. This will look like:
```
2015-09-16 16:59:29,187 - INFO # PKTDUMP: data received:
2015-09-16 16:59:29,189 - INFO # ~~ SNIP  0 - size:  28 byte, type:
NETTYPE_UNDEF (0)
2015-09-16 16:59:29,190 - INFO # 000000 7b 3b 3a 02 85 00 e7 fb 00 00 00 00 01
02 5a 55
2015-09-16 16:59:29,192 - INFO # 000010 40 42 3e 62 f2 1a 00 00 00 00 00 00
2015-09-16 16:59:29,194 - INFO # ~~ SNIP  1 - size:  18 byte, type:
NETTYPE_NETIF (-1)
2015-09-16 16:59:29,195 - INFO # if_pid: 4  rssi: 49  lqi: 78
2015-09-16 16:59:29,196 - INFO # src_l2addr: 5a:55:40:42:3e:62:f2:1a
2015-09-16 16:59:29,197 - INFO # dst_l2addr: ff:ff
2015-09-16 16:59:29,198 - INFO # ~~ PKT    -  2 snips, total size:  46 byte
```
