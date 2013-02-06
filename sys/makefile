
INCLUDES = -Iinclude -I$(RIOTBASE)/drivers/ -I$(RIOTBASE)/drivers/include -Ilib -I../.. -I../cpu/$(CPU)/include -I../cpu/ -Ilib/cmdengine -Inet -I../hal/include -I../core/include -Iconfig

MODULE =sys
ifneq (,$(findstring config,$(USEMODULE)))
    DIRS += config
endif
ifneq (,$(findstring net,$(USEMODULE)))
    DIRS += net
endif
ifneq (,$(findstring lib,$(USEMODULE)))
    DIRS += lib
endif
ifneq (,$(findstring logd,$(USEMODULE)))
    DIRS += logd
endif
ifneq (,$(findstring cmdd,$(USEMODULE)))
    DIRS += cmdd
endif
ifneq (,$(findstring mprint,$(USEMODULE)))
    DIRS += mprint
endif
ifneq (,$(findstring sync_read,$(USEMODULE)))
    DIRS += sync_read
endif
ifneq (,$(findstring syslog,$(USEMODULE)))
    DIRS += syslog
endif
ifneq (,$(findstring sysmon,$(USEMODULE)))
    DIRS += sysmon
endif
ifneq (,$(findstring utimer,$(USEMODULE)))
    DIRS += utimer
endif
ifneq (,$(findstring mqueue,$(USEMODULE)))
    DIRS += mqueue
endif
ifneq (,$(findstring shell,$(USEMODULE)))
    DIRS += shell
endif
ifneq (,$(findstring tracelog,$(USEMODULE)))
    DIRS += tracelog
endif
ifneq (,$(findstring transceiver,$(USEMODULE)))
    DIRS += transceiver
endif

all: $(BINDIR)$(MODULE).a 
	@for i in $(DIRS) ; do $(MAKE) -C $$i ; done ;

include $(RIOTBASE)/makefile.base

# remove compilation products
clean::
	@for i in $(DIRS) ; do $(MAKE) -C $$i clean ; done ;



