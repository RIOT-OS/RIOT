# ZBOSS Zigbee remote control example

RIOT integrates ZBOSS, the open source ZigBee stack. This example is used for
controlling ZigBee devices using Touchlink Commissioning and the
ZigBee Cluster Library.

## Commands for Touchlink Initiator

- ```tl_scan``` performs a Touchlink Scan. Be sure to place the bulb as close as \
possible to the Microcontroller.
- ```dev_info``` shows information about the joined device.
- ```zcl_toggle <4 digit short address> <endpoint>``` toggles the bulb on and off.

## Other Commands

- ```zconfig``` shows a more detailed state of ZBOSS.
- ```reset``` resets persistant storage and reboots the Microcontroller.
- ```zigp <hex bytes>``` takes a Zigbee packet and injects it into ZBOSS (as if it was received).
- ```inj <hex bytes>``` takes a Zigbee packet and sends it out the transmitter.

# Tested Boards

- nrf52840dk

Note: ZBOSS is currently unable to clear some command buffers.
It is best practice to reboot the microcontroller after each tl_scan.

For more information see [ZBOSS](https://github.com/Lukas-Luger/zboss)
