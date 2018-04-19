# gnrc_minimal example

This is a minimalistic example for RIOT's gnrc network stack.
The application will auto initialize the device's default network interface and
the IPv6 stack and print its link-local address.
You can ping the printed address from another device to check if the node is
alive. Please note that you may have to disable 6LoWPAN header compression
(IPHC), since this example does not include it. In the `gnrc_networking`
example or any other application with GNRC and a shell, you can do so by
calling `ifconfig <if_id> -iphc`).

The example demonstrates how one can reduce the memory footprint of a `gnrc`
application by enabling only the minimum subset of features and setting
configuration options to smaller values than the default. In particular, this
reduction of memory consumption in comparison to the `gnrc_networking` example
is achieved by:
 * Using only the `gnrc_ipv6` and `gnrc_ndp` modules (instead of
   `gnrc_ipv6_router_default`). This configures the node as a IPv6/6LoWPAN host
   instead of a router and disable optional features such as 6LoWPAN IPHC or
   fragmentation.
 * Not using `gnrc_rpl`.
 * Disabling features meant for helping during development and debugging such
   as the shell or setting the `DEVELHELP` flag.
 * Using the minimal standard pseudo random number generator instead of the
   Mersenne Twister.
 * Reducing the number of configurable IPv6 addresses per interface to 4
   (instead of 7 or 8).
 * Reducing the packet buffer size from 6kB to 512 bytes.
 * Reducing the maximum neighbor cache size from 8 to 1.

Please take a look at the Makefile to see how the configuration is done.
