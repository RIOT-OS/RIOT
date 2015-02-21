ifneq (,$(filter native_gpio_virtual,$(USEMODULE)))
	DIRS += gpio/virtual
endif
ifneq (,$(filter native_gpio_sysfs,$(USEMODULE)))
	DIRS += gpio/sysfs
endif
include $(RIOTBASE)/Makefile.base
