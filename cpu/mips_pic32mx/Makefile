ifeq ("$(OS)","Windows_NT")
MIPS_ELF_ROOT?=C:\cross-mips-mti-elf
else
MIPS_ELF_ROOT?=/opt/imgtec/Toolchains/mips-mti-elf/2016.05-03
endif

# define the module that is build
MODULE = cpu

# add a list of subdirectories, that should also be build
DIRS += periph $(RIOTCPU)/mips32r2_common

include $(RIOTBASE)/Makefile.base


