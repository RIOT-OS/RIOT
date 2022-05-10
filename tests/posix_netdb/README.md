# Test for the posix module *netdb*

## Overview

This test application tests the *posix_netdb* module.

## Run the test

The test needs an upstream connection to the internet. This is easily done with
the module *esp_wifi* on an esp32. To run the test on an esp32 you can run:\
`WIFI_SSID='"Your WiFi SSID"' WIFI_PASS='"Your WiFi Password"' BOARD=esp32-wroom-32 make flash test`
