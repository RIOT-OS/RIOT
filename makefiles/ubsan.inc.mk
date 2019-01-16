# Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
#
# # Introduction
#
# This file contains makefile convenience support for gcc/clang's undefined
# behaviour sanitizer.
#
# # Overview
#
# Both gcc and clang allow generation on code that does runtime checks for
# undefined behavior (UB).
#
# E.g., the following code might trigger UB for some parameters:
#
#   void test(int foo) {
#     return (foo << 24);
#   }
#
#  In this case, the signed shift would be alright unless it would "push out"
#  bits to the left, with undefined runtime result. Using ubsan, this can be
#  caught.
#
# # How to use
#
# 1. build with "UBSAN_ENABLE=1". This will cause a trap on undefined
# behaviour.
#
# 2. build with "UBSAN_ENABLE=1 UBSAN_MODE=msg_exit" will print where and how
# the undefined behaviour occured, then exit with error code.  This is only
# available on native built with gcc.
#

# use this to switch on ubsan
UBSAN_ENABLE ?= 0

# trap, msg_exit, msg_recover
UBSAN_MODE ?= trap

ifeq (1,$(UBSAN_ENABLE))
  CFLAGS += -fsanitize=undefined

  ifeq (gnu,$(TOOLCHAIN))
    ifeq (native,$(BOARD))
      ifneq (,$(filter msg_%,$(UBSAN_MODE)))
        LINKFLAGS += -lubsan
        ifneq (msg_recover,$(UBSAN_MODE))
          CFLAGS += -fno-sanitize-recover=undefined
        endif
      else
        CFLAGS += -fsanitize-undefined-trap-on-error
      endif
    else
      # on real hardware, there's currently no runtime support.
      # so just crash when undefined behaviour is triggered.
      CFLAGS += -fsanitize-undefined-trap-on-error
    endif
  else
    # libubsan doesn't link properly when using clang.
    # thus when using llvm as toolchain, always generate traps.
    CFLAGS += -fsanitize-trap=undefined
  endif
endif
