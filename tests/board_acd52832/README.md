# About
This is a test application for the Board ACD52832 with the following features:
 - MCP23017 16-Bit I/O Expander with I2C Interface.
This test application will initialize the MCP23017 with the following parameters:
 - I2C device address set to 0x20
 - Blinking LED 2

# Datasheet
http://ww1.microchip.com/downloads/en/DeviceDoc/20001952C.pdf

# Usage
Use the RTT feature in RIOT:
 -> boards -> Makefile.features -> FEATURES_PROVIDED += periph_rtt
 -> application -> Makefile -> USEMODULE += rtt_stdio
Setup for terminal 1:
 -> JLinkExe -> connect -> Device: NRF52 -> TIF: SWD -> Speed: 4000
Setup for terminal 2:
 -> JLinkRTTClient
