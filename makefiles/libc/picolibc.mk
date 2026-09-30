ifneq (,$(filter picolibc,$(USEMODULE)))
  # Test if picolibc.specs is available
  ifeq ($(shell $(LINK) -specs=picolibc.specs -E - 2>/dev/null >/dev/null </dev/null ; echo $$?),0)
    USE_PICOLIBC = 1
    ifeq ($(shell LC_ALL=C $(LINK) $(RIOTTOOLS)/testprogs/minimal_linkable.c -o /dev/null -lc -specs=picolibc.specs -Wall -Wextra -pedantic 2>&1 | grep -q "use of wchar_t values across objects may fail" ; echo $$?),0)
        CFLAGS += -fshort-wchar
        LINKFLAGS += -Wl,--no-wchar-size-warning
    endif
  else
    BUILDDEPS += _missing-picolibc
  endif
endif

.PHONY: _missing-picolibc

_missing-picolibc:
	@$(Q)echo "picolibc was selected to be build but no picolibc.specs could be found"
	@$(Q)echo "you might want to install "picolibc" for "$(TARGET_ARCH)""
	@$(Q)echo "or add "FEATURES_BLACKLIST += picolibc" to Makefile)"
	@$(COLOR_ECHO) "$(COLOR_RED)check your installation or build configuration.$(COLOR_RESET)"
	@$(Q)exit 1

ifeq (1,$(USE_PICOLIBC))
  LINKFLAGS += -specs=picolibc.specs
  ifeq (llvm,$(TOOLCHAIN))
    # clang does not support spec files, the include directories of picolibc
    # are added via GCC_C_INCLUDES instead (see makefiles/toolchain/llvm.inc.mk)
    GCC_SPECS += -specs=picolibc.specs
  else
    CFLAGS += -specs=picolibc.specs
  endif
  ifeq (,$(filter printf_float scanf_float,$(USEMODULE)))
    CFLAGS += -DPICOLIBC_INTEGER_PRINTF_SCANF
    LINKFLAGS += -DPICOLIBC_INTEGER_PRINTF_SCANF
  endif
endif

LINKFLAGS += -lc
