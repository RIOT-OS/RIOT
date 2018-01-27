MODULE = mips32r2_common

DIRS = periph

ifneq (,$(filter newlib_syscalls_mips_uhi,$(USEMODULE)))
  DIRS += newlib_syscalls_mips_uhi
endif

include $(RIOTBASE)/Makefile.base
