# Setup the build
First switch to this application directory:

	cd RIOT/examples/rpl_udp

The build system provides three specific parameters for the RPL module.
These parameters are:
 * `RPL_MOP` sets the _mode of operation_ (MOP) of RPL. _The default value (`0x02`) is used if this variable is not set._
 * `RPL_MAX_ROUTING_ENTRIES` sets the desired maximum number of entries for the RPL routing table. _If this parameter is not provided, a default value (`128`) is used._
 * `RPL_NODE_IS_ROOT` indicating the build is specifically for a root node. This parameter is **only required** for using `RPL_MOP_NON_STORING_MODE` MOP to initialize a RPL routing table. The parameter is ignored in all other cases. _For non-storing MOP a node does not require to provide a routing table._

These RPL build parameters and their according _default_ values are used in the [`rpl_config.h`](https://github.com/RIOT-OS/RIOT/blob/master/sys/net/include/rpl/rpl_config.h).

#### Compiling the executable
**example for `RPL_MOP_STORING_MODE_MC`:**

	make RPL_MOP=RPL_MOP_STORING_MODE_MC

builds the project and creates `128` entries for the RPL routing table by default.

	make RPL_MOP=RPL_MOP_STORING_MODE_MC RPL_MAX_ROUTING_ENTRIES=103

builds the project and creates `103` entries for the RPL routing table **overwriting** the default value.

**example for `RPL_MOP_NON_STORING_MODE`:**

	make RPL_MOP=RPL_MOP_NON_STORING_MODE RPL_MAX_ROUTING_ENTRIES=103 RPL_NODE_IS_ROOT=1

builds the project and creates `103` entries for the RPL routing table for the root node.

#### _[native only]_ Set up a bridge for *two* tap devices for communication

	../../cpu/native/tapsetup.sh create 2

The `2` corresponds to the number of _virtual_ nodes to be started.
The tap devices are bound to the _virtual_ native node on starting the created executable `rpl_udp.elf`.

#### _[native only]_ Starting _virtual_ nodes
Run the first node

	make term

This starts the executable binding a _virtual_ node to the first tap device `tap0`.

**Further nodes must be bound to tap devices manually.**
For the second node it would be:

	make term PORT=tap1


# RPL init
Type ``help`` to see the commands available.

#### Command order:

	init (r|n|h)
	server
	send <node-id> <message>


Init your RPL node either as root (`r`), as node router (`n`), or as non-routing node (host-mode) (`h`)

	> init r
	init r
	INFO: Initialize as root on address 1
	6LoWPAN and RPL initialized.
	Channel set to 10
	Transport layer initialized

or

	> init n
	init n
	INFO: Initialize as node on address 2
	6LoWPAN and RPL initialized.
	Channel set to 10
	Transport layer initialized

or

	> init h
	init h
	INFO: Initialize as non-routing node on address 3
	6LoWPAN initialized.
	Channel set to 10
	Transport layer initialized

(It is advised to start all nodes before running the root node, because the interval for sending DIOs from the root increases over time)
A few seconds withing running, setting and initializing your second node, the nodes will automatically start to exchange control traffic. You should be seeing output similar to the following:

	41 CC FC 34 12 36 12 00 FF FE 00 00 02 36 12 00 FF FE 00 00 01 7A 33 3A 9B 03 F9 90 00 00 04 00 00 80
	IPv6 datagram received (next header: 3A) from fe80:0000:0000:0000:3612:00ff:fe00:0001 	 ICMP type: 5A 	 ICMP code: 08
	Received packet from ID 1
		Length:	34
		Src:	1
		Dst:	2
		LQI:	0
		RSSI:	0

You can now use the ``dodag`` command to obtain information on the node's rank in the rpl dodag as well as their preferred parent

	> dodag
	dodag
	---------------------------
	Part of Dodag:
	fe80:0000:0000:0000:3612:00ff:fe00:0001
	my rank: 512
	my preferred parent:
	fe80:0000:0000:0000:3612:00ff:fe00:0001
	---------------------------

#Sending via UDP

In order to be able to receive UDP messages from other nodes, we have to start `rpl_udp`'s UDP server:

	> server
	server
	UDP SERVER ON PORT 511 (THREAD PID: 17)

When rpl is running, you can send a message from any participating node to the node initialized as a server (in our case, node 1) using the ``send`` command

	> send 1 hi
	send 1 hi
	Successful deliverd 11 bytes over UDP to abcd:0000:0000:0000:3612:00ff:fe00:0001 to 6LoWPAN

In case of an error message, make sure that rpl is running and you've started the UDP server on the receiving node by running the ``server`` command.
