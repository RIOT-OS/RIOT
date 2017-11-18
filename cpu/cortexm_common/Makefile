# thread_arch.c's inline assembler breaks when compiling with link time
# optimization. see https://github.com/RIOT-OS/RIOT/issues/5774.
SRC_NOLTO += thread_arch.c

# see https://github.com/RIOT-OS/RIOT/issues/5775.
SRC_NOLTO += vectors_cortexm.c

DIRS = periph

include $(RIOTBASE)/Makefile.base
