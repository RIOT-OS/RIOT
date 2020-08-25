# Candev abstraction test

## About

This application is a test for using the candev abstraction directly.
Use this if you want to use a single CAN driver and thus don't need the CAN-DLL layer.

You can select the driver you want to use by redefining the CAN_DRIVER variable in the Makefile. Alternatively you can pass it to the make command.
The application will automatically adapt its initialization procedure to the selected driver.
By default the mcp2515 driver is used.
NOTE: When building for native, use PERIPH_CAN.

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

sudo ldconfig /usr/local/lib
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

## Usage

### Sending

Messages can be sent over the CAN-bus through the `send` command. Optionally, up to 8 bytes can be passed as arguments (in decimal form). If no arguments are passed it will default to sending AB CD EF (hex).

```
send <bytes>
```

When running the app native on linux, the sent bytes can be seen by scanning the CANbus with candump:

```
$ candump vcan0
```

### Receiving

The test-app is always listening for incoming CAN messages. They will be stored asynchronously in a buffer and can be requested by means of the `receive` command. Optionally, an argument n can be passed to receive n messages in a row.

```
receive <n>
```

If more messages are requested than are available in the buffer, the receive function will block until new data is available.

When running the app native on linux, data can be sent with `cansend`:

```
$ cansend <interface> <can_id>:<hexbytes>
```

e.g.:

```
$ cansend vcan0 001:1234ABCD
```

An alternative is to use `cangen` to generate a number of random can messages:

```
$ cangen <interface> -v -n <n>
```

e.g.:

```
$ cangen vcan0 -v -n 5
```

will send 5 can messages to vcan0 with verbose output.
