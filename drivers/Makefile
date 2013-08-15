MODULE = drivers

DIRS=
ifneq (,$(findstring powermon,$(USEMODULE)))
    DIRS += powermon
endif
ifneq (,$(findstring cc2420,$(USEMODULE)))
    DIRS += cc2420
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
ifneq (,$(findstring at86rf231,$(USEMODULE)))
    DIRS += at86rf231
endif
ifneq (,$(findstring gps_ublox,$(USEMODULE)))
    DIRS += gps_ublox
endif

all:
	@for i in $(DIRS) ; do $(MAKE) -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

# remove compilation products
clean::
	@for i in $(DIRS) ; do $(MAKE) -C $$i clean ; done ;




