# CAN on `native`

## RIOT native Prerequisites

For using the can stack on top of socketCAN, available for linux, you need:
- socketCAN (part of kernel starting from 2.6.25)
- install the 32bit version of libsocketcan:
- install `can-utils` `sudo apt install can-utils`

By default `native` will use the `libsocketcan` package to download and compile
`libsocketcan` from source.

Alternatively, you can compile from source:

```shell
wget http://www.pengutronix.de/software/libsocketcan/download/libsocketcan-0.0.10.tar.bz2
sudo tar xvjf libsocketcan-0.0.10.tar.bz2
sudo rm -rf libsocketcan-0.0.10.tar.bz2
sudo cd libsocketcan-0.0.10
sudo ./configure
compile in 32bits
./configure --build=i686-pc-linux-gnu "CFLAGS=-m32" "CXXFLAG
sudo make
sudo make install
sudo ldconfig /usr/local/lib
```

## Connecting RIOT native and Linux Host through socketCAN

The default native configuration defines two virtual can interfaces: `vcan0`
and `vcan1`. By default a single one is enabled, this can be changed through
the `CAN_DLL_NUMOFF` flag.

First, make sure you've compiled the application by calling `make`.

Create the default `vcan` interface

```shell
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set vcan0 up
```

Now start the application by invoking `make term`, this should automatically
connect to the `vcan0` interface.

## Connecting two RIOT native instances

```
 ___   ___                   _______
| _ | | _ |                 | _   _ |
||A|| ||B||                 ||A| |B||
|___| |___|                 |_______|
  |     |                       |
-----------(1)- CAN bus -(2)---------
```

### Through the same vcan interface (1)

First, make sure you've compiled the application by calling `make`.

Create a `vcan` interface:

```shell
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set vcan0 up
```

Now start the application by invoking `make term`, this should automatically
connect to the `vcan0` interface, repeat for the second native instance.

Since they are both connected to the same `vcan` they will seem like they are
on the same CAN-BUS and so will be able to receive messages from each other.

### Through different vcan interfaces with `can-gw` (2)

Alternatively you can connect two distinct `vcan` interfaces through the use of
the `can-gw` module,

First, make sure you've compiled the application by calling `make`.

Create two `vcan` interfaces

```shell
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link add dev vcan1 type vcan
sudo ip link set vcan0 up
sudo ip link set vcan1 up
```

Connect them with can-gw

```shell
sudo modprobe can-gw
sudo cangw -A -s vcan0 -d vcan1 -e
sudo cangw -A -s vcan1 -d vcan0 -e
```

Now start the application by invoking `make term`, this should automatically
connect to the `vcan0` interface. For the second native interface specify the
vcan interface through:

```shell
# first instance
make term
# second instance
VCAN_IFNAME=vcan1 make term
```

## Enabling multiple vcan interfaces

By default a single can interface is enabled, a second one can be added
by setting `CAN_DLL_NUMOF=2`, e.g. `CFLAGS=-DCAN_DLL_NUMOF=2`. These two
interfaces will not be connected by default but look like if they where on
different Buses.

To configure which `vcan` interface each native CAN device should use then
`-n` or `--can` flags can be used (see [cpu/native/startup.c](../../cpu/native/startup.c))

```c
"    -n <ifnum>:<ifname>, --can <ifnum>:<ifname>\n"
"        specify CAN interface <ifname> to use for CAN device #<ifnum>\n"
```

These should be added to `PERIPH_CAN_FLAGS`, eg:

```shell
CFLAGS=-DCAN_DLL_NUMOF=2 BOARD=native PERIPH_CAN_FLAGS="--can 0:vcan1 --can 1:vcan2" make flash term
```

## Linux CAN basic commands

To send or receive bytes from the interface `can-utils` can be used:

- send raw CAN frames: by using `cansend` or altenatively `cangen` to send random can messages:

```shell
$ cansend <interface> <can_id>:<hexbytes>
# e.g.:
$ cansend vcan0 001#1234ABCD
```

```shell
$ cangen <interface> -v -n <n>
# e.g.: to send 5 messages through vcan0 with verbose output
$ cangen vcan0 -v -n 5
```

- receive raw CAN frames: by scanning the CANbus with `candump`:

```shell
$ candump <interface>
# e.g.:
$ candump vcan0
```

- send ISO-TP datagrams: by using the `isotpsend` command to send hex bytes

```shell
echo XX XX XX XX | isotpsend -s <src-id> -d <dst-id> <interface>
# e.g.: Send an ISO-TP datagram, source id 700, dest id 708, data 00 11 22 33 aa bb cc dd:
echo 00 11 22 33 aa bb cc dd | isotpsend -s 700 -d 708 vcan0
```

- receive ISO-TP datagrams: by using `isotprecv` command

```shell
$ isotprecb -s <src-id> -d <dst-id> <interface>
# e.g.:
$ isotprecv -s 708 -d 700 vcan0
```
