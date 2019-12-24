Expected result
===============
This test periodically puts the device into deep sleep mode.
During deep sleep only the backup RAM is retained and the device is
reset upon leaving deep sleep.
To test the memory retention, a counter is incemented with each wake-up.

Background
==========
Test the functionality of a platforms Backup RAM implementation.
