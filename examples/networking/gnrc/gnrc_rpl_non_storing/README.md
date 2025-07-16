# GNRC RPL Non-Storing Mode Example

This example demonstrates how to configure and use **RPL in non-storing mode** using RIOT OS. It covers setting up a root node, verifying the source routing table, and checking connectivity between nodes.

> This example requires **at least two real nodes** to function properly.

---

## Prerequisites

Ensure the application is compiled for your target board. For example, for IoT-LAB M3:

```sh
make BOARD=iotlab-m3
```

> Replace `iotlab-m3` with the appropriate board name if using another platform.

---

## Select RPL Mode

You can define the RPL Mode of Operation (MOP) via `CFLAGS` in the Makefile:

- **Storing Mode**
    ```make
    CFLAGS += -DGNRC_RPL_MOP_STORING_MODE_NO_MC=1
    ```
    or

    ```make
    CFLAGS += -DGNRC_RPL_MOP_STORING_MODE_MC=1
    ```

- **Non-Storing Mode**
    ```make
    CFLAGS += -DCONFIG_GNRC_RPL_MOP_NON_STORING_MODE=1
    ```

---

## Configure the Root Node

1. Check the node's **link-local IPv6 address**:

   ```sh
   ifconfig
   ```

   Example output:

   ```
   Iface  6  ...
   inet6 addr: fe80::58ac:f9d7:b9f1:63c7  scope: link  VAL
   ```

2. Assign a **global IPv6 address** and configure the node as the RPL root (we use `2001:db8::` as a prefix):

   ```sh
   ifconfig 6 add 2001:db8::58ac:f9d7:b9f1:63c7
   rpl root 1 2001:db8::58ac:f9d7:b9f1:63c7
   ```

   > Replace `6` with the correct interface number from your `ifconfig` output.

3. Confirm the node now has a global IPv6 address:

   ```
   inet6 addr: 2001:db8::58ac:f9d7:b9f1:63c7  scope: global  VAL
   ```

---

## Check the Source Routing Table

On the root node, use the following command to inspect the source routing entries:

```sh
rpl sr_table
```

Sample output:

```
Destination                             Flags     Next Hop                               Flags      Expires       Interface
2001:db8::acab:2bd9:edfc:e934           0x00000001 H      2001:db8::58ac:f9d7:b9f1:63c7  0x00000001 297.60
2001:db8::6c39:af4c:615d:9313           0x00000001 H      2001:db8::d840:f681:c642:30d0  0x00000001 291.68
...
```

---

## Test Node Connectivity (ping)

Ping one of the leaf nodes to verify the source routing path is working:

```sh
ping 2001:db8::6c39:af4c:615d:9313
```

Example output:

```
12 bytes from 2001:db8::6c39:af4c:615d:9313: icmp_seq=0 ttl=62 rssi=-67 dBm time=32.92 ms
12 bytes from 2001:db8::6c39:af4c:615d:9313: icmp_seq=1 ttl=62 rssi=-68 dBm time=32.34 ms
12 bytes from 2001:db8::6c39:af4c:615d:9313: icmp_seq=2 ttl=62 rssi=-67 dBm time=38.31 ms
```
