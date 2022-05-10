# Test for *sock_dns* using *lwIP*

## Overview

This test application tests the *sock_dns* module by using *lwIP* as network stack.
Both IPv4 and IPv6 are tested.

## Run the test

### Automatically

The test needs an upstream connection to the internet. This is easily done with
the module *esp_wifi* on an esp32. To run the test on an esp32 you can run:\
`WIFI_SSID='"Your WiFi SSID"' WIFI_PASS='"Your WiFi Password"' BOARD=esp32-wroom-32 make flash test`

### Manually

You can also play with the test application instance by interfacing the shell.
First flash and open a terminal by running:\
``WIFI_SSID='"Your WiFi SSID"' WIFI_PASS='"Your WiFi Password"' BOARD=esp32-wroom-32 make flash term``

Following commands can be used:

- Set a DNS server: `dns server <4 or 6 (IPv4/IPv6)> <DNS server addr> [DNS server port]`
  - Examples:
    - `dns server 4 8.8.8.8`
    - `dns server 6 2001:4860:4860::8888`
- Request an IP for a domain name: `dns request <0, 4 or 6 (Unspecified/IPv4/IPv6)> <name>`
  - Examples:
    - `dns request 0 example.com` => `example.com resolves to 2606:2800:220:1:248:1893:25c8:1946`
    - `dns request 4 example.com` => `example.com resolves to 93.184.216.34`
    - `dns request 6 example.com` => `example.com resolves to 2606:2800:220:1:248:1893:25c8:1946`