DTLS Playground for adults and kids
===================================

Let you anger flow!!!111

```
$ cd posix_udp_socket
$ ../../cpu/native/tapsetup.sh create
```

Start RIOT and DTLS server

```
/home/cholin/uni/swp-telematik/RIOT/examples/dtls_playground/bin/native/dtls_playground.elf tap0
RIOT native uart0 initialized.
RIOT native interrupts/signals initialized.
LED_GREEN_OFF
LED_RED_ON
RIOT native board initialized.
RIOT native hardware initialization complete.

kernel_init(): This is RIOT! (Version: 2014.05-15-g2265-cholin-features/dtls)
kernel_init(): jumping into first task...
UART0 thread started.
uart0_init() [OK]
Initializing transport layer packages. Size of socket_type: 177
> init
init
Configured IPv6 addresses:
	fe80::ff:fe00:23b8 / 128
	ff02::1:0:ff00:23b8 / 128

DTLS SERVER ON PORT 8443 (THREAD PID: 11)

> 
```

Start another RIOT instance with DTLS

```
$ bin/native/dtls_playground.elf tap1
RIOT native uart0 initialized.
RIOT native interrupts/signals initialized.
LED_GREEN_OFF
LED_RED_ON
RIOT native board initialized.
RIOT native hardware initialization complete.

kernel_init(): This is RIOT! (Version: 2014.05-15-g2265-cholin-features/dtls)
kernel_init(): jumping into first task...
UART0 thread started.
uart0_init() [OK]
Initializing transport layer packages. Size of socket_type: 177
> init
init
Configured IPv6 addresses:
	fe80::ff:fe00:23f5 / 128
	ff02::1:0:ff00:23f5 / 128

DTLS SERVER ON PORT 8443 (THREAD PID: 11)

> 
```

Now you can send data through DTLS from one instance to another with ``send``.

```
> send "ff02::1:0:ff00:23b8" "0xFE 0xFF 0xCF"
send "ff02::1:0:ff00:23b8" "0xFE 0xFF 0xCF"
dtls_record_compress() not yet implemented
dtls_record_encrypt() not yet implemented
TLS RECORD LAYER: Sent 3 Bytes
```

On the other RIOT instance you should see the following:

```
> dtls_record_decrypt() not yet implemented
dtls_record_decompress() not yet implemented
GOT DATA
	0: 254 (FE)
	1: 255 (FF)
	2: 207 (CF)
```


