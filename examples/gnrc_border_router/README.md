# gnrc_border_router using automatic configuration
This setup uses a single serial interface, ethos (Ethernet Over Serial) and
DHCPv6.
Ethos multiplexes serial data to separate ethernet packets from shell commands.
DHCPv6 is in charge of configuring the wireless interface prefix on the BR.

The script `start_network.sh` enables a *ready-to-use* BR in only one command
(given you have set-up a DHCPv6 server on your machine).

## Requirements
This functionality works only on Linux machines.
Mac OSX support will be added in the future (lack of native `tap` interface).

You also will need to have a DHCPv6 server to be set up. As an example we use
KEA (`sudo apt-get install kea-dhcp6-server` on Debian and Ubuntu machines), as
it is supported by the ISC, freely available and easy to configure, but you can
use any DHCPv6 client that is compliant to the DHCPv6 RFCs.

## Setup
You need to configure your DHCPv6 server for prefix delegation of the prefix you
choose. Here is an example configuration for KEA with the prefix `2001:db8::/32`
on the interface `tap0` (keep the attributes not mentioned here as is; for
native, when using the `dist/tools/tapsetup/tapsetup` script you need to take
the `tapbr0` interface though):

```json
"Dhcp6":
{
  "interfaces-config": {
    "interfaces": [ "tap0" ]
  },
  ...
  "subnet6": [
  {    "interface": "tap0",
       "subnet": "2001:db8::/16",
       "pd-pools": [ { "prefix": "2001:db8::",
                       "prefix-len": 16,
                       "delegated-len": 64 } ] },
  ]
  ...
}
```

Note that when working with TAP interfaces you might need to restart your DHCPv6
server once *after* you started the border router application (see below), since
Linux might not recognize the interface as connected.

Afterwards, proceed to compile and flash `gnrc_border_router` to your board:

```bash
make clean all flash
```

When using `native` you also might need to configure your tap interface:

```sh
sudo ip a a fe80::1 dev tap0
sudo ip route add 2001:db8::/32 via fe80::2 dev tap0
```

For other platforms this is done via the `start_network.sh` script in
`dist/tools/ethos` that is started by the `make term` target so you don't need
to do that.

## Usage
Just use the `make term` command as you usually would. The example's Makefile
assures that the rest is done for you:

```bash
make term
```

If you want to use another interface with ethos or have configured another
prefix on your DHCPv6 server, you can set those using the `TAP` and
`IPV6_PREFIX` variable respectively:

```bash
TAP=tap1 IPV6_PREFIX=2001:db8:8000::/40 make term
```

As said previously, `ethos` (which will be started via `make term`) allows to
send IP packets and shell commands. This is done through the same serial
interface.  By typing `help` you will get the list of available shell commands.

