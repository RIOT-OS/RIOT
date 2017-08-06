tests/conn_can
================
Demo application for the CAN stack with conn_can interface.


Native prerequisites
============
For using the can stack on top of socketCAN, available for linux, you need:
- socketCAN (part of kernel starting from 2.6.25)
- install  the 32bit version of libsocketcan:

if you're on a 64bit system:
```
sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install libsocketcan-dev:i386
```
On 32 bit you can just do the following:
```
sudo apt-get install libsocketcan-dev
```

Alternatively, you can compile from source:
```
wget http://www.pengutronix.de/software/libsocketcan/download/libsocketcan-0.0.10.tar.bz2

$ sudo tar xvjf libsocketcan-0.0.10.tar.bz2

$ sudo rm -rf libsocketcan-0.0.10.tar.bz2

$ sudo cd libsocketcan-0.0.10

$ sudo ./configure

compile in 32bits

./configure --build=i686-pc-linux-gnu "CFLAGS=-m32" "CXXFLAG

$ sudo make

$ sudo make install


sudo ldconfig
/usr/local/lib
```

The default native configuration defines two virtual can ifaces to be used.
Before running this test on native, you should create those:

```
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link add dev vcan1 type vcan
sudo ip link set vcan0 up
sudo ip link set vcan1 up
```

Usage
=====

Build, flash and start the application:
```
export BOARD=your_board
make
make flash
make term
```

The CAN interfaces are registered at startup to the dll. The list of registered
interfaces and their RIOT names can be retrieved with:
```
can list
```

To send a raw CAN frame, id 0x100 with 2 bytes of data 01 02 on interface 0:
```
can send 0 100 01 02
```

Two threads are launched to enable receiving frames. To receive raw CAN frames,
ids 0x100 and 0x500 with thread 0 on interface 1, with 10s timeout:
```
can recv 1 0 10000000 100 500
```

A connection can be closed with its thread id, for instance:
```
can close 0
```


To send an ISO-TP datagram, first bind a connection with one of the threads,
source id 700, dest id 708, thread 1 and interface 0:
```
can bind_isotp 0 1 700 708
```
Then send the data 01 02 03 04 05 0a 0b 0c:
```
can send_isotp 1 01 02 03 04 05 0a 0b 0c
```

To receive from an ISO-TP channel, it must be bound, then with the previous channel,
and 10s timeout:
```
can recv_isotp 1 10000000
```

An ISO-TP channel can be closed with:
```
can close_isotp 1
```

You can also set a bitrate (this won't work on native with vcan, only with real
interfaces, but then root access are needed), for instance 250000 bit/s with
sampling point 87.5%:
```
can set_bitrate 250000 875
```

Linux CAN basic commands
========================

Once the interfaces are set up, can-utils commands provide a way to send and receive
raw CAN frames and ISO-TP datagrams.

For ISO-TP, an experimental module for linux can be found [here](https://github.com/hartkopp/can-isotp).
It needs to be loaded before trying to use ISO-TP protocol.

Here are some basics examples.

Send a raw CAN frame, id 0x100, data 00 11 22:
```
cansend vcan0 100#001122
```

Dump the traffic on a CAN interface:
```
candump vcan0
```

Send an ISO-TP datagram, source id 700, dest id 708, data 00 11 22 33 aa bb cc dd:
```
echo 00 11 22 33 aa bb cc dd | isotpsend -s 700 -d 708 vcan0
```

Receive ISO-TP datagram:
```
isotprecv -s 708 -d 700 vcan0
```

Please read commands help for more details on usage.
