# Overview

This folder contains a test application for RIOT's sock-based DNS over DTLS
client.

# How to test with native

## Setting up a tap interface

1. Create a tap interface with a valid IPv6 address

   ```console
   $ sudo ip tuntap add dev tap0 mode tap user $(id -u -n)
   $ sudo ip a a 2001:db8::1/64 dev tap0
   $ sudo ip link set up dev tap0
   $ ip addr show dev tap0
   4: tap0: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc fq_codel state DOWN group default qlen 1000
    link/ether e2:bc:7d:6b:8b:08 brd ff:ff:ff:ff:ff:ff
    inet6 2001:db8::1/64 scope global
       valid_lft forever preferred_lft forever
    inet6 fe80::e0bc:7dff:fe6b:8b08/64 scope link
       valid_lft forever preferred_lft forever
   ```

   **Note down** the link-local address.

2. Run the test application

   ```console
   $ make flash -j term
   ```

   And copy the link-local address using `ifconfig`:

   ```
   > ifconfig
   ifconfig
   Iface  5  HWaddr: E2:BC:7D:6B:8B:09
             L2-PDU:1500  MTU:1500  HL:64  Source address length: 6
             Link type: wired
             inet6 addr: fe80::e0bc:7dff:fe6b:8b09  scope: link  VAL
             inet6 group: ff02::1
             inet6 group: ff02::1:ff6b:8b09

   ```

3. Use it to configure a route to the `native` device (replace `2001:db8::/64` if you used a
   different prefix in 1.):

   ```console
   $ sudo ip route add 2001:db8::/64 via fe80::e0bc:7dff:fe6b:8b09 dev tap0
   ```

4. Run `make term` again to configure the global address for the `native` device and the route to
   the host from the `native` device:

   ```console
   > ifconfig 5 add 2001:db8::2
   ifconfig 5 add 2001:db8::2
   success: added 2001:db8::2/64 to interface 5
   > nib route add 5 default fe80::e0bc:7dff:fe6b:8b08
   nib route add 5 default fe80::e0bc:7dff:fe6b:8b08
   ```

   **Keep the `native` instance open for [2.3](#configure-dns-over-dtls-client-and-query-a-name)**

## Install and run a test server

1. In a new terminal install `aiodnsprox` as your test server:

   ```console
   $ sudo pip install git+https://github.com/anr-bmbf-pivot/aiodnsprox/
   ```

2. Provide a minimal configuration file containing the `TLS_PSK_WITH_AES_128_CCM_8` pre-shared key
   credentials for the DTLS server:

   ```console
   $ cat << EOF > test.yaml
   dtls_credentials:
     client_identity: Client_identity
     psk: secretPSK
   EOF
   ```

3. Run the DNS server with a DTLS transport bound to the `tap0` interface (`-d 2001:db8::1`; replace
   the address if you used a different one in [2.1](#setting-up-a-tap-interface)'s step 1), using a
   public DNS server as upstream (`-U 9.9.9.9`). `sudo` is required to be able to bind to the
   DNS over DTLS port 853:

   ```console
   $ sudo aiodns-proxy -C test.yaml -U 9.9.9.9 -d 2001:db8::1
   ```

## Configure DNS over DTLS client and query a name

Use the RIOT shell you kept open in [2.1](#setting-up-a-tap-interface) to configure the DNS over
DTLS server and request `example.org` from it

1. Provide the DNS over DTLS server address, port (optional), credential tag (5853),
   `TLS_PSK_WITH_AES_128_CCM_8` client identity (`Client_identity`) and
   `TLS_PSK_WITH_AES_128_CCM_8` secret key (`secretPSK`) to the DNS over DTLS client:

   ```console
   > dodtls server [2001:db8::1]:853 5853 Client_identity secretPSK
   > dodtls server
   DNS over DTLS server: [2001:db8::1]:853
   ```

2. Now you should be able to query a name:
   ```console
   > dodtls request example.org inet6
   dodtls request example.org inet6
   example.org resolves to 2606:2800:220:1:248:1893:25c8:1946
   > dodtls request example.org inet
   dodtls request example.org inet
   example.org resolves to 93.184.216.34
   ```
