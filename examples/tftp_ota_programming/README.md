# tftp_ota_programming example

## Connecting RIOT board and the Linux host

> **Note:** RIOT does not support IPv4, so you need to stick to IPv6 anytime. 
To establish a connection between RIOT and the Linux host, you will need `tftp` 
(with IPv6 support). On Ubuntu and Debian you would need the package `tftp-hpa` 
for TFTP client and `tftpd-hpa` for TFTP server.
Be aware that many programs require you to add an option such as -6 to tell 
them to use IPv6, otherwise they will fail. If you're using a _Raspberry Pi_, 
run `sudo modprobe ipv6` before trying this example, because raspbian does not 
load the IPv6 module automatically.
On some systems (openSUSE for example), the _firewall_ may interfere, and 
prevent some packets to arrive at the application (they will however show up 
in Wireshark, which can be confusing). So be sure to adjust your firewall 
rules, or turn it off (who needs security anyway).

First, you need to setup a border-router which will serve the binary image.
This can be done by your means or by using the RIOT border-router example.
If the latter is the case, by doing `ifconfig` for this example, you should 
have:

```cosole
INFO #  ifconfig
INFO # Iface  7   HWaddr: 0f:36  Channel: 26  Page: 0  NID: 0x23
INFO #            Long HWaddr: 15:11:6b:10:65:f8:8f:36 
INFO #            TX-Power: 0dBm  State: IDLE  max. Retrans.: 3  CSMA Retries: 4 
INFO #            ACK_REQ  CSMA  MTU:1280  HL:64  6LO  RTR  RTR_ADV  IPHC  
INFO #            Source address length: 8
INFO #            Link type: wireless
INFO #            inet6 addr: ff02::1/128  scope: local [multicast]
INFO #            inet6 addr: fe80::1711:6b10:65f8:8f36/64  scope: local
INFO #            inet6 addr: ff02::1:fff8:8f36/128  scope: local [multicast]
INFO #            inet6 addr: 2001:db8::1711:6b10:65f8:8f36/64  scope: global
INFO #            inet6 addr: ff02::2/128  scope: local [multicast]
```

If you have an address starting by `2001:db8` it means your configuration is
successful, since the border-router just gave you a global address.

On the Linux side, you should have `fd00:dead:beef::1/128` as a global address 
in the lo interface:

```console
lo        Link encap:Local Loopback  
          inet addr:127.0.0.1  Mask:255.0.0.0
          inet6 addr: ::1/128 Scope:Host
          inet6 addr: fd00:dead:beef::1/128 Scope:Global
          UP LOOPBACK RUNNING  MTU:65536  Metric:1
          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
          TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1 
          RX bytes:0 (0.0 B)  TX bytes:0 (0.0 B)
```

To configure your tftp server on linux, you should first stop the tftpd-hpa 
server:

```console
$ sudo /etc/init.d/tftpd-hpa stop
```

Then, create a folder where you'll put the firmwares:

```console
$ mkdir tftpserv
```

And copy to it the binaries you want to download.

To start the server:

```console
$ sudo in.tftpd -vvv -L -6 -c -s -u $USER ./tftpserv
```

## Download the image on RIOT
On the RIOT side, you can start the downloading by typing:

```console
> tftpc get application-slot2-APPID-VERSION.bin octet 1 fd00:dead:beef::1
```

This should trigger the downloading and writing into the internal flash.
It will download by pieces of 45 bytes, then accumulate `FLASHPAGE_SIZE` bytes
(2048 on stm32f103re mcu) in RAM which will be dumped to flash.

## Booting the slot 2 image
When the image is successfully downloaded, the device can be rebooted. If the
interactive bootloader is chosen, the image can be verified by doing:

```console
> verify 2
```

Which will perfom a sha256 check on the image and compare it to the one in the
metadata.

To boot the image:

```console
> jump 2
```

Which will verify and boot the image on slot 2.

If the normal bootloader is selected, it will choose the newest image and boot
it, if the APPIDs are the same on both slot 1 and slot 2 images.
