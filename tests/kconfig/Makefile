include ../Makefile.tests_common

USEMODULE += external_module_1
USEMODULE += external_module_2
EXTERNAL_MODULE_DIRS += external_modules

ifeq (1, $(TEST_KCONFIG))
  KCONFIG_ADD_CONFIG += $(APPDIR)/app.config
endif

include $(RIOTBASE)/Makefile.include
