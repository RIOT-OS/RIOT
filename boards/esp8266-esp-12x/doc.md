/**

@defgroup    boards_esp8266_esp-12x  ESP-12x based boards
@ingroup     boards_esp8266
@brief       Support for boards that use ESP-12x modules.

\section esp8266_esp-12x ESP-12x based boards

## Overview

This board definition covers not just a single board, but rather a large
set of generic boards that either use one of the AI-Tinker ESP-12x ESP8266
modules or are compatible with them. ESP-12x stands for different versions
of the ESP-12 module: ESP-12, ESP-12E, ESP-12E and ESP-12S.

@image html "https://gitlab.com/gschorcht/RIOT.wiki-Images/raw/master/esp8266/ESP-12F_Module.png" "ESP8266 ESP-12F module" width=250px
\n

Common examples for this type of boards are the
[WEMOS LOLIN D1 mini V2](#esp8266_wemos_lolin_d1_mini), the
[NodeMCU DEVKIT](#esp8266_nodemcu_devkit_esp8266) and the
[Adafruit Feather HUZZAH ESP8266](#esp8266_adafruit_feather_huzzah_esp8266).
All these boards are characterized by using any of the ESP-12x module and
breaking out all GPIO pins.

@note This board definition is the most generic one and might also be used
for other ESP8266 and ESP8285 boards.

## MCU

Most features of the boards are provided by the ESP8266EX SoC.

<center>
MCU         | ESP8266EX
------------|----------------------------
Family      | Tensilica Xtensa LX106
Vendor      | Espressif
RAM         | 80 kByte
Flash       | 1 ... 16 MByte
Frequency   | 80 / 160 MHz
FPU         | no
Timers      | 1 x 32 bit
ADCs        | 1 x 10 bit (1 channel)
LEDs        | 1 x GPIO2
I2Cs        | 2 (software implementation)
SPIs        | 1
UARTs       | 1 (console)
WiFi        | built in
Vcc         | 2.5 - 3.6 V
Datasheet   | [Datasheet](https://www.espressif.com/sites/default/files/documentation/0a-esp8266ex_datasheet_en.pdf)
Technical Reference | [Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf)
</center>

## Flashing the Device

Flashing RIOT is quite straight forward, just connect the board using the
programming port to your host computer and type:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
make flash BOARD=esp8266-esp-12x ...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
For detailed information about ESP8266 as well as configuring and compiling
RIOT for ESP8266 boards, see \ref esp8266_riot.

## WEMOS LOLIN D1 mini {#esp8266_wemos_lolin_d1_mini}

[WEMOS LOLIN D1 mini]
(https://www.wemos.cc/en/latest/d1/d1_mini.html)
is a very interesting board series as it offers a stackable ESP8266 platform.
This board can be easily extended with a large number of compatible peripheral
shields, e.g. a micro SD card shield, an IR controller shield, a battery
shield, and various sensor and actuator shields, see [D1 mini shields]
(https://docs.wemos.cc/en/latest/d1_mini_shield/index.html) for more
information. This makes it possible to create different hardware configurations
without the need for a soldering iron or a breadboard.

@image html "https://gitlab.com/gschorcht/RIOT.wiki-Images/raw/master/esp8266/Wemos_D1_mini_Stack.png" "WEMOS LOLIN D1 mini stack example" width=400px
\n

There is also a MRF24J40 shield that can be used to extend the board with
an IEEE 802.15.4 radio module, the standard networking technology in RIOT.

@image html "https://gitlab.com/gschorcht/RIOT.wiki-Images/raw/master/esp8266/Wemos_MRF24J40_Shield.png" "MRF24J40 shield for WEMOS LOLIN D1 mini" width=280px
\n

There are several versions of WEMOS LOLIN D1 mini, which only differ in the
size of the flash memory and the MCU version used. All versions have a
microUSB port with flash / boot / reset logic that makes flashing much
easier. Their peripherals are equal and work with the default ESP8266 ESP-12x
board definition.

For more information, see [D1 Boards]
(https://docs.wemos.cc/en/latest/d1/d1_mini.html).

<center>
Board        | MCU       | Flash    | Antenna | Remark
-------------|-----------|----------|---------|--------
D1 mini V2   | ESP8266EX |  4 MByte | PCB     | retired
D1 mini V3   | ESP8266EX |  4 MByte | PCB     | |
D1 mini Lite | ESP8285   |  1 MByte | PCB     | |
D1 mini Pro  | ESP8266EX | 16 MByte | ceramic | |
</center>
\n

Following image shows the pinout of all WEMOS LOLIN D1 mini boards. It is
compatible with the WEMOS LOLIN D32 Pro ESP32 board.

@image html "https://gitlab.com/gschorcht/RIOT.wiki-Images/raw/master/esp8266/Wemos_D1_mini_pinout.png" "WEMOS LOLIN D1 mini pinout"

## NodeMCU DEVKIT {#esp8266_nodemcu_devkit_esp8266}

NodeMCU DEVKIT is an open-source hardware project hosted on
[GitHub](https://github.com/nodemcu/nodemcu-devkit-v1.0). Therefore,
there are a lot of clones available. The board was originally designed
for NodeMCU firmware development.

As the other boards described here, NodeMCU ESP12 is generic board that
uses ESP-12E module and breaks out all available GPIO pins. It has a
Micro-USB port including a flash/boot/reset logic which makes flashing
much easier.

@image html "https://raw.githubusercontent.com/nodemcu/nodemcu-devkit-v1.0/master/Documents/NodeMCU_DEVKIT_1.0.jpg" "NodeMCE DEVKIT V1.0" width=500px
\n

Following image shows the pinout of NodeMCU DEVKIT boards.

@image html "https://gitlab.com/gschorcht/RIOT.wiki-Images/raw/master/esp8266/NodeMCU_pinout.png" "NodeMCE DEVKIT V1.0 pinout"

## Adafruit Feather HUZZAH ESP8266 {#esp8266_adafruit_feather_huzzah_esp8266}

Feather is the new series of development boards from Adafruit.
[Adafruit Feather HUZZAH ESP8266](https://www.adafruit.com/product/2821)
is a ESP8266 based development board with built in WiFi, USB and battery
charging. As the other boards described here, Adafruit Feather HUZZAH ESP8266
is a generic board that uses an ESP-12x module and breaks out most of the
available GPIO pins. It has one additional LED connected to GPIO0 and a
Micro-USB port including a flash/boot/reset logic which makes flashing much
easier.

@image html "https://cdn-shop.adafruit.com/640x480/2821-04.jpg" "Adafruit Feather HUZZAH ESP8266" width=500px
\n

Following image shows the pinout of Adafruit Feather HUZZAH ESP8266.

@image html "https://gitlab.com/gschorcht/RIOT.wiki-Images/raw/master/esp8266/Adafruit_Huzzah_ESP8266_pinout.png" "Adafruit Feather HUZZAH ESP8266 pinout"
*/
