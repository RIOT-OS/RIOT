ifneq (, $(filter chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1, $(BOARD)))
  CFLAGS += -DMODULE_UART0_GETCHAR
endif

include $(RIOTBASE)/Makefile.base
