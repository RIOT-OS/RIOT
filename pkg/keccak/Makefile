PKG_NAME    := keccak
PKG_URL     := https://github.com/XKCP/XKCP
PKG_VERSION := bc592d1780a6b3204c26c6f872552ace8fc6fd65
PKG_LICENSE := PD

PKG_BUILD_OUT_OF_SOURCE ?= 0
include $(RIOTBASE)/pkg/pkg.mk

ifeq ($(CPU),native)
  CPU_ARCHIVE_NAME := Native
else
ifeq ($(CPU_ARCH),armv6m)
  CPU_ARCHIVE_NAME := ARMv6M
else
ifeq ($(CPU_ARCH),armv7m)
  CPU_ARCHIVE_NAME := ARMv7M
endif
endif
endif

ifeq (llvm,$(TOOLCHAIN))
  CFLAGS += -Wno-unused-command-line-argument
endif

all: $(BINDIR)/keccak.a

$(BINDIR)/keccak.a: $(PKG_BUILD_DIR)/bin/KeccakHash$(CPU_ARCHIVE_NAME).a
	@cp $(PKG_BUILD_DIR)/bin/KeccakHash$(CPU_ARCHIVE_NAME).a $(BINDIR)/keccak.a

$(PKG_BUILD_DIR)/bin/KeccakHash$(CPU_ARCHIVE_NAME).a:
	@cp -r $(RIOTBASE)/pkg/keccak/contrib/Modes $(PKG_BUILD_DIR)
	$(MAKE) KeccakHash$(CPU_ARCHIVE_NAME).a -C $(PKG_BUILD_DIR)
