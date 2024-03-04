include ../Makefile.drivers_common

USEMODULE += shell
USEMODULE += at
USEMODULE += at_urc

# Enable if the DCE is sending only \n for EOL
# CFLAGS += -DAT_RECV_EOL_1=""

# Enable this to test with echo off. Don't forget to disable echo in
# 'tests-with-config/emulated_dce.py' too!
# CFLAGS += -DCONFIG_AT_SEND_SKIP_ECHO=1

# we are printing from the event thread, we need more stack
CFLAGS += -DEVENT_THREAD_MEDIUM_STACKSIZE=1024

include $(RIOTBASE)/Makefile.include
