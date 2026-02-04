include ../Makefile.sys_common

USEMODULE += event_thread

# Test that selecting event thread size works from the modules `Makefile.dep`
# work as expected by including a bunch of empty test module
EXTERNAL_MODULE_DIRS += external_modules
USEMODULE += test_module_a
USEMODULE += test_module_b
USEMODULE += test_module_c
USEMODULE += test_module_d

include $(RIOTBASE)/Makefile.include
