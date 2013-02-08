DIRS=
ifneq (,$(findstring powermon,$(USEMODULE)))
    DIRS += powermon
endif
ifneq (,$(findstring sht11,$(USEMODULE)))
    DIRS += sht11
endif
ifneq (,$(findstring ltc4150,$(USEMODULE)))
    DIRS += ltc4150
endif
ifneq (,$(findstring cc110x,$(USEMODULE)))
    ifneq (,$(findstring cc110x_ng,$(USEMODULE)))
    	DIRS += cc110x_ng
    else
    	DIRS += cc110x
    endif
endif
ifneq (,$(findstring gps_ublox,$(USEMODULE)))
    DIRS += gps_ublox
endif

all:
	@for i in $(DIRS) ; do $(MAKE) -C $$i ; done ;

include $(RIOTBASE)/makefile.base

# remove compilation products
clean::
	@for i in $(DIRS) ; do $(MAKE) -C $$i clean ; done ;




