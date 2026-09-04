LOG_DIRS = $(subst log_,,$(filter log_%, $(USEMODULE)))
DIRS += $(dir $(wildcard $(addsuffix /Makefile, $(LOG_DIRS))))

$(info $(DIRS) )

include $(RIOTBASE)/Makefile.base
