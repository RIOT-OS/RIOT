# Add modules implementing used features
#
# This is done after the regular dependency resolution in Makefile.dep, as
# feature resolution depends on the used modules. As these modules however have
# no dependencies(except for periph_common), no dependency resolution is needed.

PERIPH_FEATURES := $(filter periph_%,$(FEATURES_USED))
# all periph features correspond to a periph submodule
# FEATURES_USED is defined in Makefile.features
USEMODULE += $(PERIPH_FEATURES)

# Add all USED periph_% init modules unless they are blacklisted
ifneq (,$(filter periph_init, $(USEMODULE)))
  PERIPH_MODULES := $(filter-out periph_init% periph_common,\
                                 $(filter periph_%,$(USEMODULE)))
  # Use simple expansion to avoid USEMODULE referencing itself
  PERIPH_INIT_MODULES := $(subst periph_,periph_init_,$(PERIPH_MODULES))
  DEFAULT_MODULE += $(PERIPH_INIT_MODULES)
endif

# select cpu_check_address pseudomodule if the corresponding feature is used
USEMODULE += $(filter cpu_check_address, $(FEATURES_USED))

# select bootloader_stm32 module if the feature is used
USEMODULE += $(filter bootloader_stm32, $(FEATURES_USED))

# include puf_sram if used
USEMODULE += $(filter puf_sram, $(FEATURES_USED))

# include periph_common if any periph_* driver is used
ifneq (,$(filter periph_%, $(USEMODULE)))
  USEMODULE += periph_common
endif

# select cortexm_svc pseudomodule if the corresponding feature is used
USEMODULE += $(filter cortexm_svc, $(FEATURES_USED))

# select core_idle_thread if the feature no_idle_thread is *not* used
ifeq (, $(filter no_idle_thread, $(FEATURES_USED)))
  USEMODULE += core_idle_thread
endif

# use mpu_stack_guard if the feature is used
ifneq (,$(filter cortexm_mpu,$(FEATURES_USED)))
  USEMODULE += mpu_stack_guard
endif

# use picolibc if the feature is used
ifneq (,$(filter picolibc,$(FEATURES_USED)))
  USEMODULE += picolibc
endif

# use newlib if the feature is used
ifneq (,$(filter newlib,$(FEATURES_USED)))
  USEMODULE += newlib
endif

# use efm32_coretemp if the feature is used
USEMODULE += $(filter efm32_coretemp, $(FEATURES_USED))

# if LC filter(s) is attached to the CPUs voltage regulator, use it
USEMODULE += $(filter vdd_lc_filter_%,$(FEATURES_USED))
