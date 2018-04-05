MODULE = pkg-ucglib

DIRS += csrc

# SDL can be used as a virtual display, but is for native target only.
ifneq (,$(filter ucglib_sdl,$(USEMODULE)))
  DIRS += sys/sdl/dev
endif

# Compiling Ucglib will generate a lot of compiler warnings, which are treated
# as errors. For the sake of simplicity, ignore them.
CFLAGS += -Wno-empty-translation-unit \
          -Wno-newline-eof \
          -Wno-unused-parameter \
          -Wno-unused \
          -Wno-overlength-strings \
          -Wno-pointer-arith

include $(RIOTBASE)/Makefile.base
