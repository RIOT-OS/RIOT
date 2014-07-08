MODE = $(shell grep "RPL_DEFAULT_MOP RPL_NON_STORING_MODE" ../../include/rpl/rpl_config.h)
ifneq (,$(MODE))
	DIRS += rpl_nonstoring
else
	DIRS += rpl_storing
endif
include $(RIOTBASE)/Makefile.base
