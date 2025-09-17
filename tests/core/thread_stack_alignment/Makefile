include ../Makefile.core_common

FEATURES_REQUIRED += float_math

USEMODULE += printf_float

# On ESP* a custom sched_task_exit() is used that does not implement
# test_utils_print_stack_usage yet, which is needed by the test script
# to measure the worst case memory wasting when stacks are unaligned.
FEATURES_BLACKLIST += arch_esp

include $(RIOTBASE)/Makefile.include
