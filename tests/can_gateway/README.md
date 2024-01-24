## test/can_gateway test
This test application shows the use of the CAN to Ethernet or SOME/IP gateway.
The gateway can be operated in two different modes, so that it acts either as
an interface between CAN and Ethernet or IPv6 based networks or as an interface
between CAN and IPv4 based networks. The application provides a set of shell
commands that are used to configure the gateway.
The gateway itself is not able to generate messages independently. The gateway
sends messages only if it has received messages via CAN, Ethernet or SOME/IP
(IPv4 or IPv6) before.

## Usage
For the gateway to work correctly, some compile time and runtime configurations
must be made. First of all, the respective operating mode must be selected
using the parameter `GATEWAY_OPERATING_MODE` (default CAN - Ethernet / SOME/IP
(IPv6)). In addition, the CAN transceiver used must be selected via the
parameter `TRX_TO_BUILD`. Furthermore, the necessary pins of the used CAN
transceiver, the server port, the CAN bit rate and the CAN sample point must be
set in the Makefile.

In addition, the gateway needs forwarding information to reliably forward
received messages to the respective target interfaces. Therefore it is possible
to provide the gateway with list entries containing the necessary information
for the respective CAN identifiers:
```
can_gateway add_id [CAN ID] [Dst Port] [Dst IP] [Eth send] [Dst CAN Interface] [Src CAN Interface]
```
If the CAN bit rate and the sample point for an interface should be changed:
```
can_gateway set_bitrate [iface] [bitrate] [sample_point]
```
When the CAN receive threads should start to receive CAN messages:
```
can_gateway start_can_recv
```
Close can connection sockets:
```
can_gateway close
```
Delete CAN ID-list:
```
can_gateway delete_list
```
The shell commands come with online help. Call `help` to see which commands
exist and what they do.

These configurations and commands can also be executed using the configuration
tool `config_gateway.py`. `configIPv4.json` and `configIPv6.json` contain one
CAN identifier list for each operating mode, which can be passed on to the
gateway via the configuration tool.
To see all possible configuration tool options:
```
python config_gateway.py -h
```
