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
