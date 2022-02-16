# Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file contains support for UBSan, the undefined behaviour sanitizer
# provided by gcc and clang.
#
# Please see doc/doxygen/src/debugging-aids.md for more info.

# trap, msg_exit, msg_recover
UBSAN_MODE ?= msg_exit

CFLAGS_UBSAN = -fsanitize=undefined

ifeq (gnu,$(TOOLCHAIN))
  ifeq (native,$(BOARD))
    ifneq (,$(filter msg_%,$(UBSAN_MODE)))
      LINKFLAGS_UBSAN += -lubsan
      ifneq (msg_recover,$(UBSAN_MODE))
        CFLAGS_UBSAN += -fno-sanitize-recover=undefined
      endif
    else
      CFLAGS_UBSAN += -fsanitize-undefined-trap-on-error
    endif
  else
    # on real hardware, there's currently no runtime support.
    # so just crash when undefined behaviour is triggered.
    CFLAGS_UBSAN += -fsanitize-undefined-trap-on-error
  endif
else
  # libubsan doesn't link properly when using clang.
  # thus when using llvm as toolchain, always generate traps.
  CFLAGS_UBSAN += -fsanitize-trap=undefined
endif

all-ubsan: CFLAGS += $(CFLAGS_UBSAN)
all-ubsan: LINKFLAGS += $(LINKFLAGS_UBSAN)
all-ubsan: all
