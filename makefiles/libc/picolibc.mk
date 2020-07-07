ifneq (,$(filter picolibc,$(USEMODULE)))
  # Test if picolibc.specs is available
  ifeq ($(shell $(LINK) -specs=picolibc.specs -E - 2>/dev/null >/dev/null </dev/null ; echo $$?),0)
    USE_PICOLIBC = 1
    ifeq ($(shell echo "int main(){} void _exit(int n) {(void)n;while(1);}" | LC_ALL=C $(CC) -xc - -o /dev/null -lc -specs=picolibc.specs -Wall -Wextra -pedantic 2>&1 | grep -q "use of wchar_t values across objects may fail" ; echo $$?),0)
        CFLAGS += -fshort-wchar
        LINKFLAGS += -Wl,--no-wchar-size-warning
    endif
  endif
endif

ifeq (1,$(USE_PICOLIBC))
  LINKFLAGS += -specs=picolibc.specs
endif

LINKFLAGS += -lc

ifeq (1,$(USE_PICOLIBC))
  PICOLIBC_INCLUDE_DIR ?= /usr/local/picolibc/$(TARGET_ARCH)/include

  INCLUDES += -isystem $(PICOLIBC_INCLUDE_DIR)
endif

