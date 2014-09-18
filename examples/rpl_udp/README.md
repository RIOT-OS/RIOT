#tl;dr

command order:

	set <node-id>
	init [r|n]
	server
	send <node-id> <message>

# setup
compile the application

	cd RIOT/examples/rpl_udp
	make

Set up a bridge for *two* tap devices to communicate over

	../../cpu/native/tapsetup.sh create 2

# rpl init

Run the first node

	make term

Type ``help`` to see the commands available.
Now you need set the node ID. This ID will be used by other nodes to address this node.

	> set 1
    set 1
    Set node ID to 1

Init your rpl node as either a root or a node router.

	> init n
	init n
	INFO: Initialize as node on address 2
	6LoWPAN and RPL initialized.
	Channel set to 10
	Transport layer initialized

or

	> init r
	init r
	INFO: Initialize as root on address 1
	6LoWPAN and RPL initialized.
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

#sending via UDP

In order to be able to receive UDP messages from other nodes, we have to start rpl_udp's UDP server:

	> server
	server
	UDP SERVER ON PORT 511 (THREAD PID: 17)

When rpl is running, you can send a message from any participating node to the node initialized as a server (in our case, node 1) using the ``send`` command

	> send 1 hi
	send 1 hi
	Successful deliverd 11 bytes over UDP to abcd:0000:0000:0000:3612:00ff:fe00:0001 to 6LoWPAN

In case of an error message, make sure that rpl is running and you've started the UDP server on the receiving node by running the ``server`` command.
