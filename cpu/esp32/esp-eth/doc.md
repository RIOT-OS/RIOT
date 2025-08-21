<!--
Copyright (C) 2018 Gunar Schorcht

This file is subject to the terms and conditions of the GNU Lesser
General Public License v2.1. See the file LICENSE in the top level
directory for more details.
-->

@defgroup       cpu_esp32_esp_eth ESP32 Ethernet netdev interface
@ingroup        cpu_esp32
@brief          ESP32 ethernet network device driver

This module realizes a netdev interface using the ESP32 Ethernet MAC
module.

@author      Gunar Schorcht <gunar@schorcht.net>

ESP32 provides an <b>Ethernet MAC layer module (EMAC)</b> according to the IEEE 802.3 standard which can be used together with an external physical layer chip (PHY) to realize a 100/10 Mbps Ethernet interface. Supported PHY chips are the Microchip LAN8710/LAN8720, the IC Plus 101G, and the Texas Instruments TLK110.

The RIOT port for ESP32 realizes with module ```esp_eth``` a ```netdev``` driver for the EMAC which uses RIOT's standard Ethernet interface.

If the board has one of the supported PHY layer chips connected to the ESP32, the ```esp_eth``` module should be enabled by default in board's ```Makefile.dep``` when module ```netdev_default``` is used.
```
ifneq (,$(filter netdev_default,$(USEMODULE)))
    USEMODULE += esp_eth
endif
```
Otherwise, the application has to add the ```esp_eth``` module in its makefile when needed.

@note
The board has to have one of the supported PHY modules to be able to use the Ethernet MAC module.