At this point you should be able to ping motes using their global address.
For instance, if you use the [`gnrc_networking`](https://github.com/RIOT-OS/RIOT/tree/master/examples/gnrc_networking) example on the mote, you can
ping it from your machine with:

```
> ping6 2001:db8:0:1:1234:0:567:8:1
```

Just replace this address by your mote's address.
Using `ifconfig` on the shell of your mote shows you the addresses of your
mote, for instance:

```
Iface  7  HWaddr: 45:5b  Channel: 26  NID: 0x23
          Long HWaddr: 00:5a:45:50:0a:00:45:5b
          MTU:1280  HL:64  RTR
          RTR_ADV  IPHC
          Source address length: 8
          Link type: wireless
          inet6 addr: fe80::25a:4550:a00:455b  scope: local  VAL
          inet6 addr: 2001:db8:0:1:25a:4550:a00:455b  scope: global  VAL
          inet6 group: ff02::2
          inet6 group: ff02::1
          inet6 group: ff02::1:ff00:455b
```

There is also an ULA (Unique Local Address) address set-up on your loopback
interface.
You can check your ULA on your PC with `ifconfig` Linux command.
On this example, such address can be pinged from 6lo motes:

```
> ping6 fd00:dead:beef::1
```

Thus far, IPv6 communication with between your PC and your motes is enabled.

# gnrc_border_router with manual config
You can use `ethos` as a standalone driver, if you want to setup the BR manually.

## Setup
To select ethos as the serial driver, be sure that the `Makefile`
has the following:

```make
ifeq (,$(filter native,$(BOARD)))
GNRC_NETIF_NUMOF := 2
USEMODULE += stdio_ethos
CFLAGS += '-DETHOS_UART=UART_DEV(0)' -DETHOS_BAUDRATE=115200
FEATURES_REQUIRED += periph_uart
endif
```

You'll need IPv6 access to the nodes attached to the BR from your Linux PC.
To do this, it is necessary to add a `tap` interface.
As an example, you can do it as follows:

```bash
sudo ip tuntap add tap0 mode tap user ${USER}
```

This will setup your `tap` interface.
Then configure it to route the packets coming from your 6lo network:

```bash
sudo ip link set tap0 up
sudo ip a a 2001:db8::1/32 dev tap0
sudo ip r d 2001:db8::/32 dev tap0
sudo ip r a 2001:db8::2 dev tap0
sudo ip r a 2001:db8::/32 via 2001:db8::2 dev tap0
```

Please note that the prefix `2001:db8::` is used as an example.

Then you can flash the `gnrc_border_router` example on your board:

```bash
make clean all flash
```

On this RIOT BR two interfaces are present.
A wired interface represents the serial link between Linux and your mote.
A wireless interface represents the 802.15.4 radio link.
In order to route packets between this two interfaces, 
you can do the following:

```
> ifconfig 6 add 2001:db8::2/48
> ifconfig 5 add 2001:db8::3/64
> fibroute add :: via <link-local of tap> dev 6
```

By adding the address to the wireless interface the prefix will be 
disseminated.
This prefix will be automatically added by the motes in the radio range.

## Usage
Run ethos by choosing a serial interface according to your board.
For instance `/dev/ttyUSB*` or `/dev/ttyACM*`.

```bash
sudo ./ethos tap0 /dev/ttyACM0
```

Now, you should be able to ping your nodes.
Use the global address starting by your prefix, on our case `2001:db8::`:

```
> ping6 2001:db8:0:1234:0:567:8:1
```

# gnrc_networking_border_router with SLIP

## Requirements

In order to setup a 6LoWPAN border router on RIOT, you need either a board that
offers an IPv6 capable network interface (e.g. the `encx24j600` Ethernet chip)
or connect it over the serial interface to a Linux host and use the SLIP
standard [1]. The example application in this folder assumes as a default to be
run on an Atmel SAM R21 Xplained Pro evaluation board using an external UART
adapter for the second serial interface. However, it is feasible to run the
example on any RIOT supported platform that offers either more than one UART or
be equipped with an IPv6 capable network device. In this case only the Makefile
of this application has to be slightly modified, e.g. by replacing the line
```
USEMODULE += ethos
```
with something like
```
USEMODULE += encx24j600
```
and specify the target platform as `BOARD = myplatform`.
In order to use the border router over SLIP, please check the `periph_conf.h`
of the corresponding board and look out for the `UART_NUMOF` parameter. Its
value has to be bigger than 1.

Be sure that you have replaced on your `Makefile` the lines to use SLIP.
You should have something like this:

```make
ifeq (,$(SLIP_UART))
# set default (last available UART)
SLIP_UART="UART_DEV(UART_NUMOF-1)"
endif		
ifeq (,$(SLIP_BAUDRATE))		
# set default		
SLIP_BAUDRATE=115200
endif

GNRC_NETIF_NUMOF := 2
INCLUDES += -I$(CURDIR)
CFLAGS += -DSLIP_UART=$(SLIP_UART)
CFLAGS += -DSLIP_BAUDRATE=$(SLIP_BAUDRATE)
# Include SLIP package for IP over Serial communication
USEMODULE += slipdev
```


## Configuration

In order to connect a RIOT 6LoWPAN border router over SLIP you run a small
program called tunslip6 (imported from Contiki) [2] on the Linux host. The
program can be found in the `dist/tools/tunslip` folder and has to be compiled
before first use (simple calling `make` should be enough). Now, one can start
the program by calling something like:
```bash
cd dist/tools/tunslip
make
sudo ./tunslip6 2001:db8::1/64 -t tun0 -s /dev/ttyUSB0
```
Assuming that `/dev/ttyUSB0` is the device descriptor for the (additional) UART
interface of your RIOT board.

On the RIOT side you have to configure the SLIP interface by configuring a
corresponding IPv6 address, e.g.
```
ifconfig 6 add 2001:db8::2
```
and adding the SLIP interface to the neighbor cache (because Linux won't
respond to neighbor solicitations on an interface without a link-layer address)
by calling
```
ncache add 6 2001:db8::1
```

Then, to propagate the prefix you should add an address to the wireless interface:

```
ifconfig 5 add 2001:db8::3
```

After this you're basically done and should be able to ping between the border
router and the outside world (assuming that the Linux host is properly
forwarding your traffic).

Additionally, you can configure IPv6 addresses on the 6LoWPAN interface for
communication with other 6LoWPAN nodes. See also the `gnrc_networking` example
for further help.

[1] https://tools.ietf.org/html/rfc1055

[2] https://github.com/contiki-os/contiki/blob/master/tools/tunslip.c

