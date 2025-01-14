examples/default
================
This application is a showcase for RIOT's hardware support. Using it
for your board, you should be able to interactively use any hardware
that is supported.

To do this, the application uses the `shell` and `shell_cmds_default`
modules and all the driver modules each board supports.

`shell` is a very simple interactive command interpreter that can be
used to call functions.  Many of RIOT's modules define some generic
shell commands. These are included via the `shell_cmds_default` module.

Additionally, the `ps` module which provides the `ps` shell command is
included.

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

The shell commands come with online help. Call `help` to see which commands
exist and what they do.

Running the `help` command on an iotlab-m3:
```
2015-09-16 16:57:17,723 - INFO # help
2015-09-16 16:57:17,725 - INFO # Command              Description
2015-09-16 16:57:17,726 - INFO # ---------------------------------------
2015-09-16 16:57:17,727 - INFO # reboot               Reboot the node
2015-09-16 16:57:17,729 - INFO # ps                   Prints information about running threads.
2015-09-16 16:57:17,731 - INFO # isl29020_init        Initializes the isl29020 sensor driver.
2015-09-16 16:57:17,733 - INFO # isl29020_read        Prints data from the isl29020 sensor.
2015-09-16 16:57:17,735 - INFO # lps331ap_init        Initializes the lps331ap sensor driver.
2015-09-16 16:57:17,737 - INFO # lps331ap_read        Prints data from the lps331ap sensor.
2015-09-16 16:57:17,739 - INFO # l3g4200d_init        Initializes the l3g4200d sensor driver.
2015-09-16 16:57:17,740 - INFO # l3g4200d_read        Prints data from the l3g4200d sensor.
2015-09-16 16:57:17,742 - INFO # lsm303dlhc_init      Initializes the lsm303dlhc sensor driver.
2015-09-16 16:57:17,744 - INFO # lsm303dlhc_read      Prints data from the lsm303dlhc sensor.
2015-09-16 16:57:17,746 - INFO # ifconfig             Configure network interfaces
2015-09-16 16:57:17,747 - INFO # txtsnd               Sends a custom string as is over the link layer
```

Running the `ps` command on an iotlab-m3:

```
2015-09-16 16:57:57,634 - INFO # ps
2015-09-16 16:57:57,637 - INFO #    pid | name                 | state    Q | pri | stack ( used) | location
2015-09-16 16:57:57,640 - INFO #      1 | idle                 | pending  Q |  15 |   256 (  140) | 0x20000200
2015-09-16 16:57:57,642 - INFO #      2 | main                 | pending  Q |   7 |  1536 (  640) | 0x20000300
2015-09-16 16:57:57,645 - INFO #      3 | pktdump              | bl rx    _ |   6 |  1536 (  544) | 0x200025e0
2015-09-16 16:57:57,647 - INFO #      4 | at86rfxx             | bl rx    _ |   3 |  1024 (  360) | 0x2000099c
2015-09-16 16:57:57,649 - INFO #        | SUM                  |            |     |  4352 ( 1684)
```

RIOT specific
=============

The `ps` command is used to analyze the thread's state and memory
status.


Networking
==========

The `ifconfig` command will help you to configure all available network
interfaces. On an iolab-m3 it will print something like:
```
2015-09-16 16:58:37,762 - INFO # ifconfig
2015-09-16 16:58:37,766 - INFO # Iface  4   HWaddr: 9e:72  Channel: 26  NID: 0x23  TX-Power: 0dBm  State: IDLE CSMA Retries: 4
2015-09-16 16:58:37,768 - INFO #            Long HWaddr: 36:32:48:33:46:da:9e:72
2015-09-16 16:58:37,769 - INFO #            AUTOACK  CSMA
2015-09-16 16:58:37,770 - INFO #            Source address length: 2
```

Type `ifconfig help` to get an online help for all available options (e.g.
setting the radio channel via `ifconfig 4 set chan 12`).

The `txtsnd` command allows you to send a simple string directly over the link
layer using unicast or broadcast. The application will also automatically print
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
