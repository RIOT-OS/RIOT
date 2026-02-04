# LWIP IPv4 client and server example

This example shows how to send and receive IPv4 UDP datagrams using LWIP
network library.

# Shell commands

The following commands are available:
- `ifconfig`: allows manual IPv4 address configuration.
- `client`: sends UDP datagram to given IPv4 address.

# Usage on BOARD=native

This instruction, using native board, utilize tap interface and allows
communication between RIOT system and host Linux.

In the following sections we describe how to:
- configure tap interface using RIOT `tapsetup` tool.
- send data from the Linux host to the RIOT.
- send data from the RIOT to the Linux host.

## Network configuration

Create `tap0` interface and `tapbr0` using RIOT `tapsetup` tool.
Commands should be executed on the Linux host:

```
$sudo ./dist/tools/tapsetup/tapsetup -c 1
$sudo ip addr add 192.168.100.100/24 dev tapbr0
```

Compile and manually configure IPv4 address on the RIOT.

```
$make all
$./bin/native64/lwip_ipv4.elf tap0
RIOT native interrupts/signals initialized.
TZ not set, setting UTC
RIOT native64 board initialized.
RIOT native hardware initialization complete.

main(): This is RIOT! (Version: 2024.04-devel-2705-g39b6f-examples-lwip_ipv4)

> ifconfig add ET0 192.168.100.11/24
ifconfig add ET0 192.168.100.11/24
> ifconfig
ifconfig
Iface ET0 HWaddr: 8e:9d:7d:a1:fd:cd Link: up State: up
        Link type: wired
        inet addr: 192.168.100.11 mask: 255.255.255.0 gw: 0.0.0.0
>
```

Check connectivity from the Linux host machine to the RIOT:

```
$ping 192.168.100.11
PING 192.168.100.11 (192.168.100.11) 56(84) bytes of data.
64 bytes from 192.168.100.11: icmp_seq=1 ttl=255 time=0.240 ms
64 bytes from 192.168.100.11: icmp_seq=2 ttl=255 time=0.210 ms
...
```

## RIOT LWIP IPv4 server

The server starts automatically on the RIOT:

```
main(): This is RIOT! (Version: 2024.04-devel-3275-gab2302-examples-lwip_ipv4)
{"IPv4 addresses": ["192.168.100.11"]}
Server started.
>
```

Send UDP datagram from the Linux host using `nc` tool:

```
$ nc -u 192.168.100.11 4444
Msg from Linux
Msg from Linux
```

Observe results on the RIOT:

```
main(): This is RIOT! (Version: 2024.04-devel-3275-gab2302-examples-lwip_ipv4)
{"IPv4 addresses": ["192.168.100.11"]}
Server started.
> Received 15 bytes - Msg from Linux

```

## RIOT LWIP IPv4 client

Run server on the Linux host using `nc` tool:

```
$ nc -ul -p 4444
```

Send UDP datagram from the RIOT:

```
> client 192.168.100.100 4444 Msg_from_RIOT
client 192.168.100.100 4444 Msg_from_RIOT
>
```

Observe result on the Linux host:

```
$nc -ul -p 4444
Msg_from_RIOT

```
