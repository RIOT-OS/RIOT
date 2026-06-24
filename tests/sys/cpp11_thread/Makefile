include ../Makefile.sys_common

# include color echo macros
include $(RIOTMAKE)/utils/ansi.mk
include $(RIOTMAKE)/color.inc.mk

USEMODULE += cpp11-compat
USEMODULE += ztimer64_usec
USEMODULE += timex

# Boards with little RAM (~8KB) can't handle threads with the large default
# stack size.
# Beware: The reduced stack size will also suppress the thread stack usage
#         printout!
LOW_MEMORY_BOARDS := \
    nucleo-c031c6 \
    nucleo-f042k6 \
    nucleo-g031k8 \
    nucleo-l031k6 \
    stm32g0316-disco \
    weact-g030f6 \
    #

ifneq (,$(filter $(BOARD),$(LOW_MEMORY_BOARDS)))
  $(shell $(COLOR_ECHO) "$(COLOR_YELLOW)Using a reduced stack size for low" \
                        "memory board \"$(BOARD)\"!$(COLOR_RESET)" 1>&2)

  CFLAGS += -DTHREAD_STACKSIZE_MAIN=512
  DISABLE_MODULE += test_utils_print_stack_usage
endif

include $(RIOTBASE)/Makefile.include
