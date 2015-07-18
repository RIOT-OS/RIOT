# ng_networking example

## Connecting RIOT native and the Linux host

To establish a connection between RIOT and the Linux host, the following dependencies are required:

* netcat (with IPv6 support)

Ubuntu 14.04 comes with netcat IPv6 support pre-installed. On Debian it's available in the package `netcat-openbsd`.

After installing, create a tap interface (to which RIOT will connect) and a bridge (to which Linux will connect):

    sudo ip tuntap add tap0 mode tap user ${USER}
    sudo ip link set tap0 up

Now you can start the `ng_networking` example by invoking `make term`. This should automatically connect to the `tap0` interface.

To verify that there is connectivity between RIOT and Linux, go to the RIOT console and run `ifconfig`:

    > ifconfig
    Iface  7   HWaddr: ce:f5:e1:c5:f7:5a
    inet6 addr: ff02::1/128  scope: local [multicast]
    inet6 addr: fe80::ccf5:e1ff:fec5:f75a/64  scope: local
    inet6 addr: ff02::1:ffc5:f75a/128  scope: local [multicast]

Copy the link-local address of the RIOT node (prefixed with `fe80`) and try to ping it **from the Linux node**:

    ping6 fe80::ccf5:e1ff:fec5:f75a%tap0

Note that the interface on which to send the ping needs to be appended to the IPv6 address, `%tap0` in the above example. When talking to the RIOT node, you always want to send to/receive from the `tap0` interface.

If the pings succeed you can go on to send UDP packets. To do that, first start a UDP server on the RIOT node:

    > udp server start 8808
    Success: started UDP server on port 8808

Now, on the Linux host, you can run netcat to connect with RIOT's UDP server:

    nc -6uv fe80::ccf5:e1ff:fec5:f75a%tap0 8808

You should now see that UDP messages are received on the RIOT side. Opening a UDP server on the Linux side is also possible. Do do that, write down the IP address of the host (run on Linux):

    ifconfig tap0
    tap0     Link encap:Ethernet  HWaddr ce:f5:e1:c5:f7:59
            inet6 addr: fe80::4049:5fff:fe17:b3ae/64 Scope:Link
            UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
            RX packets:6 errors:0 dropped:0 overruns:0 frame:0
            TX packets:36 errors:0 dropped:0 overruns:0 carrier:0
            collisions:0 txqueuelen:0
            RX bytes:488 (488.0 B)  TX bytes:3517 (3.5 KB)

Then open a UDP server on Linux:

    nc -6ul 8808

Now, on the RIOT side, send a UDP packet using:

    udp send fe80::4049:5fff:fe17:b3ae 8808 testmessage
