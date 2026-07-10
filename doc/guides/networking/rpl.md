---
title: Configuring a multi-hop network with RPL.
description: Guide on how to configure a multihop network with RPL routing on native, hardware, and the FIT IoT-LAB.
---

import FileTree from '@components/FileTree.astro';

## Prerequisites

Please have a look at [The Getting Started](https://guide.riot-os.org/getting-started/installing/) and [Creating an Application](https://guide.riot-os.org/advanced_tutorials/creating_application/).

## Preparation

For routing capabilities, it is necessary that the RPL implementation is compiled into your RIOT application.
This can be achieved by adding `gnrc_rpl` to the `USEMODULE` variable in your application's `Makefile`.
You can consult [this Makefile](https://github.com/RIOT-OS/RIOT/blob/master/examples/networking/gnrc/networking/Makefile) for a complete example.

For the following tutorials, the [gnrc_networking](https://github.com/RIOT-OS/RIOT/blob/master/examples/networking/gnrc/networking/) example will be used.
All commands listed here will be issued from within the `examples/networking/gnrc/networking` directory.
You may use your own application after adjusting the `Makefile` as described above.

## Starting RIOT

You can run the tutorial either on local hardware, native, or the FIT IoT-Lab.

### Using Local Hardware

If you have three or more hardware boards with IEEE 802.15.4 radio capabilities, you can run the tutorial on actual hardware.
This allows you to experiment with different distances between the boards and, e.g., create multi-hop topologies.

In this tutorial, I am using the Adafruit Feather nRF52840 Sense boards.
If you are using different hardware, simply replace the `BOARD` value in the commands below with your board name.

#### Handling Multiple Boards

A detailed description of development with multiple boards can be found in the [build system guide](/build-system/flashing/#handling-multiple-versions-of-the-same-board).
For the sake of simplicity, we are setting the `PORT` and `DEBUG_ADAPTER_ID` values for each board manually in this tutorial.

List all connected boards, their port, and serial number.

```sh
$ make list-ttys

path         | driver  | vendor      | model                           | model_db | serial           | ctime    | iface_num
-------------|---------|-------------|---------------------------------|----------|------------------|----------|----------
/dev/ttyACM0 | cdc_acm | RIOT-os.org | adafruit-feather-nrf52840-sense | None     | AD57F0199B27E0C5 | 14:27:03 | 0
/dev/ttyACM1 | cdc_acm | RIOT-os.org | adafruit-feather-nrf52840-sense | None     | A595DD98EC28FB7C | 14:27:07 | 0
/dev/ttyACM2 | cdc_acm | RIOT-os.org | adafruit-feather-nrf52840-sense | None     | A766F77F8FC7A4A4 | 14:27:13 | 0
```

Open one terminal window for each board and set the `BOARD`, `PORT`, and `DEBUG_ADAPTER_ID` for each board.
Set the `DEBUG_ADAPTER_ID` to the printed `serial` value.

```sh
export BOARD=adafruit-feather-nrf52840-sense PORT=/dev/ttyACM0 DEBUG_ADAPTER_ID=AD57F0199B27E0C5
```

In each window, compile and flash RIOT.

```sh
$ make flash term
```

You will be greeted by RIOT's shell in all three terminals.
In the following sections, the term node {0,1,2} will be used to refer to the RIOT instances running on the boards connected to /dev/ttyACM{0,1,2}, respectively.

### Using Native

Information about the native port of RIOT can be found in [Family:-native](https://github.com/RIOT-OS/RIOT/wiki/Family:-native).

#### Downside

Because all nodes are connected via the `tapbr0` bridge, all nodes are reachable from each other within a single hop.
Therefore, it is not possible to test real multi-hop communication with this method.
[Virtual-riot-network](https://github.com/RIOT-OS/RIOT/wiki/Virtual-riot-network) describes how native nodes can be deployed with a virtualized network topology using tap interfaces.
See [README.desvirt.md](https://github.com/RIOT-OS/RIOT/blob/master/dist/tools/desvirt/README.desvirt.md) for more information.

#### Creating Tap Interfaces

The native RIOT instances communicate through tap interfaces.
We provide a [script](https://github.com/RIOT-OS/RIOT/wiki/Family%3A-native/blob/master/dist/tools/tapsetup/tapsetup) that supports the setup and cleanup of such interfaces.

To set up tap interfaces and a bridge that connects each tap interface:

```sh
$ ../../../../dist/tools/tapsetup/tapsetup -c 4
```

With the `-c` parameter, you can specify how many tap interfaces you want to create (default: 2).
To delete all tap interfaces, you can execute:

```sh
$ ../../../../dist/tools/tapsetup/tapsetup -d
```

See the help for further information about additional parameters:

```sh
$ ../../../../dist/tools/tapsetup/tapsetup -h
```

You can verify that the tap interfaces have been created successfully by looking at the output of `ip l`.
This list should include the new interfaces `tapbr0`, `tap0`, `tap1`, `tap2`, and `tap3`.

#### Compiling for Native

```sh
$ make clean all
```

Type the following command in three different terminals, using `tap0`, `tap1`, and `tap2`, respectively.

```sh
$ make term PORT=tap{0,1,2}
RIOT native interrupts/signals initialized.
TZ not set, setting UTC
RIOT native board initialized.
RIOT native hardware initialization complete.

main(): This is RIOT! (Version: XXX)
RIOT network stack example application
All up, running the shell now
>
```

You will be greeted by RIOT's shell in all three terminals.

In the following sections, the term node {0,1,2} will be used to refer to the nodes at tap{0,1,2}.

### Using the IoT-Lab Testbed

Basic information about the IoT-Lab Testbed can be found in their [wiki](https://github.com/iot-lab/iot-lab/wiki/Riot-support).
This tutorial will cover only the `iotlab-m3` nodes, but the following commands should also be applicable to the `wsn430` nodes.

#### Compiling for the iotlab-m3 Nodes

To compile the `gnrc_networking` example for the `iotlab-m3`, the following command can be executed.

```sh
$ BOARD=iotlab-m3 make all
```

#### Starting an Experiment

An experiment can be started either by using the IoT-Lab [dashboard](https://www.iot-lab.info/testbed) or by using a [Makefile](https://github.com/RIOT-OS/RIOT/blob/master/dist/testbed-support/Makefile.iotlab) provided by RIOT.
In this tutorial, the `Makefile` method will be used.

Add the iotlab Makefile to your own Makefile:

```mk
include $(RIOTBASE)/dist/testbed-support/Makefile.iotlab
```

To start an experiment, execute:

```sh
$ IOTLAB_USER=<iotlab_user> IOTLAB_PHY_NODES=10-12+14-16 IOTLAB_EXP_NAME=test IOTLAB_DURATION=60 BOARD=iotlab-m3 make iotlab-exp
```

This command may prompt for a password to authenticate your IoT-Lab account.
You will need to use the same password that you use for the IoT-Lab [dashboard](https://www.iot-lab.info/testbed).

|Parameter|Description|
|---------|-----------|
|IOTLAB_USER|Specify your IoT-Lab account name. The same name used for the [dashboard](https://www.iot-lab.info/testbed)|
|IOTLAB_PHY_NODES|Choose the nodes for the experiment. If a node is not available, the Makefile will return an error specifying which nodes are busy|
|IOTLAB_EXP_NAME|Name of the experiment|
|IOTLAB_DURATION|Duration in minutes|
|BOARD|The board to use: iotlab-m3|

More information and further parameters can be found in the [README](https://github.com/RIOT-OS/RIOT/blob/master/dist/testbed-support/README.iotlab.md).

To flash your application onto all nodes in the experiment, use the `iotlab-flash` command:

```sh
$ IOTLAB_USER=<iotlab_user> BOARD=iotlab-m3 make iotlab-flash
```

Assuming the experiment started successfully, you can then communicate with the nodes directly from the command line like this:

```sh
$ IOTLAB_USER=<iotlab_user> BOARD=iotlab-m3 make iotlab-term
```

This command may prompt you for a password if your SSH key that you uploaded to the IoT-Lab Testbed for SSH usage is password protected.

Once you are connected, the serial aggregator of the IoT-Lab testbed will be started, and you will have a console from which you can control every node.

##### Flashing Individual Nodes

Alternatively, you can flash individual nodes or get their serial I/O by setting `IOTLAB_NODE` and using the usual `make flash` / `make term` commands, e.g.:

```sh
$ IOTLAB_NODE=m3-10.grenoble.iot-lab.info BOARD=iotlab-m3 make flash term
```

#### Using the Serial Aggregator

Each command issued in the serial aggregator will be transmitted to all nodes used for the current experiment.
To direct a command specifically to a single node, the node ID must be prepended to the command.

E.g., start RPL on interface 5 for all nodes:

```sh
> rpl init 5

m3-10;> rpl init 5
m3-10;successfully initialized RPL on interface 5
m3-11;> rpl init 5
m3-11;successfully initialized RPL on interface 5
m3-12;> rpl init 5
m3-12;successfully initialized RPL on interface 5
m3-14;> rpl init 5
m3-14;successfully initialized RPL on interface 5
m3-15;> rpl init 5
m3-15;successfully initialized RPL on interface 5
m3-16;> rpl init 5
m3-16;successfully initialized RPL on interface 5
```

Start RPL only on one node:

```sh
> m3-10; rpl init 5

m3-10;successfully initialized RPL on interface 5
```

In the tutorial below we use node {0,1,2} to refer to specific nodes.
These commands should be prepended with the node ids, e.g., m3-10 / m3-11 / m3-12 for node 0, node 1, and node 2, respectively.

## Initializing RPL

In order to use RPL, we have to choose a RPL root node.
We will choose node 0 as the RPL root.

With the `ifconfig` command, we can get the configured interfaces for a particular RIOT node.
On my system, the output is as follows for node 0.
It may look slightly different if you are using different hardware or are running on native.

```sh
> ifconfig

Iface  6  HWaddr: 68:C5  Channel: 26  NID: 0x23  PHY: O-QPSK
           Long HWaddr: AE:57:F0:19:9B:27:E8:C5
           TX-Power: 0dBm  State: IDLE
           ACK_REQ  L2-PDU:102  MTU:1280  HL:64  RTR
           RTR_ADV  6LO  IPHC
           Source address length: 8
           Link type: wireless
           inet6 addr: fe80::ac57:f019:9b27:e8c5  scope: link  VAL
           inet6 group: ff02::2
           inet6 group: ff02::1
           inet6 group: ff02::1:ff27:e8c5
           inet6 group: ff02::1a
```

At first, we need to configure a global IPv6 address **for one node**, the root node.

```sh
> ifconfig 6 add 2001:db8::1
success: added 2001:db8::1/64 to interface 6

> ifconfig

Iface  6  HWaddr: 68:C5  Channel: 26  NID: 0x23  PHY: O-QPSK
           Long HWaddr: AE:57:F0:19:9B:27:E8:C5
            TX-Power: 0dBm  State: IDLE
           ACK_REQ  L2-PDU:102  MTU:1280  HL:64  RTR
           RTR_ADV  6LO  IPHC
           Source address length: 8
           Link type: wireless
           inet6 addr: fe80::ac57:f019:9b27:e8c5  scope: link  VAL
           inet6 addr: 2001:db8::1  scope: global  VAL
           inet6 group: ff02::2
           inet6 group: ff02::1
           inet6 group: ff02::1:ff27:e8c5
           inet6 group: ff02::1a
```

Now, RPL must be initialized on that particular interface **for all nodes**:

```sh
> rpl init 6
successfully initialized RPL on interface 6
```

## Starting RPL

On the root node (node 0 in this tutorial), type:

```sh
> rpl root 1 2001:db8::1
successfully added a new RPL DODAG
```

Note, however, that the DODAG ID `2001:db8::1` matches a global IPv6 address configured on an interface, as done in [Initializing RPL](initializing-rpl).
The `1` before `2001:db8::1` is the Instance ID and can be adjusted if desired.

To see the RPL status, type `rpl`.

```sh
> rpl

instance table:	[X]	
parent table:	[ ]	[ ]	[ ]	

instance [1 | Iface: 5 | mop: 2 | ocp: 0 | mhri: 256 | mri 0]
	dodag [2001:db8::1 | R: 256 | OP: Router | PIO: on | TR(I=[8,20], k=10, c=1)] rpl
```

The root node has a rank of `R: 256`.

Now type `rpl` in the other terminals (here is an example for node 1):

```sh
> rpl

instance table:	[X]	
parent table:	[X]	[ ]	[ ]	

instance [1 | Iface: 5 | mop: 2 | ocp: 0 | mhri: 256 | mri 0]
	dodag [2001:db8::1 | R: 512 | OP: Router | PIO: on | TR(I=[8,20], k=10, c=1)]
		parent [addr: fe80::ac57:f019:9b27:e8c5 | rank: 256]
```

This node is part of the RPL DODAG with the DODAG ID `2001:db8::1` and Instance ID `1`.
Furthermore, this node's rank is `R: 512`, and the preferred parent has the link-local IPv6 address `fe80::d069:49ff:fede:b38c`.
This parent's rank is `256`, therefore we know that this parent must be the root of this RPL DODAG.

Note that all participating nodes in the RPL DODAG automatically configure a global IPv6 address matching the prefix from the DODAG ID. Verify this with `ifconfig`.

RPL should now start to fill the NIB (Neighbor Information Base) table, which is consulted when forwarding packets.
On the root node (node 0):

```sh
> nib route

2001:db8::/64 dev #6
2001:db8::14f3:36ff:fe6f:6395/128 via fe80::14f3:36ff:fe6f:6395 dev #6
2001:db8::d8bc:8aff:feaa:7c46/128 via fe80::d8bc:8aff:feaa:7c46 dev #6
```

and on the participating node (node 1):

```sh
> nib route

2001:db8::/64 dev #6
default* via fe80::ac57:f019:9b27:e8c5 dev #6
```

The nodes can now ping each other using the public IP addresses (replace the destination IP):

```sh
> ping 2001:db8::14f3:36ff:fe6f:6395

12 bytes from 2001:db8::14f3:36ff:fe6f:6395: icmp_seq=0 ttl=64 time=1.728 ms
12 bytes from 2001:db8::14f3:36ff:fe6f:6395: icmp_seq=1 ttl=64 time=1.070 ms
12 bytes from 2001:db8::14f3:36ff:fe6f:6395: icmp_seq=2 ttl=64 time=1.010 ms

--- 2001:db8::14f3:36ff:fe6f:6395 PING statistics ---
3 packets transmitted, 3 packets received, 0% packet loss
round-trip min/avg/max = 1.010/1.269/1.728 ms
```

