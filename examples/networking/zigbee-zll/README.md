# ZBOSS Zigbee ZLL example

RIOT integrates ZBOSS, the open source ZigBee stack. This example is used for
controlling ZigBee devices using Touchlink Commissioning and the ZigBee Cluster
Library.

Use the set_ch command to set it to one of the primary Touchlink channels
(11,15,20,25).
Take a ZigBee enabled light bulb and place it close to the microcontroller.
The run the tl_scan command. This will initiate a Touchlink commissioning procedure.
Wait a few seconds for the joining to complete.
Finally, zcl_toggle should toggle the light.

Note: If no encrypted key is displayed on the command line, the device is likely
on the wrong channel or not close enough to the bulb.
Also, ZBOSS is currently unable to clear some command buffers.
It is best practice to reboot the microcontroller after each tl_scan.
Use the zconfig command to view ZigBee information including the current channel.



