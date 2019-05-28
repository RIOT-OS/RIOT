include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove \
                             arduino-leonardo \
                             arduino-nano \
                             arduino-uno \
                             i-nucleo-lrwan1 \
                             nucleo-f031k6 \
                             nucleo-f042k6 \
                             nucleo-l031k6 \
                             nucleo-l053r8 \
                             nucleo-f030r8 \
                             stm32f0discovery \

BOARD_BLACKLIST := chronos \
                   msb-430 \
                   msb-430h \
                   telosb \
                   wsn430-v1_3b \
                   wsn430-v1_4 \
                   z1 \

TEST_ON_CI_WHITELIST += all

# Set vfs file and dir buffer sizes
CFLAGS += -DVFS_FILE_BUFFER_SIZE=56 -DVFS_DIR_BUFFER_SIZE=44
# Reduce LFS_NAME_MAX to 31 (as VFS_NAME_MAX default)
CFLAGS += -DLFS_NAME_MAX=31

USEMODULE += littlefs
USEMODULE += embunit

include $(RIOTBASE)/Makefile.include
