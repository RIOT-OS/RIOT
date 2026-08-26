The driver works only if the ATA8510 EEPROM is configured with the provided
firmware. To flash it, use avrdude like this:

"""
avrdude -c atmelice_isp -p ata8510 -C +ata8510.conf -U eeprom:w:firmware.hex:i
"""

Tested with:
- avrdude 6.3 (from http://www.nongnu.org/avrdude/)
- Debian Jessie avrdude 6.1-2
- Debian Buster avrdude 6.3-20171130

BUGS:
- after flashing, the USBHID driver is left in an unknown state; you can detach
  and reattach the programmer, or use the provided flash.sh script to reset it.
