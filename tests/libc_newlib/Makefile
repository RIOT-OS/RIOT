include ../Makefile.tests_common

USEMODULE += embunit

include $(RIOTBASE)/Makefile.include

# Compile time tests
.PHONY: compile-test test-newlib-nano
# The tests should only be executed in build environment
ifneq ($(BUILD_IN_DOCKER),1)
all: compile-test
endif

ifneq (,$(filter newlib,$(USEMODULE)))
  COMPILE_TESTS += test-newlib
  ifneq (,$(filter newlib_nano,$(USEMODULE)))
    CMP_OP = =
  else
    CMP_OP = !=
  endif
endif

# Newlib-nano removed the integer only 'iprintf' functions which are now mapped
# to printf.
#
# `main.c` uses both functions to be sure they are included.
# It also duplicates the same test in C to verify it at runtime.
#
# Source:
#
# https://github.com/32bitmicro/newlib-nano-1.0/blob/f157c994b9a2c4bd8d0cfe9fe8fdd9cd54f8c63b/newlib/README.nano#L32

test-newlib: $(ELFFILE)
	$(Q)test -f $^
	$(Q)\
	PRINTF_ADDR=$$($(NM) $^ | sed -n '/ printf$$/ s/ .*//p');\
	IPRINTF_ADDR=$$($(NM) $^ | sed -n '/ iprintf$$/ s/ .*//p');\
	echo "Test: comparing addresses of 'printf' and 'iprintf' symbols";\
	if test "$${PRINTF_ADDR}" $(CMP_OP) "$${IPRINTF_ADDR}" ; then \
	  echo "[SUCCESS] '$${PRINTF_ADDR}' $(CMP_OP) '$${IPRINTF_ADDR}' is True"; \
	else \
	  echo "[FAILED] '$${PRINTF_ADDR}' $(CMP_OP) '$${IPRINTF_ADDR}' is False"; \
	  exit 1; \
	fi

compile-test: $(COMPILE_TESTS)
