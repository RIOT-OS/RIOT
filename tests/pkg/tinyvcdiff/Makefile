include ../Makefile.pkg_common

BLOBS += source.bin delta.bin target.bin

# Only run on native ...
TEST_ON_CI_WHITELIST += native32 native64

USEMODULE += embunit

# Library under test
USEPKG += tinyvcdiff

# MTD backend
USEMODULE += mtd

# VFS backend
USEPKG += littlefs2

include $(RIOTBASE)/Makefile.include
