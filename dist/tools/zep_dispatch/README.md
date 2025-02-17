ZEP packet dispatcher
=====================

The ZEP dispatcher is a simple daemon that forwards ZEP packets to all connected
nodes.

```
usage: zep_dispatch [-t topology] [-s seed] [-g graphviz_out] <address> <port>
```

By default the dispatcher will forward every packet it receives to every other
connected node (flat topology).

Advanced Topology Mode
----------------------

It is possible to simulate a more complex topology as well as packet loss by
specifying a topology file.


The file format is:

```
<node_a> <node_b> [weight_ab] [weight_ba]
```

This line defines a connection between <node_a> and <node_b>.
The weight of the edge <node_a> -> <node_b> is <weight_ab>.

An edge weight is a float value between 0 and 1 that represents the probability of
a successful packet transmission on that path.

A weight of `0` would mean no connection, whereas a weight of `1` would be a perfect
connection. Intermediate values are possible, e.g. `0.6` would result in a packet
loss probability of 40%.

The weights can be omitted.
If both weights are omitted, a perfect connection is assumed.
If <weight_ba> is omitted, a symmetric connection is assumed and <weight_ab> is used
for both directions.

refer to the file `example.topo` for an example.

There can only be as many nodes connected to the dispatcher as have been named in
the topology file.
Any additional nodes that try to connect will be ignored.


Packet capture
--------------

To view traffic in Wireshark you need to create a virtual 802.15.4 device to which
the network traffic is sent. This can then be selected as a capture source in Wireshark.

To create the virtual device, load the `mac802154_hwsim` module

    sudo modprobe mac802154_hwsim

This will create two wpan devices, `wpan0` and `wpan1`.

If you then run the dispatcher with

    sudo zep_dispatch -w wpan0 ::1 17754

all traffic on the simulated network will be also sent to the virtual `wpan0` interface
where it can be captures with Wireshark.


Network visualization
---------------------

It is possible to generate a graph of the current simulated network by executing the

    make graph

target. This sends a USR1 signal to the `zep_dispatch` process to generate a graph in
Graphviz format.


### Foren6

For real-time visualization of the (RPL) network, the [Foren6](https://cetic.github.io/foren6/)
tool can be used.

Support for ZEP dispatcher is currently only available through [a fork](https://github.com/benpicco/foren6).

#### Foren6 setup

Start `foren6` with the `make run` target and in the 'Manage Sources' menu, add a ZEP sniffer:

![](https://user-images.githubusercontent.com/1301112/134354399-7010ad73-044d-4ffa-ad99-61a6838af268.png)

The 'target' field takes a hostname:port combination, but you can leave it blank. In that case the default `[::1]:17754` will be used.

#### Foren6 usage

Foren6 will connect to a running ZEP dispatcher. If the dispatcher started after Foren6 or
if the dispatcher was restarted, you have to re-connect by stopping the current capture ('Stop' button)
and starting it again ('Start' button).

You should see a view of the DODAG with all nodes in the network as they send packets.

![](https://user-images.githubusercontent.com/1301112/144511776-3a2d7072-8162-40dc-911f-dfe476d01112.png)

#### RIOT setup

A proper simulated network will need a border router and some mesh nodes.
The `gnrc_border_router` and `gnrc_networking` can serve as a starting point for those.

By default the border router example will start the ZEP dispatcher with a flat topology, that is
each node is connected to every other node.
This makes for a very boring topology where no RPL is needed.

To run the ZEP dispatcher with a custom topology, simply run it before the `gnrc_border_router`
example. You can use the `make run` target which will default to the `example.topo` topology.
This can be changed with the `TOPOLOGY` environment variable.

Next, start the border router example with RPL enabled:

    USEMODULE=gnrc_rpl make -C examples/networking/gnrc/gnrc_border_router all term

Verify that the border router got a prefix on it's downstream interface with `ifconfig`.

```
Iface  7  HWaddr: 36:DE  Channel: 26  NID: 0x23
          Long HWaddr: BE:92:89:04:D7:B4:B6:DE
          L2-PDU:102  MTU:1280  HL:64  RTR
          RTR_ADV  6LO  IPHC
          Source address length: 8
          Link type: wireless
          inet6 addr: fe80::bc92:8904:d7b4:b6de  scope: link  VAL
     -->  inet6 addr: 2001:db8::bc92:8904:d7b4:b6de  scope: global  VAL
          inet6 group: ff02::2
          inet6 group: ff02::1
          inet6 group: ff02::1:ffb4:b6de
          inet6 group: ff02::1a

Iface  6  HWaddr: 7A:37:FC:7D:1A:AF
          L2-PDU:1500  MTU:1500  HL:64  RTR
          Source address length: 6
          Link type: wired
          inet6 addr: fe80::7837:fcff:fe7d:1aaf  scope: link  VAL
          inet6 addr: fe80::2  scope: link  VAL
          inet6 group: ff02::2
          inet6 group: ff02::1
          inet6 group: ff02::1:ff7d:1aaf
          inet6 group: ff02::1:ff00:2
```

Now start as many `gnrc_networking` nodes as you have mesh nodes defined in your topology file:

    USE_ZEP=1 make -C examples/networking/gnrc/gnrc_networking all term
    USE_ZEP=1 make -C examples/networking/gnrc/gnrc_networking all term
    …

The node should be able to join the DODAG as you can verify with the `rpl` command:

```
instance table:	[X]
parent table:	[X]	[ ]	[ ]

instance [0 | Iface: 7 | mop: 2 | ocp: 0 | mhri: 256 | mri 0]
	dodag [2001:db8::bc92:8904:d7b4:b6de | R: 768 | OP: Router | PIO: on | TR(I=[8,20], k=10, c=119)]
		parent [addr: fe80::6467:2e89:b035:d51f | rank: 512]
```

This should also be visible in Foren6.

Topology generation
-------------------

To generate a random topology use the `topogen.sh` script.
This will randomly distribute *N* nodes on on a *W* × *H* map.
Each node has a radio range *R* ± *V* where *V* is a random variance that can also be set to 0.

The further away a node is from a sending node, the higher the packet loss probability.
Nodes outside the sending radius have a zero probability of receiving a packet.

If you have `gnuplot` installed this will also generate a plot of the resulting node distribution:

![example topology](https://gist.githubusercontent.com/benpicco/6fd6f7c79a30cbbc41c3a65e53ed3682/raw/33afb859b65d949238129096858d14e2319fb5fb/network.topo.svg)

A light color means that a node only has a one-way connection to the network, gray means a node is
entirely isolated.
