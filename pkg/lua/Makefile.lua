SRC := $(filter-out loadlib.c lua.c luac.c,$(wildcard *.c))

ifneq (llvm, $(TOOLCHAIN))
  CFLAGS += -fstack-usage -fconserve-stack
endif

CFLAGS += -DLUA_MAXCAPTURES=16 -DL_MAXLENNUM=50

#    Enable these options to debug stack usage
#          -Wstack-usage=128 -Wno-error=stack-usage=128

include $(RIOTBASE)/Makefile.base
ifneq (,$(filter -Wformat-nonliteral -Wformat=2, $(CFLAGS)))
  CFLAGS += -Wno-format-nonliteral
endif
