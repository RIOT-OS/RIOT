/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_esp_common_esp_now ESP-NOW netdev interface
 * @ingroup     cpu_esp_common
 * @ingroup     cpu_esp32
 * @ingroup     cpu_esp8266
 * @brief       WiFi based ESP-NOW network device driver
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>

This module realizes a netdev interface using Espressif's ESP-NOW technology which uses the built-in WiFi module.

With ESP-NOW, the ESP nodes (ESP32 and ESP8266) provide a connectionless communication technology, featuring short packet transmission. It applies the IEEE802.11 Action Vendor frame technology, along with the IE function developed by Espressif, and CCMP encryption technology, realizing a secure, connectionless communication solution.

The RIOT port for ESP implements in module ```esp_now``` a ```netdev``` driver which uses ESP-NOW to provide a link layer interface to a meshed network of ESP nodes. In this network, each node can send short packets with up to 250 data bytes to all other nodes that are visible in its range.

@note Due to symbol conflicts in the ```esp_idf_wpa_supplicant_crypto``` module used by the ```esp_now``` with RIOT's ```crypto``` and ```hashes``` modules, ESP-NOW cannot be used for application that use these modules. Therefore, the module ```esp_now``` is not enabled automatically if the ```netdev_default``` module is used. Instead, the application has to add the ```esp_now``` module in its makefile when needed.<br>
```
USEMODULE += esp_now
```

For ESP-NOW, ESP nodes are used in WiFi SoftAP + Station mode to advertise their SSID and become visible to other ESP nodes. The SSID of an ESP node is the concatenation of the prefix ```RIOT_ESP_``` with the MAC address of its SoftAP WiFi interface. The driver periodically scans all visible ESP nodes.

The following parameters are defined for ESP-NOW nodes.

<center>

Parameter | Default | Description
:---------|:--------|:-----------
ESP_NOW_SCAN_PERIOD_MS | 10000UL | Defines the period in ms at which an node scans for other nodes in its range. The default period is 10 s.
ESP_NOW_SOFT_AP_PASS | "ThisistheRIOTporttoESP" | Defines the passphrase as clear text (max. 64 chars) that is used for the SoftAP interface of ESP-NOW nodes. It has to be same for all nodes in one network.
ESP_NOW_CHANNEL | 6 | Defines the channel that is used as the broadcast medium by all nodes together.
ESP_NOW_KEY | NULL | Defines a key that is used for encrypted communication between nodes. If it is NULL, encryption is disabled. The key has to be of type ```uint8_t[16]``` and has to be exactly 16 bytes long.

</center>

@note The ESP-NOW network interface (module `esp_now`) and the
Wifi network interface (module `esp_wifi`)
can be used simultaneously, for example, to realize a border router for
a mesh network which uses ESP-NOW.
In this case the ESP-NOW interface must use the same channel as the AP of the
infrastructure WiFi network. All ESP-NOW nodes must therefore be compiled with
the channel of the AP asvalue for the parameter 'ESP_NOW_CHANNEL'.

 */
