# lwIP test application

This application provides an example of how to use lwIP and provides test for over-the-air
interaction between two lwIP nodes.

## Testing on `native`

### IPv6

For IPv6 on `native` there is a test script. It requires for a TAP bridge to be set up between two
interfaces. For that run

```console
$ sudo dist/tools/tapsetup/tapsetup
```

Then just run it with the usual test command.

```
$ make flash test
```

### IPv4

For IPv4 a DHCP server is needed. If not already done for the IPv6 tests, please set up a TAP bridge
now. Then add an IPv4 address to it, e.g.,

```console
$ sudo ip addr add 10.233.68.1/24 dev tapbr0
```

Then configure a DHCP server (e.g. [`dhcpd`](https://wiki.archlinux.org/title/Dhcpd))

```
# comment in if you need DNS and have a name server available
# option domain-name-servers X.X.X.X X.X.X.Y;
option subnet-mask 255.255.255.0;
option routers 10.233.68.1;
subnet 10.233.68.0 netmask 255.255.255.0 {
  range 10.233.68.166 10.233.68.206;
}
```

Then, you can use the `ip`, `udp`, and `tcp` commands to start servers and connect to them from
clients.

We use `LWIP_IPV6=0` in the following, but dual-stack support, with both `LWIP_IPV4=1` and
`LWIP_IPV6=1` is also possible.

## 6LoWPAN

If `<board>` is supported, just running the following should enable 6LoWPAN.

```console
$ BOARD="<board>" make flash test
```

## Running the tests

We use `native` ports below, but of course these can be interchanged with actual serial ports and
`DEBUG_ADAPTER_ID`s to identify an actual board.

#### `ip`

##### Server

```console
$ LWIP_IPV4=1 LWIP_IPV6=0 PORT=tap0 make -j flash term
[...]
> ifconfig
ifconfig
Iface ET0 HWaddr: da:27:1d:a8:64:24 Link: up State: up
        Link type: wired
        inet addr: 10.233.68.166 mask: 255.255.255.0 gw: 10.233.68.1
> ip server start 254
Success: started IP server on protocol 254
> Received IP data from [10.233.68.167]:
00000000  AB  CD  EF
```

##### Client

```console
$ LWIP_IPV4=1 LWIP_IPV6=0 PORT=tap1 make term
[...]
> ip send 10.233.68.166 254 abcdef
ip send 10.233.68.166 254 abcdef
Success: send 3 byte over IPv6 to 10.233.68.166 (next header: 254)
```

#### `udp`

`make` commands are the same as for `ip`.

##### Server

```console
> udp server start 1337
udp server start 1337
Success: started UDP server on port 1337
Received UDP data from 10.233.68.167:49154
00000000  12  34  56  78  90
```

##### Client

```console
> udp send 10.233.68.166:1337 123456789
udp send 10.233.68.166:1337 123456789
Success: send 5 byte over UDP to 10.233.68.166:1337
```

#### `tcp`

`make` commands are the same as for `ip`.

##### Server

```console
> tcp server start 1337
tcp server start 1337
Success: started TCP server on port 1337
TCP client 10.233.68.167:49153 connected
Received TCP data from 10.233.68.167:49153
00000000  76  54  32  10
TCP connection to 10.233.68.167:49153 reset
```

##### Client

```console
> tcp connect 10.233.68.166:1337
tcp connect 10.233.68.166:1337
> tcp send 7654321
tcp send 7654321
Success: send 4 byte over TCP to server
> tcp disconnect
tcp disconnect
```
