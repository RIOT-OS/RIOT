# OpenWSN MAC on RIOT

This test demonstrates how to run the
[OpenWSN](https://github.com/openwsn-berkeley/openwsn-fw)  MAC layer
(IEEE802.15.4E TSCH) isolated from the other components.

## Experimental setups
The following experiments act as a starting point for testing and debugging. Either
build and flash local nodes or incorporate the [FIT IoT-LAB](https://www.iot-lab.info/)
Testbed. Please check the ports [documentation](../../pkg/openwsn/doc.txt) for information
about supported hardware platforms.

A TSCH network requires at least one node running as PAN Coordinator, and one or
more nodes running as Coordinator (parent nodes than can advertise themselves so
othe nodes can join the TSCH network) or Leaf (end devices that can only join the
network, usually low power constrained devices).

## Start a TSCH network

1. Set the slotframe length in the PAN coordinator with the `slotframe` command:
```
openwsn sched slotframe 101
```

2. Set the role of the PAN coordinator to `pancoord`:
```
openwsn role pancoord
```

3. Use the `cell` command to add an advertisement cell in the PAN coordinator:
E.g to add an advertisement cell in `slot_offset` 0 and `channel_offset` 0:
```
openwsn sched add 0 0 adv
```

4. Wait for some seconds. Nodes nearby should print "`Synchronized`" when they
joined the TSCH network.

## Send messages between nodes.

1. Read the 64 bit Link Layer address from the receiver:
```
ifconfig

2020-09-09 14:57:51,229 # Iface  2      HWaddr: 2A:B5  NID: FE:CA
2020-09-09 14:57:51,229 #
2020-09-09 14:57:51,230 #               Long HWaddr: 2A:B5:FC:65:10:6B:13:15
2020-09-09 14:57:51,230 #               IEEE802154E sync: 1

```

2. Send a message to that address using `txtsnd` command.
```
txtsnd <address> <message>
```
E.g
```
txtsnd 2A:B5:FC:65:10:6B:13:15 "Hello World!"
```

If the address is not specified, the message will be multicasted to all nodes
in the line of sight of the sender.

## Expand the network

To add more nodes outside of the boundaries of the PAN coordinator, simply add
coordinators in the line of sight of the PAN coordinator or regular coordinators.

To declare a node as a coordinator, use the `role` command
```
openwsn role coord
```

A node can be set to leaf again using the same command:
```
openwsn role leaf
```

## Schedule management

Use the `sched` command to manage the cells:

```
sched add <slot_offset> <channel_offset> <adv|txrx|tx|rx> [<address>]: add cell directly to schedule
sched rmv <slot_offset> <channel_offset> <adv|txrx|tx|rx> [<address>]: remove cell directly in schedule

```

E.g to create a link in slot offset 50 and channel offset 20 that receives
data from address 79:65:18:40:22:11:c7:4e
```
sched add 50 20 rx 79:65:18:40:22:11:c7:4e
```

E.g to create a link in slot offset 50 and channel offset 20 that sends
data to address 79:65:18:40:22:11:aa:bb

```
sched add 50 20 tx 79:65:18:40:22:11:aa:bb
```

With this configuration when 79:65:18:40:22:11:c7:4e sends a packet to
79:65:18:40:22:11:aa:bb, it will be sent either in the advertisement slot
(anycast) or in the (50,20) slot.

It's also possible to add more advertisement cells. E.g
```
sched add 40 0 adv
```

To remove the recently created cell:
```
sched rem 40 0 adv
```

## 6top Schedule management

Use the `6top` command to manage the schedule through 6top:

```
6top clear [<neighbor>]: request neighbor to clear all cells
6top add <num> <adv|tx|rx> [<neighbor>]: request parent to add num cells
6top rmv <num> <adv|tx|rx> [<neighbor>]: request parent to remove num cells
6top rel <num> <adv|tx|rx> [<neighbor>]: request parent to relocate num cel
```

For example requesting 2 transmit cells to 36:8F:FA:65:10:6B:13:15.

```
openwsn 6top add 2 tx 36:8F:FA:65:10:6B:13:15
sixtop request succeeded

openwsn sched list
neigh: 00:00:00:00:00:00:00:00, slot: 000, chan: 00, type: RXTX
neigh: 36:8F:FA:65:10:6B:13:15, slot: 062, chan: 03, type: TX
neigh: 36:8F:FA:65:10:6B:13:15, slot: 083, chan: 07, type: TX
neigh: 36:8F:FA:65:10:6B:13:15, slot: 073, chan: 10, type: TX
```

## Change slotframe length

Before starting the pan coordinator, set the frame length with the `slotframe`
command:

```
openwsn slotframe <number_of_slots>
```

By default, `number_of_slots` is set to 101. A lower number will decrease the
latency but reduce the number of available slots.
