APPLICATION = twr-aloha

# If no BOARD is found in the environment, use this default:
BOARD ?= dwm1001

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../..

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Include uwb-core, uwb-dw1000
USEPKG += uwb-core
USEPKG += uwb-dw1000
# Use event threads instead of a custom event queue
USEMODULE += uwb-core_event_thread
CFLAGS += -DEVENT_THREAD_MEDIUM_STACKSIZE=THREAD_STACKSIZE_DEFAULT

# Include all ranging algorithms
USEMODULE += uwb-core_twr_ss
USEMODULE += uwb-core_twr_ss_ack
USEMODULE += uwb-core_twr_ss_ext
USEMODULE += uwb-core_twr_ds
USEMODULE += uwb-core_twr_ds_ext
## Uncomment to enable trx info such as rssi, los, fppl calculation
# USEMODULE += uwb-core_rng_trx_info

# System modules used by this application
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += l2util
USEMODULE += event_callback
USEMODULE += event_timeout_ztimer
USEMODULE += event_periodic
USEMODULE += test_utils_result_output

# Collision between defines of Silabs SDK and mynewt-core
BOARD_BLACKLIST := xg23-pk6068a

# All uwb-core applications need to enable `-fms-extensions`
CFLAGS += -fms-extensions
ifneq (,$(filter llvm,$(TOOLCHAIN)))
  CFLAGS += -Wno-microsoft-anon-tag
endif

include $(RIOTBASE)/Makefile.include
