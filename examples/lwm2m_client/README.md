LWM2M Client Example
================
This application is a showcase for RIOT's LWM2M protocol support. At the moment it supports only some basic features.

To test the client we will use the LESHAN LWM2M server.


Setup
=====

First create the tap interface used by the client and configure it accordingly:
```
sudo openvpn --mktun --dev tap0
sudo ip a a 2001:db8::1/64 dev tap0
sudo ip link set up dev tap0
```

Now we have to start the client and configure it:
```
make -B all
PORT=tap0 make term
ifconfig <if> add 2001:db8::2/64
```

In the RIOT shell the '<if>' can be determined by calling the 'ifconfig' command and looking at the number next to 'Iface'

Now we need to start the server, in a different terminal go into the 'leshan-server' directory and start the server: 'make all'
The server interface can be accessed through the browser at '[::1]:8080'

To connect to the server, from the RIOT shell:
```
lm init <name> 2001:db8::1 <port>
```
