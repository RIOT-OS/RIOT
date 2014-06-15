DTLS Playground for adults and kids
===================================

Let you anger flow!!!111

```
$ cd posix_udp_socket
$ ../../cpu/native/tapsetup.sh create
```

```
$ make term
/home/cholin/uni/swp-telematik/RIOT/examples/dtls/bin/native/dtls.elf tap0
RIOT native uart0 initialized.
RIOT native interrupts/signals initialized.
LED_GREEN_OFF
LED_RED_ON
RIOT native board initialized.
RIOT native hardware initialization complete.

kernel_init(): This is RIOT! (Version: 2014.05-12-g4edb6-cholin-features/dtls)
kernel_init(): jumping into first task...
UART0 thread started.
uart0_init() [OK]
Initializing transport layer packages. Size of socket_type: 177
> init
init
Configured IPv6 addresses:
	fe80::ff:fe00:4413/128
	ff02::1:0:ff00:4413/128
> send "ff02::1:0:ff00:4413" "0x12 0xA1"
send "ff02::1:0:ff00:4413" "0x12 0xA1"
dtls_record_compress() not yet implementedTLS RECORD PACKET
	Header:
		Type: 23
		Version: 254 253
		Epoch: 0
		Sequence: 0
		Length: 2
	Fragment:
		13: 18 (12)
		14: 161 (A1)
TLS RECORD LAYER: Sent 2 Bytes

> 
```

