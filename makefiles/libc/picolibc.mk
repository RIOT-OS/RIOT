ifneq (,$(filter picolibc,$(USEMODULE)))
  # Test if picolibc.specs is available
  ifeq ($(shell $(LINK) -specs=picolibc.specs -E - 2>/dev/null >/dev/null </dev/null ; echo $$?),0)
    USE_PICOLIBC = 1
    ifeq ($(shell echo "int main(){} void _exit(int n) {(void)n;while(1);}" | LC_ALL=C $(CC) -xc - -o /dev/null -lc -specs=picolibc.specs -Wall -Wextra -pedantic 2>&1 | grep -q "use of wchar_t values across objects may fail" ; echo $$?),0)
        CFLAGS += -fshort-wchar
        LINKFLAGS += -Wl,--no-wchar-size-warning
    endif
  else
    $(warning picolibc was selected to be build but no picolibc.spec could be found)
    $(warning you might want to install "picolibc" for "$(TARGET_ARCH)")
    $(warning or add "FEATURES_BLACKLIST += picolibc" to Makefile)
    $(error   check your installation or build configuration.)
  endif
endif

ifeq (1,$(USE_PICOLIBC))
  LINKFLAGS += -specs=picolibc.specs
  CFLAGS += -specs=picolibc.specs
  ifeq (,$(filter printf_float scanf_float,$(USEMODULE)))
    CFLAGS += -DPICOLIBC_INTEGER_PRINTF_SCANF
    LINKFLAGS += -DPICOLIBC_INTEGER_PRINTF_SCANF
  endif
endif

LINKFLAGS += -lc
