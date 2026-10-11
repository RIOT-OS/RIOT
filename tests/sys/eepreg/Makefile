include ../Makefile.sys_common

USEMODULE += eepreg
# EEPREG could be run on other MTD than EEPROM but not all are guaranteed to work.
# Alternatives are mtd_at24cxxx and mtd_at25cxxx, but the test is only run for
# boards with internal EEPROM.
USEMODULE += mtd_eeprom

include $(RIOTBASE)/Makefile.include
