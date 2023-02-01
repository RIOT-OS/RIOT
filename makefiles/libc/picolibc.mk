ifneq (,$(filter picolibc,$(USEMODULE)))
  # Test if picolibc.specs is available
  ifeq ($(shell $(LINK) -specs=picolibc.specs -E - 2>/dev/null >/dev/null </dev/null ; echo $$?),0)
    USE_PICOLIBC = 1
    ifeq (LC_ALL=C $(LINK) $(RIOTTOOLS)/testprogs/minimal_linkable.c -o /dev/null -lc -specs=picolibc.specs -Wall -Wextra -pedantic 2>&1 | grep -q "use of wchar_t values across objects may fail" ; echo $$?),0)
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
  CFLAGS += -specs=picolibc.specs
  ifeq (,$(filter printf_float scanf_float,$(USEMODULE)))
    CFLAGS += -DPICOLIBC_INTEGER_PRINTF_SCANF
    LINKFLAGS += -DPICOLIBC_INTEGER_PRINTF_SCANF
  endif
  # For some reason segments with RWX permissions will be created with
  # picolibc. But since (as of now) RIOT only supports disabling the execute of
  # all RAM via the `mpu_noexec_ram` module, permissions of the segments are
  # ignored anyway. So for now, we just simply disable the warning.
  LINKER_SUPPORTS_NOWARNRWX ?= $(shell LC_ALL=C $(LINK) $(RIOTTOOLS)/testprogs/minimal_linkable.c -o /dev/null -lc -Wall -Wextra -pedantic -Wl,--no-warn-rwx-segments 2> /dev/null && echo 1 || echo 0)
  ifeq (1,$(LINKER_SUPPORTS_NOWARNRWX))
    LINKFLAGS += -Wl,--no-warn-rwx-segments
  endif
endif

LINKFLAGS += -lc
