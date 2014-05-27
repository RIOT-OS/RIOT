UDP Client/Server
================

This is a basic example how to use UDP as server or client in RIOT. You can send
and receive data through UDP.


To start this example you should create two bridged tap devices.

```
$ cd udp-sockets
$ ../../cpu/native/tapsetup.sh
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
UDP SERVER ON PORT 64288 (THREAD PID: 11)
Already configured IPv6-Addresses:
    fe80::ff:fe00:6b26/128
    ff02::1:0:ff00:6b26/128
>
```

To start another RIOT instance you have to specify the tap device

```
$ bin/native/udp-sockets.elf tap1
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
UDP SERVER ON PORT 64288 (THREAD PID: 11)
Already configured IPv6-Addresses:
    fe80::ff:fe00:6af4/128
    ff02::1:0:ff00:6af4/128
>
```


As you can see every instance starts automatically an UDP server. You can send
between those two RIOT instances data with the "send" command.

```
> send "fe80::ff:fe00:6b26" "Hello world!"
send "fe80::ff:fe00:6b26" "Hello world!"
Successful deliverd 61 bytes over UDP to fe80::ff:fe00:6b26 to 6LoWPAN
```

This should output the following in the other instance

```
> UDP packet received, payload: Hello world!
```

