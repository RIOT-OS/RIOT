# avoid clang warning in tests-ztimer/tests-ztimer-extend.c:141
ifeq (llvm,$(TOOLCHAIN))
  CFLAGS += -Wno-gnu-folding-constant
endif

include $(RIOTBASE)/Makefile.base
