include ../Makefile.sys_common

# avrlibc's snprintf doesn't support uint64_t / int64_t and even fails
# to compile due to PRI*64 macros not being defined
FEATURES_BLACKLIST := arch_avr8

# This enables support for printing 64 bit numbers. You can comment this out
# to shrink the test app. The test will skip the corresponding test
# automatically if printf_long_long is not used.
USEMODULE += printf_long_long

include $(RIOTBASE)/Makefile.include
