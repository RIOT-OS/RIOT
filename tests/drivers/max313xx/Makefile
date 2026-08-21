include ../Makefile.drivers_common

# The test supports both MAX31331 and MAX31343. Comment out the variant you have.
DRIVER ?= max31331
#DRIVER ?= max31343

USEMODULE += $(DRIVER)

USEMODULE += ztimer_sec
USEMODULE += shell
USEMODULE += shell_cmds_default

include $(RIOTBASE)/Makefile.include
