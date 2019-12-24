include ../Makefile.tests_common

# Modules that will have an impact on the size of the TCB (thread_t):
#
# disabled by default, enable on demand:
ifneq (,$(USE_FLAGS))
  USEMODULE += core_thread_flags
endif
#
# enabled by default, disable on demand:
ifneq (,$(NO_MSG))
  DISABLE_MODULE += core_msg
endif

include $(RIOTBASE)/Makefile.include
