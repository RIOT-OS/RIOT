# gnrc_networking_border_router example

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
USEMODULE += gnrc_slip
```
with something like
```
USEMODULE += encx24j600
```
and specify the target platform as `BOARD = myplatform`.
In order to use the border router over SLIP, please check the `periph_conf.h`
of the corresponding board and look out for the `UART_NUMOF` parameter. Its
value has to be bigger than 1.

## Configuration

In order to connect a RIOT 6LoWPAN border router over SLIP you run a small
program called tunslip (imported from Contiki) [2] on the Linux host. The
program can be found in the `dist/tools/tunslip` folder and has to be compiled
before first use (simple calling `make` should be enough). Now, one can start
the program by calling something like:
```bash
cd dist/tools/tunslip
make
sudo ./tunslip6 affe::1/64 -t tun0 -s /dev/ttyUSB0
```
Assuming that `/dev/ttyUSB0` is the device descriptor for the (additional) UART
interface of your RIOT board.

On the RIOT side you have to configure the SLIP interface by configuring a
corresponding IPv6 address, e.g.
```
ifconfig 6 add affe::2
```
and adding the SLIP interface to the neighbor cache (because Linux won't
respond to neighbor solicitations on an interface without a link-layer address)
by calling
```
ncache add 6 affe::1
```
After this you're basically done and should be able to ping between the border
router and the outside world (assuming that the Linux host is properly
forwarding your traffic).

Additionally, you can configure IPv6 addresses on the 6LoWPAN interface for
communication with other 6LoWPAN nodes. See also the `gnrc_networking` example
for further help.

[1] https://tools.ietf.org/html/rfc1055

[2] https://github.com/contiki-os/contiki/blob/master/tools/tunslip.c


# gnrc_border_router with ethos
A new setup is possible thanks to \#4438(https://github.com/RIOT-OS/RIOT/pull/4438) on which only one serial interface is needed. This is done through the ethos (Ethernet Over Serial) driver that multiplexes serial data between ethernet packets and shell commands.

## Requirements
This functionality works only in Linux machines, Mac support will be added in the future (lack of `tap` interface), and Windows support should possible by setting up the needed `tap` interface with an external program (not tested).

## Setup
Take care about the modification of the `Makefile`, since by default SLIP is enabled.
In order to select ethos as the driver, you should modify some lines as following:
```make
#CFLAGS += -DSLIP_UART=$(SLIP_UART)
#CFLAGS += -DSLIP_BAUDRATE=$(SLIP_BAUDRATE)
CFLAGS += '-DETHOS_UART=UART_DEV(0)' -DETHOS_BAUDRATE=115200 -DUSE_ETHOS_FOR_STDIO
```
Then deactivate the slip module and activate ethos
```make
# Include SLIP package for IP over Serial communication
#USEMODULE += gnrc_slip
USEMODULE += ethos gnrc_netdev2
```
This will allow to compile the BR firmware using ethos.
Thus:
```bash
$ make clean all flash
```
should compile the example and flash it to your board.

Afterwards, you need to compile the ethos tool instead of tunslip.
To do this, in `dist/tools/ethos` you'll find the tool, just compile it with:
```bash
$ make
```

## Usage
Once you flashed the BR example on your board, you should intialize your IP settings by choosing a prefix for your 6lo network. Just type `make term` and setup your BR:
```
> ifconfig 6 add affe::2/48
> ifconfig 5 add affe::3/64
> fibroute add :: via <link-local of tap> dev 6
```
Here we have choosen `affe` as the prefix. It should be disseminated to your other nodes on the radio range.

Next, to have access to the nodes attached to the BR from your linux machine, it is necessary to add a `tap` interface as well as configure it to route the packets coming from your 6lo network.
As an example, you can do the following:
```bash
$ sudo ip tuntap add tap0 mode tap user ${USER}
```
This will setup your `tap` interface, then:
```bash
$ sudo ip link set tap0 up
$ sudo ip a a affe::1/48 dev tap0
$ sudo ip r d affe::/48 dev tap0
$ sudo ip r a affe::2 dev tap0
$ sudo ip r a affe::/48 via affe::2 dev tap0
```
will configure the interface and add some routes in order to have access to the 6lo network from Linux.

Finally, run ethos by choosing the right serial interface (`/dev/ttyUSB*` or `/dev/ttyACM*` according to your board):
```bash
$ sudo ./ethos tap0 /dev/ttyACM0
```
After doing this, you should be able to ping your nodes using the global addres starting by your prefix, on our case `affe::`.

# gnrc_border_router using automatic configuration (not merged yet)
In \#4725 (https://github.com/RIOT-OS/RIOT/pull/4725) an effort to automate all the previous steps is done using 3 tools:
- ethos
- uhcp
- start_network.sh

This will enable a *ready-to-use* environment in only one command.

## Setup
Merge \#4725 to your current branch.
Afterwards, compile `ethos` as explained previously.
Then, you need to compile UHCP (micro host control protocol). This tool is found in `/dist/tools/uhcp`. So, as usual:
```
$ make clean all
```

At this point you should have these two programs compiled, necessary to compile your gnrc_border_router ** DO NOT COMPILE IT BEFORE ethos AND uhcp **.
Then, proceed to compile and flash gnrc_border_router to your board as explained before.

## Usage
Start the `start_network.sh` script by doing on `dist/tools/ethos`:
```bash
$ sudo sh start_network.sh /dev/ACMx tap0 2001:db8::/64
```
which will execute the commands mentionned on the previous setup.
Notice that this will also configure `2001:db8::/64` as a prefix, and it should announce it to the wireless interface.
Thus, nothing to do on the RIOT side.

As this is not already merged on the RIOT master, maybe you can find some bugs. Refer to the discussion on the PR to find if someone has already experienced the same issue, maybe a workaround is available.
Another common issue is that after a while the global addresses on the BR Neighbour Cache can dissappear. We are investigating why it happens and we'll propose a solution soon. If you experience it, just reboot your nodes and they will come again on-line.
