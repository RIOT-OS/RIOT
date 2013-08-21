INCLUDES = -I../include
MODULE = crypto

include $(RIOTBASE)/Makefile.base

ifeq ($(strip $(BOARD)),msba2)
$(warning sha256 produces wrong results on msba2 with our old toolchain)
endif

