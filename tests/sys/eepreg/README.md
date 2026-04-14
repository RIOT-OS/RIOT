# EEPROM registry (eepreg) test

This test will verify the functionality of the eepreg module.

WARNING: This will write to your EEPROM and probably corrupt any data that is
there!

Add `CFLAGS += -DTEST_EEPREG_MTD=MTD_<X>` if a different MTD should be used than MTD_0.
