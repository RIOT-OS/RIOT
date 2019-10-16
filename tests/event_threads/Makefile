include ../Makefile.tests_common

USEMODULE += event_thread_highest event_thread_medium event_thread_lowest

# arm7 has an issue with it's ISR_STACKSIZE define
FEATURES_BLACKLIST += arch_arm7

include $(RIOTBASE)/Makefile.include
