# The module name would otherwise default to `cpu`, which clashes with the
# module of the CPU itself.
MODULE = cpus

# SORT THIS ALPHABETICALLY BY COMMON CPU MODULE NAME!
ifneq (,$(filter cpu_common_rp2,$(USEMODULE)))
  DIRS += $(RIOTCPU)/common/rp2
endif
ifneq (,$(filter cpu_common_rp2350,$(USEMODULE)))
  DIRS += $(RIOTCPU)/common/rp2350
endif

include $(RIOTBASE)/Makefile.base
