include ../Makefile.drivers_common

FEATURES_OPTIONAL += periph_can

USEMODULE += shell
USEMODULE += can
USEMODULE += isrpipe

include $(RIOTBASE)/Makefile.include

# Loop delay
ifneq (,$(filter fdcan,$(USEMODULE)))
	ifneq (1,$(RIOT_CI_BUILD))
		# Check your CAN transceiver datasheet to apply the good loop delay in nanoseconds.
		# This configuration is mandatory if you are using CAN FD with bitrate switching.
		# Can be configured here or through Kconfig
		#CFLAGS += -DCONFIG_FDCAN_DEVICE_TRANSCEIVER_LOOP_DELAY=0

	# Allow only a default value for CI
	else
		CFLAGS += -DCONFIG_FDCAN_DEVICE_TRANSCEIVER_LOOP_DELAY=0
	endif
endif
