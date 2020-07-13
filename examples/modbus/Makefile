# name of your application
APPLICATION = hello-world

# If no BOARD is found in the environment, use this default:
BOARD ?= bluepill

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

FEATURES_USED += gpio
USEMODULE += $(filter periph_%,$(FEATURES_USED))
USEMODULE += modbus_rtu  
USEMODULE += xtimer  

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include

LOADER=$(RIOTBASE)/dist/tools/jlink/jlink.sh
# LOADER=st-flash

export JLINK_DEVICE=$(CPU_MODEL)
export FLASH_ADDR=0x08000000
load: hexfile
	$(LOADER) flash $(HEXFILE)
	# $(LOADER)  --reset --format ihex write $(HEX)

.PHONY: load
