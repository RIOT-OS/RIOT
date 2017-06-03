About
=====

This application is designed to run together with the script
`tools/plot_rssi.py` to monitor energy levels on all
available wireless channels. This application works with any board with a
network device that supports the gnrc network stack (or precisely the gnrc parts
up to the link-layer). Further, in order to get any measurements from the
device, the network device and its driver needs to support the netopt options
for executing a manual CCA and getting the last ED level
(NETOPT_IS_CHANNEL_CLR, and NETOPT_LAST_ED_LEVEL, respectively). Finally the
board needs to include auto-initialization code for the targeted network device.

Usage
=====

Compile and flash this application to the board of your choice. You can check if
everything on the RIOT side works by connecting to the board via UART and
looking at the output.

Data format
===========

The format of the data on the UART is:

```
[interface_number, timestamp, count] channel: ed_level, channel: ed_level, ...
```

where interface_number is the interface number (in RIOT's `ifconfig`), timestamp
is the time since power on in microseconds, count is the number of measurements
that were averaged, channel is a channel number, ed_level is the average energy
level for that channel since the last update.

For further information on setting up the host part, see
`tools/README.md`.
