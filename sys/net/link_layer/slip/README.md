SLIP network interface
======================

This module provides access to the [http://tools.ietf.org/html/rfc1055](Serial Line Internet Protocol (SLIP)).
It allows you to send IP and IPv6 packages over the U(S)ART. For this you
have to initialize your serial device as a network device. In Linux (and Mac OS X) [http://playground.arduino.cc/Code/SerialIP#Connecting_Linux](this is easy):

```bash
sudo modprobe slip  # you do not need this line in Mac OS X
sudo slattach -s <your UART's baudrate, typically 115200> -p slip <your serial device, typically /dev/ttyUSB0>
sudo ifconfig sl0 mtu 1280  # or smaller, for e.g. MSP-430 boards
sudo ifconfig sl0 add fe80::/64
sudo ifconfig sl0 add abcd::/64 # replace with an IPv6 prefix of your choice
sudo ifconfig sl0 up
```

If your RIOT board is configured as an IPv6 router, the rest should happen automatically.
