APPLICATION = pm_layered
include ../Makefile.tests_common

# You can define PM_BLOCKER_INITIAL if not defined by the cpu
#CFLAGS += '-DPM_BLOCKER_INITIAL={ .val_u32 = 0x0101 }'

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

FEATURES_REQUIRED += periph_rtc

include $(RIOTBASE)/Makefile.include
