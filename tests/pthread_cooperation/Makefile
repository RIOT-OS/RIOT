include ../Makefile.tests_common

# nucleo-f303k8 doesn't have enough RAM to run the test so we reduce the stack
# size for every thread
ifneq (,$(filter nucleo-f303k8 nucleo-f334r8,$(BOARD)))
  CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=512
endif

# The test compiles, but these boards have too little RAM for
# dynamic allocation of thread stacks
BOARD_BLACKLIST := arduino-nano arduino-duemilanove \
                   arduino-uno nucleo-f031k6 stm32f030f4-demo

USEMODULE += posix_headers
USEMODULE += pthread

include $(RIOTBASE)/Makefile.include
