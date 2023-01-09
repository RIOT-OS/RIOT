/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_esp_common_esp_wifi ESP-WiFi netdev interface
 * @ingroup     cpu_esp_common
 * @ingroup     cpu_esp32
 * @ingroup     cpu_esp8266
 * @brief       Network device driver for the ESP SoC WiFi interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

This module realizes a `netdev` interface for the built-in WiFi interface of
ESP SoCs.

@note On the ESP32 this `netdev` driver supports both WPA2 Personal Mode and
WPA2 Enterprise Mode. On ESP8266, only WPA2 Personal mode is currently
supported.

### WPA2 Personal Mode

To use the WiFi `netdev` driver in WPA2 Personal Mode with a
preshared key (PSK), module `esp_wifi` has to be enabled.

```
USEMODULE += esp_wifi
```

Furthermore, the following configuration parameters have to be defined:

<center>

Parameter          | Default                   | Description
:------------------|:--------------------------|:------------
ESP_WIFI_SSID      | "RIOT_AP"                 | SSID of the AP to be used.
ESP_WIFI_PASS      | -                         | Passphrase used for the AP as clear text (max. 64 chars).
ESP_WIFI_STACKSIZE | #THREAD_STACKSIZE_DEFAULT | Stack size used for the WiFi netdev driver thread.

</center>

These configuration parameter definitions, as well as enabling the `esp_wifi`
module, can be done either in the makefile of the project or at make command
line, for example:

```
USEMODULE=esp_wifi \
CFLAGS='-DESP_WIFI_SSID=\"MySSID\" -DESP_WIFI_PASS=\"MyPassphrase\"' \
make -C examples/gnrc_networking BOARD=...
```

@note
- Module `esp_wifi` is not enabled automatically when module
`netdev_default` is used.
- Leave 'ESP_WIFI_PASS' undefined to connect to an open WiFi access point.
- The Wifi network interface (module `esp_wifi`) and the
ESP-NOW network interface (module `esp_now`)
can be used simultaneously, for example, to realize a border router for
a mesh network which uses ESP-NOW.

### WPA2 Enterprise Mode

To use the WiFi `netdev` driver in WPA2 Enterprise Mode with IEEE 802.1X/EAP
authentication, module `esp_wifi_enterprise` has to be enabled.

```
USEMODULE += esp_wifi_enterprise
```

It supports the following EAP authentication methods:

- PEAPv0
- PEAPv1
- TTLS

As inner (phase 2) EAP authentication method, only MSCHAPv2 is supported.

To use module `esp_wifi_enterprise` with these authentication methods, the
following configuration parameters have to be defined:

<center>

Parameter          | Default   | Description
:------------------|:----------|:------------
ESP_WIFI_SSID      | "RIOT_AP" | SSID of the AP to be used.
ESP_WIFI_EAP_ID    | none      | Optional anonymous identity used in phase 1 (outer) EAP authentication. If it is not defined, the user name defined for phase 2 (inner) EAP authentication is used as idendity in phase 1.
ESP_WIFI_EAP_USER  | none      | User name used in phase 2 (inner) EAP authentication.
ESP_WIFI_EAP_PASS  | none      | Password used in phase 2 (inner) EAP authentication.
ESP_WIFI_STACKSIZE | #THREAD_STACKSIZE_DEFAULT | Stack size used for the WiFi netdev driver thread.

</center>

These configuration parameter definitions, as well as enabling the `esp_wifi`
module, can be done either in the makefile of the project or at make command
line, for example:

```
USEMODULE=esp_wifi_enterprise \
CFLAGS='-DESP_WIFI_SSID=\"MySSID\" -DESP_WIFI_EAP_ID=\"anonymous\" -DESP_WIFI_EAP_USER=\"MyUserName\" -DESP_WIFI_EAP_PASS=\"MyPassphrase\"' \
make -C examples/gnrc_networking BOARD=...
```

@note
- Since there is no possibility to add the CA certificate to the RIOT image,
the verification of the AP certificate is not yet supported.
- Module `esp_wifi_enterprise` is not enabled automatically when module
`netdev_default` is used.
- The Wifi network interface (module `esp_wifi_enterprise`) and the
ESP-NOW network interface (module `esp_now`)
can be used simultaneously, for example, to realize a border router for
a mesh network which uses ESP-NOW.
In this case the ESP-NOW interface must use the same channel as the AP of the
infrastructure WiFi network. All ESP-NOW nodes must therefore be compiled with
the channel of the AP asvalue for the parameter 'ESP_NOW_CHANNEL'.

 */
