# EFM32 ethernet driver test

Test application for the EFM32 ethernet driver.

## Usage

Build and flash the application onto a compatible EFM32 board. There are two
pseudo-modules that can be enabled (see `Makefile`):

* `efm32_eth_link_up` - enables the link state timer
* `efm32_eth_auto` - enable auto-negotiation (implies `efm32_eth_link_up`)

This test application makes use of `test_utils_netdev_eth_minimal`. Refer to
the API documentation for more information.

## Expected output

When the application is running, it should initialize the ethernet driver
automatically. Verify that the ethernet interface is up and configured, using
the `ifconfig` command.
