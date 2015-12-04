# gnrc_tftp example

## Connecting RIOT native and the Linux host

> **Note:** RIOT does not support IPv4, so you need to stick to IPv6 anytime. To establish a connection between RIOT and the Linux host,
you will need `tftp` (with IPv6 support). On Ubuntu and Debian you would need the package `tftp-hpa` for TFTP client and `tftpd-hpa` for TFTP server.
Be aware that many programs require you to add an option such as -6 to tell them to use IPv6, otherwise they
will fail. If you're using a _Raspberry Pi_, run `sudo modprobe ipv6` before trying this example, because raspbian does not load the
IPv6 module automatically.
On some systems (openSUSE for example), the _firewall_ may interfere, and prevent some packets to arrive at the application (they will
however show up in Wireshark, which can be confusing). So be sure to adjust your firewall rules, or turn it off (who needs security anyway).

First, create a tap interface (to which RIOT will connect) and a bridge (to which Linux will connect) from the RIOT main directory run:

    ./dist/tools/tapsetup/tapsetup -c 1

Now you can start the `gnrc_tftp` example by invoking `make term`. This should automatically connect to the `tap0` interface. If
this doesn't work for some reason, run `make` without any arguments, and then run the binary manually like so (assuming you are in the `examples/gnrc_tftp` directory):

To verify that there is connectivity between RIOT and Linux, go to the RIOT console and run `ifconfig`:

    > ifconfig
	Iface  6   HWaddr: 7e:ed:d2:ee:e1:07
           MTU:1280
           Source address length: 6
           Link type: wired
           inet6 addr: ff02::1/128  scope: local [multicast]
           inet6 addr: fe80::7ced:d2ff:feee:e107/64  scope: local
           inet6 addr: ff02::1:ffee:e107/128  scope: local [multicast]


Copy the [link-local address](https://en.wikipedia.org/wiki/Link-local_address) of the RIOT node (prefixed with `fe80`) and try to ping it **from the Linux node**:

    ping6 fe80::7ced:d2ff:feee:e107%tapbr0

Note that the interface on which to send the ping needs to be appended to the IPv6 address, `%tapbr0` in the above example. When talking to the RIOT node, you always want to send to/receive from the `tapbr0` interface.

If the pings succeed you can go on to send UDP packets. To do that, first start a UDP server on the RIOT node:

    > tftps start
    tftp_server: Starting TFTP service at port 69

Now, on the Linux host, you can run tftp to connect with RIOT's TFTP server:

    $ tftp -v -6 fe80::7ced:d2ff:feee:e107%tapbr0 -c get welcome.txt

The output will be something like:

	Connected to fe80::7ced:d2ff:feee:e107%tapbr0 (fe80::7ced:d2ff:feee:e107), port 69
	getting from fe80::7ced:d2ff:feee:e107%tapbr0:welcome.txt to welcome.txt [netascii]
	Received 94 bytes in 0.0 seconds [113425 bit/s]

The `-6` option is necessary to tell tftp to use IPv6 only and the `-v` is to tell the tftp client to be verbose.

You should now see that the TFTP messages are received on the RIOT side. Opening a TFTP server on the Linux side is also possible. To do that, write down the IP address of the host (run on Linux):

    ifconfig tapbr0
    tapbr0    Link encap:Ethernet  HWaddr 0e:bc:0f:49:7f:e4
              inet6 addr: fe80::cbc:fff:fe49:7fe4/64 Scope:Link
              UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
              RX packets:22 errors:0 dropped:0 overruns:0 frame:0
              TX packets:53 errors:0 dropped:0 overruns:0 carrier:0
              collisions:0 txqueuelen:0
              RX bytes:1928 (1.9 KB)  TX bytes:6217 (6.2 KB)

And start the tftp server on Linux:

	$ sudo /etc/init.d/tftpd-hpa stop && mkdir -p tftpserv && echo "hello world" > tftpserv/welcome.txt && sudo in.tftpd -vvv -L -6 -c -s -u ${USER} ./tftpserv

Now, on the RIOT side, send a UDP packet using:

    tftpc get welcome.txt octet 1 fe80::cbc:fff:fe49:7fe4

You will get output that looks like this:

    tftp_client: bin read welcome.txt:12

    -- CLIENT DATA --
    hello worl
    -- CLIENT DATA --

    -- CLIENT DATA --
    d

    -- CLIENT DATA --
    tftp_client: SUCCESS: (null)
