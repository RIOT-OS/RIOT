DIRS += $(dir $(wildcard $(addsuffix /Makefile, $(USEMODULE))))

ifneq (,$(filter saul_init_devs,$(USEMODULE)))
  DIRS += saul/init_devs
endif

include $(RIOTBASE)/Makefile.base
