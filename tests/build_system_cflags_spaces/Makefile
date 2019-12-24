include ../Makefile.tests_common
CFLAGS += -DSUPER_STRING='"I love sentences with spaces"'

include $(RIOTBASE)/Makefile.include

# Changing this value should trigger a rebuild even if defined after
# Makefile.include
CONFIGURATION_VALUE ?= 0
CFLAGS += -DDEFINED_AFTER_MAKEFILE_INCLUDE=$(CONFIGURATION_VALUE)
# Exported to be available in the automated test
test: export CONFIGURATION_VALUE ?=
