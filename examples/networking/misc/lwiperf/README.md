# lwIP iPerf benchmark

This application exposes the lwIP iPerf server and client functionality to
perform network throughput benchmarks between a node and another host.

Note: this example needs iPerf version 2 to work, not iPerf version 3. It
currently only supports TCP.

## Building

The default configuration compiles for the `native` target with only IPv6
support enabled.

IPv4 support can be enabled simultaneously by providing the `LWIP_IPV4` flag,
e.g. `make LWIP_IPV4=1`. However, when starting a server, it will only listen
on the IPv4 address.

Refer to the `Makefile` for more details.

## Usage

First make sure that the network interface is properly configured, and has an
address. Then, to interact with the lwIP iPerf app, the shell exposes three
commands:

* `abort` - stop the currently running server or client
* `client <host>` - start an iPerf client, connecting to the specified host
* `server` - start an iPerf server

The server and client will run in the background.

## Demonstration

This is a demonstration of the client running on a node, connecting to a server
running on the host.

When testing this on a `native` target, refer to the
`examples/networking/misc/lwip_ipv4` instructions on how to set-up a local
network.

First, check if the network interface is up and has an address. This can be
done with the `ifconfig` command:

```
> ifconfig
Iface ET0 HWaddr: d2:16:6c:09:78:35 Link: up State: up
        Link type: wired
        inet6 addr: fe80:0:0:0:d016:6cff:fe09:7835 scope: link state: valid preferred
```

When using IPv4 without DHCP (the default), the address can be configured
manually. For example:

```
> ifconfig add ET0 192.168.100.11/24

> ifconfig
Iface ET0 HWaddr: d2:16:6c:09:78:35 Link: up State: up
        Link type: wired
        inet addr: 192.168.100.11 mask: 255.255.255.0 gw: 0.0.0.0
```

On the host, start an iPerf server (`-V` is for IPv6):

```
$ iperf -V -s
```

Find the host IPv6 address (this will be machine-specific):

```
$ ip addr
...
3: tapbr0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UP group default qlen 1000
    link/ether 0a:04:9b:b4:23:de brd ff:ff:ff:ff:ff:ff
    inet 192.168.100.100/24 scope global tapbr0
       valid_lft forever preferred_lft forever
    inet6 fe80::804:9bff:feb4:23de/64 scope link proto kernel_ll
       valid_lft forever preferred_lft forever
...
```

Finally, run the client on the node, connecting to the host:

```
> client fe80::804:9bff:feb4:23de
TCP client connecting to fe80::804:9bff:feb4:23de port 5001
```

The node will connect to the server and print a report afterwards:

```
--- Results ---
Status      : TCP client done
Remote      : FE80::804:9BFF:FEB4:23DE:5001
Transferred : 309149816 bytes
Duration    : 10000 ms
Bandwidth   : 247312 kbit/s
--------------
```

The host output will look as follows:

```
------------------------------------------------------------
Server listening on TCP port 5001
TCP window size:  128 KByte (default)
------------------------------------------------------------
[  1] local fe80::804:9bff:feb4:23de port 5001 connected with fe80::d016:6cff:fe09:7835 port 49153
[ ID] Interval       Transfer     Bandwidth
[  1] 0.0000-9.9988 sec   295 MBytes   247 Mbits/sec
```
