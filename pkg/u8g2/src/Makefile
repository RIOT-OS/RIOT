MODULE = pkg-u8g2

DIRS += csrc

# SDL can be used as a virtual display, but is for native target only.
ifneq (,$(filter u8g2_sdl,$(USEMODULE)))
  DIRS += sys/sdl/common
endif

# UTF8 virtual display is not part of core. Therefore it is a separate module.
ifneq (,$(filter u8g2_utf8,$(USEMODULE)))
  DIRS += sys/utf8/common
endif

# Compiling U8g2 will generate a lot of compiler warnings, which are treated
# as errors. For the sake of simplicity, ignore them.
CFLAGS += -Wno-empty-translation-unit \
          -Wno-newline-eof \
          -Wno-unused-parameter \
          -Wno-unused \
          -Wno-overlength-strings \
          -Wno-pointer-arith

include $(RIOTBASE)/Makefile.base
