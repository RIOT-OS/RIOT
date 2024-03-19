PKG_NAME=micro-ecc
PKG_URL=https://github.com/kmackay/micro-ecc.git
PKG_VERSION=4b1709c17abbe938d6d8811f4c7c5d082a144799
PKG_LICENSE=BSD-2-Clause

include $(RIOTBASE)/pkg/pkg.mk

# With LLVM register allocation fails. We can tell LLVM to spend more time
# on trying to allocate registers using the following snippet:
ifeq (llvm,$(TOOLCHAIN))
  CFLAGS += -mllvm -exhaustive-register-search
endif

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
