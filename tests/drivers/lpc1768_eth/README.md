# LPC1768 ethernet driver test

Test application for the LPC1768 ethernet driver.

## Usage

Build and flash the application onto a compatible LPC1768 board. There are two
pseudo-modules that can be enabled (see `Makefile`):

* `lpc1768_eth_link_up` - enables the link state timer
* `lpc1768_eth_auto` - enable auto-negotiation (implies `lpc1768_eth_link_up`)

This test application makes use of `test_utils_netdev_eth_minimal`. Refer to
the API documentation for more information.

## Expected output

When the application is running, it should initialize the ethernet driver
automatically. Verify that the ethernet interface is up and configured, using
the `ifconfig` command.
