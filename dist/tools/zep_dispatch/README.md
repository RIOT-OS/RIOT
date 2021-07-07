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
