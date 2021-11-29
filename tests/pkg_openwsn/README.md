# OpenWSN on RIOT

This test demonstrates the [OpenWSN](https://github.com/openwsn-berkeley/openwsn-fw) full
stack (UDP, IPv6, RPL, 6TiSCH) running on RIOT. When flashed, it will initialize the stack
and provide the user with some minimal shell commands to:

- print the own IPv6 address `ifconfig`
- change the UDP destination port `udp server start <port>`
- send a UDP packet `udp send <addr> <port> <data>`

A tun interface can also be set to allow traffic out of the network. This
allows pinging nodes in the network from the host.

Please note that this port is still in experimental status. For further information
about the port refer to the [pkg documentation](../../pkg/openwsn/doc.txt).

## Experimental setups

The following setup act as a starting point for testing and debugging. Either
build and flash local nodes or incorporate the [FIT IoT-LAB](https://www.iot-lab.info/)
Testbed. Please check the ports [documentation](../../pkg/openwsn/doc.txt) for information
about supported hardware platforms.

This port currently needs `openvisualizer`, please make sure you follow the
[pre-requisites](../../dist/tools/openvisualizer/makefile.openvisualizer.inc.mk)
to install a patched version of `openvisualizer`.

An OpenWSN root node acts as a border router, it tunnels IEEE80215.4.E
between `openvisualizer` and the network without looking at the frame content.
This means that a root node will not be reachable when 'pinging' from the host
or if trying to send udp packets to it.

Two test cases are described with different requirements:

1. From host [ping](#Communicating-with-host-(IPV6)) nodes in network over ipv6:
```
    - 1 root node + tun interface
    - +1 leaf node
```

2. Sending [UDP](#Communicating-over-udp) packets between nodes:
```
    - 1 root node
    - +2 leaf node
```

### Synchronization

There are 3 things that can be done to speed up and help nodes synchronization,
all related to how often a node will receive an EB (enhanced beacon).

    - disable channel hopping
    - increase EB_PORTION
    - reduce SLOTFRAME_LENGTH

If you want to disable channel hopping to speed up synchronization you can set
a fix channel.

    $ export CFLAGS=-DIEEE802154E_SINGLE_CHANNEL=17

Enhanced beacon are sent once per slotframe with a likelihood of 1/EB_PORTION.
The default value is EB_PORTION = 10 (10%), reducing the value of EB_PORTION
increases the frequency at which beacons are sent, this will also help nodes
to stay synchronized.

    $ export CFLAGS=-DEB_PORTION=4

EB gets an opportunity to be sent (depending on 1/EB_PORTION) once in every
slotframe, reducing the soltframe length will cause EB to be sent more often.
Beware that this could have an impact on the MSF (Minimal Scheduling Function).

    $ export CFLAGS=-DSLOTFRAME_LENGTH=51

See [documentation](../../pkg/openwsn/doc.txt#Synchronization) for more on
synchronization.

## IMPORTANT!

OpenWSN uses source routing and this means all network traffic must go through
from the root node to OpenVisualizer. If the root node configuration can't
handle the configured baudrate correctly this will lead to packet loss.

Currently these are the tested configurations:

(a) samr21-xpro network:
    - leaf nodes using `openwsn_sctimer_rtt`
    - root node using `openwsn_sctimer_rtt` and 19200 baudrate directly
      connected to UART pins (not through the usb debugger)

(b) iotlab-m3 network:
    - leaf nodes using `openwsn_sctimer_rtt` or `sctimer_ztimer`
    - root node using `openwsn_sctimer_rtt` and 57600 baudrate

For more details on this please refer to [pkg documentation](../../pkg/openwsn/doc.txt).

## Testing configuration (a) (three local samr21-xpro nodes)

1. figure out each node's serial number (`edbg -l` output might help)
2. hook up the root node to an external USB-serial-converter. For that, connect
   the converter's RXD/TXD pins to the node's PA22/PA23.
3. flash the root node:

        $ SERIAL=${ROOT_SERIAL_NODE} OPENSERIAL_BAUD=19200 USEMODULE=openwsn_serial \
              BOARD=samr21-xpro make flash -j4

4. flash the leaf nodes:

        $ BOARD=samr21-xpro make all -j4
        $ BOARD=samr21-xpro SERIAL=${LEAF_SERIAL_NODE0} make flash-only
        $ BOARD=samr21-xpro SERIAL=${LEAF_SERIAL_NODE1} make flash-only

5. open a shell to the leaf nodes
   so in two shell windows, do (one in each):

        $ BOARD=samr21-xpro SERIAL=${LEAF_SERIAL_NODE0} make term
        $ BOARD=samr21-xpro SERIAL=${LEAF_SERIAL_NODE1} make term

6. in a third shell, launch openvisualizer:

        $ BOARD=samr21-xpro PORT=<USB-serial-port, e.g., /dev/ttyUSB0> BAUD=19200 \
              make openv-termroot

## Testing configuration (b) (iotlab-m3 network on iotlab)

When using OpenVisualizer over iot-lab an ssh-tunnel is opened to connect to the
IoT-LAB motes' TCP port. For this to work you will need:

- A valid IoT-LAB [account](https://www.iot-lab.info/testbed/signup)
- Authenticate locally to IoT-LAB `$ iotlab-auth -u <login>`

1. Launch an experiment booking 3+ `iotlab-m3` nodes:

        $ iotlab-experiment submit -d 120 -l 3,archi=m3:at86rf231+site=saclay
        $ iotlab-experiment wait
        $ iotlab-experiment get --nodes

Since multiple nodes where configured for the experiment `IOTLAB_NODE` needs
to be specified for every node,  `IOTLAB_NODE=m3-%.saclay.iot-lab.info`

2. flash the root node

        $ IOTLAB_NODE=${ROOT_IOTLAB_NODE} USEMODULE=openwsn_serial \
          BOARD=iotlab-m3 make -C tests/pkg_openwsn flash

3. open a shell to the leaf nodes so in two shell windows, do (one in each):
        $ BOARD=iotlab-m3 make -C tests/pkg_openwsn all -j4
        $ BOARD=iotlab-m3 IOTLAB_NODE=${LEAF_IOTLAB_NODE0} make -C tests/pkg_openwsn flash-only
        $ BOARD=iotlab-m3 IOTLAB_NODE=${LEAF_IOTLAB_NODE1} make -C tests/pkg_openwsn flash-only

4. open a shell to the leaf nodes
   so in two shell windows, do (one in each):

        $ BOARD=iotlab-m3 IOTLAB_NODE=${LEAF_IOTLAB_NODE0} make -C tests/pkg_openwsn term
        $ BOARD=iotlab-m3 IOTLAB_NODE=${LEAF_IOTLAB_NODE1} make -C tests/pkg_openwsn term

5. in a third shell, launch openvisualizer:

        $ BOARD=iotlab-m3 IOTLAB_NODE=${ROOT_IOTLAB_NODE} make -C tests/pkg_openwsn openv-termroot

### Network Setup

If (a) and (b) where followed then on each lead node you should be able
to see the ipv6 address:

    main(): This is RIOT! (Version: 2020.04-devel-1649-g96fa9-pr_openwsn)
    OpenWSN UDP test
    You are running RIOT on a(n) iotlab-m3 board.
    This board features a(n) stm32f1 MCU.
    > ifconfig
    ifconfig
    Iface  3      HWaddr: 07:C2  NID: CA:FE

                  Long HWaddr: 96:35:9A:92:4E:3D:65:78
                  inet6 addr: fe80::9635:9a92:4e3d:6578

                  IEEE802154E sync: 0
                  6TiSCH joined: 0

                  NO RPL parent

Note that the '6TiSCH joined' field will only show up if `openwsn_cjoin`
is used.

On the root node Openvisualizer is launched and the DAGroot is setup.

    [OpenVisualizerServer:INFO] Extracting firmware definitions.
    [Utils:VERBOSE] Extracting firmware component names
    [Utils:VERBOSE] Extracting firmware log descriptions.
    [Utils:VERBOSE] Extracting 6top return codes.
    [Utils:VERBOSE] Extracting 6top states.
    [OpenVisualizerServer:INFO] Starting RPC server
    [OpenVisualizerServer:SUCCESS] Setting mote 43eb as root
    [ParserIEC:ERROR] 43eb [IEEE802154E] wrong state 1 in startSlot, at slotOffset 1
    [RPL:INFO] registering DAGroot 82-6b-de-ec-58-34-65-78

The root node will now start sending beacons and other nodes will synchronize, and
join. If channel hopping is enabled this can take quite some time (see
[Synchronization](../../pkg/openwsn/doc.txt#Synchronization). Once leaf nodes
have joined the network when issuing `ifconfig` you should see:

    ifconfig
    Iface  3      HWaddr: 0F:F4  NID: CA:FE

                Long HWaddr: 06:84:F6:65:10:6B:11:14
                inet6 addr: bbbb::684:f665:106b:1114

                IEEE802154E sync: 1
                6TiSCH joined: 1

                RPL rank: 2816
                RPL parent: 2A:BA:F7:65:10:6B:11:14
                RPL children:
                RPL DODAG ID: bbbb::2aba:f765:106b:1114


The root node should start receiving RPL DAOs:

    [RPL:INFO] received RPL DAO from bbbb:0:0:0:ab8:fc65:106b:1114
        - parents:
        bbbb:0:0:0:2aba:f765:106b:1114
        - children:
        bbbb:0:0:0:684:f665:106b:1114

    [RPL:INFO] received RPL DAO from bbbb:0:0:0:684:f665:106b:1114
        - parents:
        bbbb:0:0:0:2aba:f765:106b:1114
        - children:
        bbbb:0:0:0:ab8:fc65:106b:1114

    [RPL:INFO] received RPL DAO from bbbb:0:0:0:684:f665:106b:1114
        - parents:
        bbbb:0:0:0:2aba:f765:106b:1114
        - children:
        bbbb:0:0:0:ab8:fc65:106b:1114

Once DAOs for all nodes start being received the network is setup and you
should be able to send packets between nodes or ping from the host.

### Communicating over udp

On one node setup a udp-server:

    > ifconfig
    ifconfig
    Iface  3      HWaddr: 0F:F4  NID: CA:FE

                Long HWaddr: 06:84:F6:65:10:6B:11:14
                inet6 addr: bbbb::684:f665:106b:1114

                IEEE802154E sync: 1
                6TiSCH joined: 1

                RPL rank: 2816
                RPL parent: 2A:BA:F7:65:10:6B:11:14
                RPL children:
                RPL DODAG ID: bbbb::2aba:f765:106b:1114
    > udp server start 3000
    udp server start 3000
    Set UDP server port to 3000

On the other node send udp messages

    > udp send bbbb::684:f665:106b:1114 3000 hello
    udp send bbbb::684:f665:106b:1114 3000 hello
    Send 5 byte over UDP to [bbbb::684:f665:106b:1114]:3000
    > msg.l2_sendDoneError: 0
    Send success

The first node should receive the message

    > Received 12 bytes on port 3000
    00000000  A6  28  00  00  00  02  00  68  65  6C  6C  6F .(.....hello

### Communicating with host (IPV6)

OpenVisualizer can set up a tun interface to communicate with the host computer.
This will require starting `OpenVisualizer` with root privileges. The only
difference with the previous setup is that the root node must be setup as
follows:

on iotlab:
    $ IOTLAB_NODE=${ROOT_IOTLAB_NODE}  BOARD=iotlab-m3 \
        make -C tests/pkg_openwsn openv-termtun

on local boards:
    $ PORT=<USB-serial-port, e.g., /dev/ttyUSB0>  BOARD=samr21-xpro \
        make -C tests/pkg_openwsn openv-termtun

Once DAOs are received you can ping nodes in the network from your host:

```
$ ping -s 40 -i 5 bbbb:0:0:0:2ab5:fc65:106b:1114
PING bbbb:0:0:0:2ab5:fc65:106b:1114(bbbb::2ab5:fc65:106b:1114) 40 data bytes
48 bytes from bbbb::2ab5:fc65:106b:1114: icmp_seq=1 ttl=64 time=1064 ms
48 bytes from bbbb::2ab5:fc65:106b:1114: icmp_seq=2 ttl=64 time=2111 ms
48 bytes from bbbb::2ab5:fc65:106b:1114: icmp_seq=3 ttl=64 time=1141 ms
48 bytes from bbbb::2ab5:fc65:106b:1114: icmp_seq=4 ttl=64 time=2197 ms
48 bytes from bbbb::2ab5:fc65:106b:1114: icmp_seq=5 ttl=64 time=1228 ms
48 bytes from bbbb::2ab5:fc65:106b:1114: icmp_seq=6 ttl=64 time=2306 ms
48 bytes from bbbb::2ab5:fc65:106b:1114: icmp_seq=7 ttl=64 time=1324 ms

```

If openserial is used on the leaf node you would get the following output:

```
16:02:38 [ParserIEC:INFO] 768f [ICMPv6ECHO] received an echo request
16:02:44 [ParserIEC:INFO] 768f [ICMPv6ECHO] received an echo request
16:02:48 [ParserIEC:INFO] 768f [ICMPv6ECHO] received an echo request
16:02:54 [ParserIEC:INFO] 768f [ICMPv6ECHO] received an echo request
```

Some considerations:
    - Nodes duty cycle is ~0.5%, so nodes get a chance to transmit roughly every
      2s, so the worst case scenario is ~4s RTT. This is increased for big payloads
      since it will lead to fragmentation.
    - If incoming packet rate is too fast the internal packet queue can be
      be overloaded.
