# gnrc_border_router using automatic configuration
This setup uses a single serial interface, ethos (Ethernet Over Serial) 
and UHCP (micro Host Configuration Protocol). 
Ethos multiplexes serial data to separate ethernet packets from shell commands.
UHCP is in charge of configuring the wireless interface prefix 
and routes on the BR.

The script `start_network.sh` enables a *ready-to-use* BR in only one command.

## Requirements
This functionality works only on Linux machines.
Mac OSX support will be added in the future (lack of native `tap` interface).

## Setup
First, you need to compile `ethos`.
Go to `/dist/tools/ethos` and type:

```bash
make clean all
```

Then, you need to compile UHCP.
This tool is found in `/dist/tools/uhcpd`. So, as for `ethos`:

```bash
make clean all
```

Afterwards, proceed to compile and flash `gnrc_border_router` to your board:

```bash
make clean all flash
```

## Usage
Start the `start_network.sh` script by doing on `dist/tools/ethos`:

```bash
sudo sh start_network.sh /dev/ttyACMx tap0 2001:db8::/64
```

This will execute the needed commands to setup a `tap` interface 
and configure the BR.
Notice that this will also configure `2001:db8::/64` as a prefix.
This prefix should be announced to other motes through the wireless interface.

As said previously, `ethos` allows to send IP packets and shell commands.
This is done through the same serial interface.
By typing `help` you will get the list of available shell commands.

At this point you should be able to ping motes using their global address.
For instance, if you use the [`gnrc_networking`](https://github.com/RIOT-OS/RIOT/tree/master/examples/gnrc_networking) example on the mote, you can 
ping it from your machine with:

```
> ping6 2001:db8:0:1234:0:567:8:1
```

Just replace this address by your mote's address.
Using `ifconfig` on the shell of your mote shows you the addresses of your 
mote, for instance:

```
Iface  7   HWaddr: 59:72  Channel: 26  Page: 0  NID: 0x23
            Long HWaddr: 5a:46:10:6e:f2:f5:d9:72 
            TX-Power: 0dBm  State: IDLE  max. Retrans.: 3  CSMA Retries: 4 
            AUTOACK  CSMA  MTU:1280  HL:64  6LO  RTR  RTR_ADV  IPHC  
            Source address length: 8
            Link type: wireless
            inet6 addr: ff02::1/128  scope: local [multicast]
            inet6 addr: fe80::5846:106e:f2f5:d972/64  scope: local
            inet6 addr: ff02::1:fff5:d972/128  scope: local [multicast]
            inet6 addr: 2001:db8::5846:106e:f2f5:d972/64  scope: global
            inet6 addr: ff02::2/128  scope: local [multicast]
```

The script also sets up a ULA (Unique Local Address) address on your 
Linux `tap0` network interface.
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
USEMODULE += ethos
CFLAGS += '-DETHOS_UART=UART_DEV(0)' -DETHOS_BAUDRATE=115200 -DUSE_ETHOS_FOR_STDIO
FEATURES_REQUIRED += periph_uart
endif
# include UHCP client
USEMODULE += gnrc_uhcpc
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
sudo ip a a 2001:db8::1/48 dev tap0
sudo ip r d 2001:db8::/48 dev tap0
sudo ip r a 2001:db8::2 dev tap0
sudo ip r a 2001:db8::/48 via 2001:db8::2 dev tap0
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

