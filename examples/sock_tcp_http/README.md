sock_tcp_http
=============

This example demonstrates how to implement simple web sensor using the
RIOT TCP sock API.

As ipv6 is disabled by default on most web browsers, it uses the lwip ipv4
network stack.

By default it uses a esp8266-esp-12x wifi module with a DHT21 temperature
and humidity sensor.

The sensor data are served as json and pulled every two seconds by
javascript XMLHttpRequest method.

There is also a munin plugin example.

Usage
=====

```
CFLAGS='-DESP_WIFI_SSID=\"MySSID\" -DESP_WIFI_PASS=\"MyPassphrase\"' make all flash term
```
Run `ifconfig` in RIOT shell to get the sensor IP address.

Open it in a web browser.
