# About openDSME

The IEEE 802.15.4 standard with its widespread usage in wireless sensor and
actuator networks was extended by several techniques that allow reliable data
transmission for critical applications, such as industrial plants. This
includes the Deterministic and Synchronous Multi-channel Extension (DSME) that
allows for distributed assignment of time slots on multiple channels.

openDSME is an open source implementation of the Deterministc and
Synchronous Multi-channel Extension.

## Features

- Topology agnostic (peer-to-peer, star, mesh, etc).
- By design, a DSME network can be extended just by adding more coordinators near the edge.
- Automatic resolution of beacon collision
- Frame transmission using either CSMA-CA or multi-channel GTS (guaranteed time slot)
- Built-in slot negotiation (dynamic allocation) and static allocation.

# Usage
1. A PAN coordinator device starts the DSME network via the GNRC network interface:
```
ifconfig <if_num> pan_coord
ifconfig <if_num> up
```

2. Child devices trigger scanning and join the DSME network:
```
ifconfig <if_num> up
```

3. On a fraction of second, `ifconfig` of the joining child device
should display "Link: up" and the L2 address should be visible

4. Devices may choose CSMA-CA or GTS transmissions. CSMA-CA transmissions are
selected by default. To switch to GTS transmissions, run:

```
ifconfig <if_num> gts
```

To switch back to CSMA-CA transmissions, run:
```
ifconfig <if_num> -gts
```
5. To transmit data, use the `txtsnd` command:
```
txtsnd <if_num> <target_L2_address> <message>
```
In case of GTS transmissions, the MAC will negotiate a slot with the target
device. On success, the message will be transmitted in the next occurrence of
the slot. As a result, the first GTS transmission will take longer than
subsequent transmissions.
Note that GTS expire if not used, which will require a new negotiation.
The expire timeout can be configured with `CONFIG_IEEE802154_DSME_GTS_EXPIRATION`
(by default 16 idle slot occurrences).

## Static GTS allocation
Although not supported by the standard, openDSME includes a mechanism to override
the dynamic allocation with static allocation. This is useful for experimentation
or quick prototypes.
To enable, compile openDSME with `CONFIG_IEEE802154_DSME_STATIC_GTS=1`. This can
be set via Kconfig or CFLAGS.

To allocate slots statically, use the `gts` command:
```
gts <if_num> <neighbour_L2_address> <is_tx> <superframe_id> <slot_id> <channel_id>
```

For example, to allocate a pair of slots node 1 (`DE:AD`) and node
B (`BE:EF`), run:

In node 1 (`DE:AD`)
```
gts 3 BE:EF 1 0 0 0
```
In node 2 (`BE:EF`)
```
gts 3 DE:AD 0 0 0 0
```

The slot is located in the first slot (0) of the first superframe in the
multisuperframe (0) and uses the first channel (0). Node 1 allocates a TX slot
and node 2 an RX slot.
In contrast to standard GTS, static GTS do not expire.

To understand more about the superframe structure, check the openDSME
documentation in the RIOT documentation.

# Port limitations
- So far only GNRC link layer communication is supported (e.g `txtsnd`).
- Only PAN coordinator and child roles are supported. This is not an
  openDSME limitation, but rather a missing configuration in the RIOT contrib.
- The current openDSME utilizes heap to allocate internal data structures.
- The scanning/joining procedure is hardcoded to join openDSME beacons.
