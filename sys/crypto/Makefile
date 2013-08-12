INCLUDES = -I../include
MODULE = crypto

include $(RIOTBASE)/Makefile.base

ifeq ($(strip $(BOARD)),msba2)
	$(error "sha256 produces wrong results on msba2 currently, aborting...")
endif
