MODULE = platform

# Files adapted for RIOT live in this directory, the rest is taken
# unchanged from the downloaded upstream POSIX platform.
UD3TN_POSIX_DIR = $(UD3TN_SRC_DIR)/components/platform/posix

SRC_LOCAL = hal_task.c hal_semaphore.c hal_time.c \
            hal_platform.c socket_util.c 
SRC_UPSTREAM = hal_io.c hal_queue.c simple_queue.c pipe_queue_util.c 

SRC = $(SRC_LOCAL) $(SRC_UPSTREAM)
VPATH += $(UD3TN_POSIX_DIR)

include $(RIOTBASE)/Makefile.base
