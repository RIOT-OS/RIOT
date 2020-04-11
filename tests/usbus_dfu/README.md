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

Background
==========

This test application can be used to verify the USBUS CDC ECM implementation.
Assuming drivers available, the board under test should show up on the host
computer as an USB network interface. Drivers are available for both Linux and
macOS.
