include ../Makefile.core_common

USEMODULE += ztimer_sec

# make sure ISR stack is big enough to handle debug printf
CFLAGS += -DISR_STACKSIZE="(THREAD_STACKSIZE_MINIMUM+THREAD_EXTRA_STACKSIZE_PRINTF)"

# This test requires setting ISR_STACKSIZE which is incompatible with the ESP toolchain
FEATURES_BLACKLIST += arch_esp32 arch_esp8266

include $(RIOTBASE)/Makefile.include
