include ../Makefile.sys_common

# avrlibc's snprintf doesn't support uint64_t / int64_t and even fails
# to compile due to PRI*64 macros not being defined
FEATURES_BLACKLIST := arch_avr8

include $(RIOTBASE)/Makefile.include
