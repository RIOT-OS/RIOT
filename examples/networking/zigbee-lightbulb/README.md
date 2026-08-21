# ZBOSS Zigbee lightbulb example

RIOT integrates ZBOSS, the open source ZigBee stack. This example is used for
controlling the integrated LED from external Zigbee devices using Touchlink Commissioning,
Permit Joining and the ZigBee Cluster Library.

## Commands

- ```zconfig``` shows a more detailed state of ZBOSS.
- ```reset``` resets persistent storage and reboots the Microcontroller.
- ```zigp <hex bytes>``` takes a Zigbee packet and injects it into ZBOSS (as if it was received).
- ```inj <hex bytes>``` takes a Zigbee packet and sends it out the transmitter.

# Tested Boards

- nrf52840dk

Note: ZBOSS is currently unable to clear some command buffers.
It is best practice to reboot the microcontroller after each tl_scan.

For more information see [ZBOSS](https://github.com/Lukas-Luger/zboss)
