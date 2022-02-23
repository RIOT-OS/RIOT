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
PERIPH_IGNORE_MODULES := \
  periph_init% \
  periph_common \
  periph_flexcomm \
  periph_gpio_mux \
  periph_i2c_hw \
  periph_i2c_sw \
  periph_rtc_ms \
  periph_mcg \
  periph_wdog \
  periph_flash \
  periph_rtc_rtt \
  periph_rtt_hw_rtc \
  periph_rtt_hw_sys \
  periph_clic \
  periph_coretimer \
  periph_plic \
  periph_spi_on_qspi
  #
PERIPH_MODULES := $(filter-out $(PERIPH_IGNORE_MODULES),\
                               $(filter periph_%,$(USEMODULE)))
# Use simple expansion to avoid USEMODULE referencing itself
PERIPH_INIT_MODULES := $(subst periph_,periph_init_,$(PERIPH_MODULES))
DEFAULT_MODULE += $(PERIPH_INIT_MODULES)

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

# include rtc_utils if periph_rtc is used
ifneq (,$(filter periph_rtc,$(USEMODULE)))
  USEMODULE += rtc_utils
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

# select arduino_pwm pseudomodule if the corresponding feature is used
USEMODULE += $(filter arduino_pwm, $(FEATURES_USED))
