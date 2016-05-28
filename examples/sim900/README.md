examples/sim900_gnrc_ppp
============

This application is a showcase for RIOT's GNRC PPP through a SIM900 gprs modem. Although is was made for SIM900, it
should work with other gsm devices as well.

The application introduces a command line interface to control the dialing up process, status of GNRC PPP and setting
APN and tunnel options. It's invoked using the `shell` command interpreter.

Due to the lack of implementation of IPv6 in most mobile operators GNRC PPP works with UDP encapsulation over IPv4. At
least a
computer with a public IPv4 address is required for running this example. This computer will act as a tunnel server for
forwarding IPv6 traffic over an IPv4 UDP tunnel.

In case the server doesn't have an IPv6 address it's possible to use a transition mechanism like 6to4. You can setup one
in http://www.tunnelbroker.net. You need to enable the /48 prefix for making it work, otherwise packets will not reach
the node.


Running the example
=====
* First you must install scapy python library in the server. The, launch start_udp_tunnel.py. The service will be
  running in UDP port 9876.
  NOTE: The script is a simple udp encapsulator for IPv6. It doesn't come with authentication yet. In fact this configuration is
  an open proxy, so use it with caution.

* From RIOT side, the first step is to set APN settings of GPRS network. This can be done through ppp cli with:
	ppp apn <interface_number> <apn_name> [<apn_user> [<apn_pass>]]

* Then, the tunnel is configured with the `tunnel` command of ppp cli:
	ppp tunnel <interface_number> <tunnel_ipv4_addres> <tunnel_udp_port>

* It's necessary to assign an IPv6 address with the same prefix to the ppp interface. You can do that with:
	ifconfig add <interface_number> <IP_address>

* Ensure that the routing tables are configured in a way that packets can go through the PPP interface

* Call `ppp dial_up <interface_number>`. You can check the status of GNRC PPP with the command `ppp status
  <interface_numner>`. IPv6 data can be transfered when this command shows "IPv6 Ready".

* For closing the PPP connection, call `ppp disconnect <interface_number>`


Known Issues
===========
* SIM900 can't connect to GSM network if the SIM card is not in home network. Virtual operators or foreign sim cards
  won't work.

TODO
====
 * Add authentication to UDP tunnel.
 * Implementation of IPCPv6, for supporting native IPv6.

PPP details
===========
The current implementation of GNRC PPP supports the following options for each protocol:
* LCP: Maximum Received Unit, Async Control Character Map
* Authentication: Only PAP, should be sufficient for all mobile companies.
* IPCP: IP address option.

GNRC PPP will try to reconnect if communication is lost or if out of sync with the modem.

