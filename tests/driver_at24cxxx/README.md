Test for at24cxxx EEPROMS
=========================
This tests general functionality of at24cxxx EEPROMS.
To select the right EEPROM, for example at24c256, do `AT24CXXX_DEV=at24c256 BOARD=<your board> make flash term`.
The at24c256 is selected by default.
To see a list of supported devices, take a look at `drivers/at24cxxx/include/at24cxxx_defines.h`

Expected result
===============
You should see that all tests print `[SUCCESS]` and that the tests finish.
