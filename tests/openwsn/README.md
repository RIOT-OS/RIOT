# OpenWSN on RIOT
This test demonstrates the [OpenWSN](https://github.com/openwsn-berkeley/openwsn-fw) full
stack (UDP, IPv6, RPL, 6TiSCH) running on RIOT. When flashed, it will initialize the stack
and provide the user with some minimal shell commands to:

- print the own IPv6 address `ifconfig`
- start a RPL root node `rplroot`
- change the UDP destination port `udp server start <port>`
- send a UDP packet `udp send <addr> <port> <data>`

Please note that this port is still in experimental status. For further information about the port,
currently supported platforms as well as issues, TODOs and debugging mechanisms, please refer to
the documentation](../../pkg/openwsn/doc.txt).

## Experimental setups
The following experiments act as a starting point for testing and debugging. Either
build and flash local nodes or incorporate the [FIT IoT-LAB](https://www.iot-lab.info/)
Testbed. Please check the ports [documentation](../../pkg/openwsn/doc.txt) for information
about supported hardware platforms.

### Single-hop w/o channel hopping
Build this application with `CFLAGS=-DOW_RIOT_SINGLE_CHANNEL=26` to disable channel hopping
and simply using radio channel 26. Flash it to two boards. Type `ifconfig` to explore the
link local IPv6 addresses. Then, on one node, enable the RPL DODAG root by `rplroot`.
After short time, check the IPv6 addresses again with `ifconfig`. You should see that both
have a global unicast address now.

### Single-hop w/ channel hopping
Do the same as above but **without** setting the `OW_RIOT_SINGLE_CHANNEL` flag.

### Multi-hop w/o channel hopping
Build this application with `CFLAGS=-DOW_RIOT_SINGLE_CHANNEL=26` to disable channel hopping.
Flash it to three boards. On one node, enable the RPL DODAG root by `rplroot`. After
short time you should see all three nodes are equipped with global unicast addresses.

The default UDP server port in this application is set to `61617`. You can change it with the
above mentioned command. Determine one of the **non**- DAGAG root nodes as UDP server and one
as client. Send a UDP packet from the client to the server (don't forget to look up the IPv6
address!) by the above mentioned `udp send` command. On the sender you will get a notification
once the packet has actually been sent by the link layer. On the receiver it will print
information about the received packet.

### Multi-hop w/ channel hopping
Do the same as above but **without** setting the `OW_RIOT_SINGLE_CHANNEL` flag.
