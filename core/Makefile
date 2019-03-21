# exclude submodule sources from *.c wildcard source selection
SUBMODULE_SRC += cond.c
SUBMODULE_SRC += kernel_init.c
SUBMODULE_SRC += mbox.c
SUBMODULE_SRC += msg.c
SUBMODULE_SRC += mutex.c
SUBMODULE_SRC += rmutex.c
SUBMODULE_SRC += sched.c
SUBMODULE_SRC += st_kernel_init.c
SUBMODULE_SRC += st_mutex.c
SUBMODULE_SRC += st_sched.c
SUBMODULE_SRC += st_thread.c
SUBMODULE_SRC += thread.c
SUBMODULE_SRC += thread_flags.c

SRC := $(filter-out $(SUBMODULE_SRC),$(wildcard *.c))

# enable submodules
SUBMODULES := 1

include $(RIOTBASE)/Makefile.base
