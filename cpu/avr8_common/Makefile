# define the module that is build (not strictly necessary)
MODULE = avr8_common

MODULE_DEFS_NEEDED += \
  avr_libc_extra \
  core_thread \
  dbgpin \
  puf_sram \
  #

# add a list of subdirectories, that should also be build
DIRS = periph avr_libc_extra

include $(RIOTBASE)/Makefile.base
