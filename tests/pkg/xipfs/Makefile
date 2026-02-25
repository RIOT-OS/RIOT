include ../Makefile.pkg_common

BOARD ?= dwm1001

SLOT_AUX_LEN := 0x8000

USEPKG += xipfs
USEMODULE += xipfs
USEMODULE += saul_default

TOOLCHAINS_BLACKLIST += llvm

CFLAGS += -DXIPFS_ENABLE_SAFE_EXEC_SUPPORT

BLOBS += minimal.fae

include $(RIOTBASE)/Makefile.include
