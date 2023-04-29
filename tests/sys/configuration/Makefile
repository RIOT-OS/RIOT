include ../Makefile.sys_common

TEST_CONFIGURATION_BACKEND ?= configuration_backend_ram

TEST_CONFIGURATION_SUPPORTED_BACKENDS ?= \
  configuration_backend_ram \
  configuration_backend_flashdb_mtd \
  configuration_backend_flashdb_vfs

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

# configuration_backend_flashdb_vfs results in hard faults with little stack size
CFLAGS += -DTHREAD_STACKSIZE_MAIN=2*THREAD_STACKSIZE_LARGE

# for configuration_backend_flashdb_vfs you may need to add:
# CFLAGS+="-DDEBUG_ASSERT_VERBOSE=1
#          -DFAL_MTD=MTD_0
#          -DVFS_DEFAULT_DATA=VFS_DEFAULT_NVM\(0\)"
# TEST_CONFIGURATION_BACKEND=configuration_backend_flashdb_vfs
# USEMODULE="vfs_aut_format
#            configuration_backend_reset_flashdb"
# BOARD=same54-xpro make flash term

USEMODULE += embunit
USEMODULE += configuration
USEMODULE += $(TEST_CONFIGURATION_BACKEND)

include $(RIOTBASE)/Makefile.include
