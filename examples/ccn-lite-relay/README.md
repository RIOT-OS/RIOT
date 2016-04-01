# CCN-Lite on RIOT

This application demonstrates how to use the Content-Centric Networking stack
from [CCN-Lite](http://www.ccn-lite.net/) on RIOT. In the current state you can
use only one packet format, `ndn2013`, and only relay over the link-layer
(Ethernet, IEEE~802.15.4 or what else is supported by your network device).

## The shell commands

RIOT provides three shell to interact with the CCN-Lite stack:
* `ccnl_open` - opens and configures a network device to be used for CCN-Lite.
                It expects one parameter specifying the PID of the network
                device. (You can figure out the PID of your network device(s)
                by calling `ifconfig`.) In this example, this should always be
                3, hence, calling `ccnl_open 3` should work. (If you specify an
                incorrect ID, you should get an error message.) You have to
                call this command, before you can actually send or receive
                anything.
* `ccnl_int`  - generates and sends out an Interest. The command expects one
                mandatory and one optional parameter. The first parameter
                specifies the exact name (or a prefix) to request, the second
                parameter specifies the link-layer address of the relay to use.
                If the second parameter is omitted, the Interest will be
                broadcasted. You may call it like this:
                `ccnl_int /riot/peter/schmerzl b6:e5:94:26:ab:da`
* `ccnl_cont` - generates and populates content. The command expects one
                mandatory and one optional parameter. The first parameter
                specifies the name of the content to be created, the second
                parameter specifies the content itself. The second parameter
                may include spaces, e.g. you can call:
                `ccnl_cont /riot/peter/schmerzl Hello World! Hello RIOT!`
* `ccnl_fib`  - modifies the FIB or shows its current state. If the command is
                called without parameters, it will print the current state of
                the FIB. It can also be called with the action parameters `add`
                or `del` to add or delete an entry from the FIB, e.g.
                `ccnl_fib add /riot/peter/schmerzl ab:cd:ef:01:23:45:67:89`
                will add an entry for `/riot/peter/schmerzl` with
                `ab:cd:ef:01:23:45:67:89` as a next hop and
                `ccnl_fib del /riot/peter/schmerzl`
                will remove the entry for `/riot/peter/schmerzl` and
                `ccnl_fib del ab:cd:ef:01:23:45:67:89`
                will remove all entries with `ab:cd:ef:01:23:45:67:89` as a
                next hop.

## Example setup

An example usage of this application could be setup like this:
1. Open a terminal window, navigate to the RIOT directory, and enter
   `dist/tools/tapsetup/tapsetup -c`.
2. Open a second terminal window and navigate to this directory in both of
   windows.
3. Call `make -B clean all term` in the first terminal and `PORT=tap1 make
   term` in the second one.
4. Enter `ccnl_open 3` in both terminals.
5. Enter `ccnl_cont /riot/peter/schmerzl Hello World! Hello RIOT!` on the first
   terminal.
6. Add a FIB entry for this prefix on the second node, e.g. using the broadcast
   address: `ccnl_fib add /riot/peter/schmerzl ff:ff:ff:ff:ff:ff`
7. Enter `ccnl_int /riot/peter/schmerzl` in the second terminal.
8. See the content being displayed. Be happy!

## Makefile configuration

The ccn-lite package provides several configuration options through defines
that can be set in the application Makefile. The following options are
mandatory for now:
* `CFLAGS += -DUSE_LINKLAYER`         - use CCN directly over the link layer
* `CFLAGS += -DCCNL_UAPI_H_`          - tell ccn-lite to use the UAPI
* `CFLAGS += -DUSE_SUITE_NDNTLV`      - use NDNTLV packet format
* `CFLAGS += -DNEEDS_PREFIX_MATCHING` - enables prefix matching
* `CFLAGS += -DNEEDS_PACKET_CRAFTING` - enable userspace packet creation
Here's a list of some additional interesting options:
* `CFLAGS += -DUSE_RONR`              - enable Reactive Optimistic Name-based
                                        Routing (RONR)
* `CFLAGS += -DUSE_STATS`             - enable statistics
* `CFLAGS += -DUSE_DUP_CHECK`         - enable duplicate checks when forwarding
* `CFLAGS += -DUSE_HMAC256`             - HMAC256 signed packets for CCNx1.0
                                        encoding


## Wireshark dissector

One can use the Wireshark dissector from the named-data project
(https://github.com/named-data/ndn-tools/tree/master/tools/dissect-wireshark)
for analyzing the traffic in Wireshark.

However, please note, that - in order to be compatible with the default
CCN-Lite upstream configuration - a different Ethertype (`0x0801` instead of
`0x8624`) is used.

The simplest way to get this working is to copy the `ndn.lua` file into your
local Wireshark plugin directory (e.g. `$HOME/.wireshark/plugins`) and update
https://github.com/named-data/ndn-tools/blob/master/tools/dissect-wireshark/ndn.lua#L408
to `0x0801).
