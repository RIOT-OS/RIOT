# About
This is a manual test application for the KSZ8851SNL wired ethernet driver

For running this test, you need Xplained Pro SAMR21 Board with an Ethernet1 XPRO accessory Board
Make sure the Ethernet1 accessory Board is connected to extension header 1

# Usage
For testing the ethernet driver:

check the interface number by typing ifconfig
```
> ifconfig
Iface  5   HWaddr: 00:04:25:19:1a:3e  State: IDLE
           MTU:1500  
           Source address length: 6
           Link type: wired
           inet6 addr: ff02::1/128  scope: local [multicast]
           inet6 addr: fe80::204:25ff:fe19:1a3e/64  scope: local
           inet6 addr: ff02::1:ff19:1a3e/128  scope: local [multicast]
           inet6 addr: ff02::2/128  scope: local [multicast]
```

add a new unicast address for the Samr21-Xpro.
```
> ifconfig 5 add affe::2
success: added affe::2/64 to interface 5
```
make sure the PC has a static IPv6 of affe::1/64 the ping the board.
```
ping6 affe::2
```
from the Samr21-Xpro type:
```
>ping6 affe::1
```
