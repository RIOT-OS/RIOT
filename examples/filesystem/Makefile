# name of your application
APPLICATION = filesystem

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# Blacklisting msp430-based boards, as file syscalls are not supported
BOARD_BLACKLIST :=  chronos \
                    msb-430 \
                    msb-430h \
                    telosb \
                    wsn430-v1_3b \
                    wsn430-v1_4 \
                    z1 \
                    #

BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Use MTD (flash layer)
USEMODULE += mtd

# Use VFS
USEMODULE += vfs

# Use a file system
USEMODULE += littlefs
# USEMODULE += spiffs
# USEMODULE += fatfs
USEMODULE += constfs
# USEMODULE += devfs

# Set file systems specific variables
ifneq (,$(filter littlefs, $(USEMODULE)))
    CFLAGS += -DVFS_FILE_BUFFER_SIZE=52 -DVFS_DIR_BUFFER_SIZE=44
else ifneq (,$(filter spiffs, $(USEMODULE)))
    SPIFFS_NB_FD ?= 8
    CFLAGS += '-DSPIFFS_FS_FD_SPACE_SIZE=(32 * $(SPIFFS_NB_FD))'
endif

include $(RIOTBASE)/Makefile.include
