MODULE = core_lib

MODULE_DEFS_NEEDED += \
  auto_init \
  backtrace \
  core_idle_thread \
  core_thread \
  periph_init_leds \
  periph_pm \
  ps \
  stdio_null \
  test_utils_main_exit_cb \
  test_utils_print_stack_usage \
  usb_board_reset \
  vfs \
  #

# enable submodules
SUBMODULES := 1
# set this so the panic and init code is available as "core_panic" and "core_init"
# instead of "core_lib_..."
BASE_MODULE = core

# exclude submodule sources from *.c wildcard source selection
SRC := $(filter-out panic.c init.c,$(wildcard *.c))

MODULE_DEFS_NEEDED := backtrace

include $(RIOTBASE)/Makefile.base
