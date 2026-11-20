@defgroup pkg_zboss  ZBOSS Zigbee Stack
@ingroup  pkg
@brief    Open Source Zigbee Stack for Embedded Devices
@see      https://github.com/Lukas-Luger/zboss

# Overview

This package is based on an [old fork of ZBOSS](https://github.com/niclash/zboss/).
In an attempt to revieve ZBOSS, this package was born. However a closed source
version is also developed by [DSR-IoT](https://dsr-iot.com/solutions/zboss/).
In it current state, the MAC-Layer is completely managed by ZBOSS and workarounds
for RIOT's submac are present. Future plans include to properly utilize
@ref net_ieee802154_submac or even switch to @ref pkg_opendsme if possible.

# Device roles

## End Device

End devices only remember their parent and should rejoin on reboot. They have
lowest functionality and are great for low energy applications such as battery
powered remotes. (see also: IEEE 802.15.4 Full/Reduced Function Device)
```
CFLAGS += -DZB_ED_ROLE
```

## Router

Router should know their end device children and allow them to join. They are
able to create networks and retransmit packets to other devices. Typical routers
include light-bulbs which are mains powered.
(see also: IEEE 802.15.4 Full Function Device)
```
CFLAGS += -DZB_ROUTER_ROLE
```

## Coordinator

Coordinators are routers with IEEE802.15.4 PAN coordinator capabilities.
Typically found in hubs and bridges.
```
CFLAGS += -DZB_COORDINATOR_ROLE
```

# Clusters

Clusters define a specific functionality of a application. Typical clusters
include: on/off, identify, basic, level control and many more.
For now only basic, on/off and group clusters are present.
They are typically divided into server and client parts. A client might request
an off command to a server, which then turns a light off. Servers also require
setup methods, as they might employ state-machines or attributes to be set.

# Endpoints

Endpoints define the place of an application. Note that zero is reserved for
the ZigBee device object (ZDO) otherwise it ranges from 1 to 255. Consistency
throughout the application is highly important.

# Simple descriptor

A simple descriptor can be requested by other devices and defines which clusters
are present on a specific endpoint. Note that they are divided into input(server)
and output(client) clusters.
