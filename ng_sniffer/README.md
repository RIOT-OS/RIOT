About
=====

This application is build to run together with the script `RIOTBASE/dist/tools/ng_sniffer/ng_sniffer.py` as sniffer for (wireless) data traffic. This application works with any board with any network device that supports the gnrc network stack (or precisely the gnrc parts up to the link-layer). Further the network device (and it's driver) needs to support promiscuous and raw mode for usable output. Finally the board needs to include auto-initialization code for the targeted network device.


Usage
=====

Compile and flash this application to the board of your choice. You can check if everything on the RIOT side works by connecting to the board via UART and by checking with `ifconfig` if a network device is available. Further you can check with `ifconfig 4 promisc` if promiscuous mode is supported and with `ifconfig 4 raw` if raw mode is supported by the driver/network device.

For further information on setting up the host part, see `RIOTBASE/dist/tools/ng_snifffer/README.md`.
