Overview
========

This test application can be used to check the implementation of the tinyUSB
netdev driver. The implementation of the tinyUSB netdev driver uses Ethernet
over USB and supports the following protocols:

- CDC ECM (Ethernet Control Model)
- CDC NCM (Network Control Model)
- RNDIS (Microsoft Remote NDIS)

While Linux and macOS support all these protocols, Microsoft Windows only
supports RNDIS and since Windows version 11 also CDC NCM. Which protocol is
used is selected by the corresponding pseudomodules `tinyusb_class_net_cdc_ecm`,
`tinyusb_class_net_cdc_ncm` and `tinyusb_class_net_rndis`.

Configuration
=============
The test application use the protocol defined by the CLASS variable, which
defaults to the RNDIS protocol (`tinyusb_class_net_rndis`). This can be
changed by setting this variable in the make command line, for example:
```
CLASS=tinyusb_class_net_cdc_ecm BOARD=... make -C tests/pkg/tinyusb_netdev flash
```
The CDC ECM protocol (`tinyusb_class_net_cdc_ecm`) and the RNDIS protocol
(`tinyusb_class_net_rndis`) can be used simultaneously to support all operating
systems, for example :
```
CLASS='tinyusb_class_net_rndis tinyusb_class_net_cdc_ecm' \
BOARD=... make -C tests/pkg/tinyusb_netdev flash
```
In this case, the CDC ECM protocol is the default protocol and the RNDIS
protocol the alternative protocol defined as second device configuration.

The CDC NCM protocol cannot be used together with the CDC ECM or the RNDIS
protocol

Expected result
===============

Use the network related shell commands to verify the network link between the
board under test and the host computer. Ping to the link local address from and
to the host computer must work.

On the host computer, using tools such as `ethtool` must show the USB CDC ECM
interface as link detected:

```
# ethtool enp0s20u9u4
Settings for enp0s20u9u4:
        Current message level: 0x00000007 (7)
                               drv probe link
        Link detected: yes
```
