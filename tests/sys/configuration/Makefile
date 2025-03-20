include ../Makefile.sys_common

TEST_CONFIGURATION_BACKEND ?= configuration_backend_ram

TEST_CONFIGURATION_SUPPORTED_BACKENDS ?= \
  configuration_backend_ram \
  configuration_backend_flashdb_mtd

ifeq (,$(filter $(TEST_CONFIGURATION_BACKEND),$(TEST_CONFIGURATION_SUPPORTED_BACKENDS)))
  $(error "Supported backends are: $(TEST_CONFIGURATION_SUPPORTED_BACKENDS)")
endif

# set the backend to be used for the test
CFLAGS += -DTEST_CONFIGURATION_BACKEND=$(TEST_CONFIGURATION_BACKEND)
# define a preprocessor variable to indicate the used backend for the test
BACKEND = $(subst configuration_backend_,,$(TEST_CONFIGURATION_BACKEND))
CFLAGS += -DTEST_CONFIGURATION_BACKEND_$(call uppercase_and_underscore,$(BACKEND))

INCLUDES += -I$(APPDIR)/include

ifeq ($(TEST_CONFIGURATION_BACKEND),configuration_backend_ram)
  DIRS += configuration_backend_ram
  NO_PSEUDOMODULES += configuration_backend_ram
endif

USEMODULE += embunit
USEMODULE += configuration
USEMODULE += $(TEST_CONFIGURATION_BACKEND)

include $(RIOTBASE)/Makefile.include
