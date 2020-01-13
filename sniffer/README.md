About
=====

This application is built to run together with the script `./tools/sniffer.py`
as a sniffer for (wireless) data traffic. This application works with any board
with any network device that supports the gnrc network stack (or precisely the
gnrc parts up to the link-layer). Further the network device (and its driver)
needs to support promiscuous and raw mode for usable output. Finally the board
needs to include auto-initialization code for the targeted network device.


Usage
=====

Compile and flash this application to the board of your choice. You can check
if everything on the RIOT side works by connecting to the board via UART and by
checking with `ifconfig` if a network device is available. Also note the
interface number for the following commands. Then you can check with
`ifconfig <iface> promisc` if promiscuous mode is supported and with
`ifconfig <iface> raw` if raw mode is supported by the driver/network device.

For further information on setting up the host part, see `./tools/README.md`.
