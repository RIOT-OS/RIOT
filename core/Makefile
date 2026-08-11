
MODULE_DEFS_NEEDED += \
  core_idle_thread \
  core_msg \
  core_mutex_debug \
  core_mutex_priority_inheritance \
  core_thread_flags \
  cortexm_stack_limit \
  mpu_stack_guard \
  sched_cb \
  sched_runq_callback \
  test_utils_print_stack_usage \
  #
# exclude submodule sources from *.c wildcard source selection
SRC := $(filter-out mbox.c msg.c msg_bus.c thread.c thread_flags.c thread_flags_group.c,$(wildcard *.c))

# enable submodules
SUBMODULES := 1

# this module is expected to pass static analysis
MODULE_SUPPORTS_STATIC_ANALYSIS := 1

MODULE_DEFS_NEEDED := core_thread_flags

include $(RIOTBASE)/Makefile.base
