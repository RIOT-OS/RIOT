POSIX UDP Client/Server
=======================

This is a basic example how to use UDP as server or client in RIOT. You can send
and receive data through UDP.


To start this example you should create two bridged tap devices.

```
$ cd posix_udp_socket
$ ../../cpu/native/tapsetup.sh create
```

Now you can compile and run two instances of RIOT (connected through these
tap devices).

```
$ make && make term
RIOT native uart0 initialized.
RIOT native interrupts/signals initialized.
LED_GREEN_OFF
LED_RED_ON
RIOT native board initialized.
RIOT native hardware initialization complete.

kernel_init(): This is RIOT! (Version: 2014.01-1010-gfb46-cholin)
kernel_init(): jumping into first task...
UART0 thread started.
uart0_init() [OK]
Initializing transport layer packages. Size of socket_type: 177
>
```

Now you should init the 6lowpan stack:

```
> init
init
Configured IPv6 addresses:
	fe80::ff:fe00:775/128
	ff02::1:0:ff00:775/128
```

To start an UDP server you can use the ``server`` command

```
> server
server
UDP SERVER ON PORT 64288 (THREAD PID: 11)
```

To actually send packets from one RIOT instance to another you have to start
another RIOT instance:

```
$ bin/native/posix-udp-socket.elf tap1
RIOT native uart0 initialized.
RIOT native interrupts/signals initialized.
LED_GREEN_OFF
LED_RED_ON
RIOT native board initialized.
RIOT native hardware initialization complete.

kernel_init(): This is RIOT! (Version: 2014.05-5-g1336-cholin-features/udp_sockets_example)
kernel_init(): jumping into first task...
UART0 thread started.
uart0_init() [OK]
Initializing transport layer packages. Size of socket_type: 177
> init
init
Configured IPv6 addresses:
	fe80::ff:fe00:874/128
	ff02::1:0:ff00:874/128
> server
server
UDP SERVER ON PORT 64288 (THREAD PID: 11)
>
```


You can send between those two RIOT instances data with the "send" command.

```
> send "fe80::ff:fe00:775" "Hello world!"
send "fe80::ff:fe00:775" "Hello world!"
Successful deliverd 61 bytes over UDP to fe80::ff:fe00:775 to 6LoWPAN
```

This should output the following in the other instance

```
> UDP packet received, payload: Hello world!
```

