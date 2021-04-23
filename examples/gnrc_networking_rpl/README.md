# gnrc_networking_rpl Demo

This example demonstrates the use of RPL with link metrics.

To showcase the difference between different objective functions and
link metrics, each node in the mesh periodically sends a message to
the root node.

The root node counts all received messages and stores the number of
hops as well as link quality information for the last hop of the last
packet from each node.

This allows for direct comparison between routing strategies, the more
packets the root receives from all mesh nodes, the better the mesh.

## Setup on mesh nodes

Mesh nodes don't need any additional setup.
As soon as they have a RPL root, they will periodically (each second)
send a hello packet to the root.

## Setup on root node

On the root node run

```
test start
```

to begin the experiment.

To query the current results, run

```
test info
```

The summary provides the number of detected nodes as well as the number of packets
received per second.

In an ideal mesh, those numbers should be equal, that is every periodic packet of
every node is received.
