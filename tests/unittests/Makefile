export PROJECT = unittests
include ../Makefile.tests_common

USEMODULE += embunit

INCLUDES += -I$(RIOTBASE)/tests/unittests/embunit

ifeq ($(OUTPUT),XML)
	CFLAGS += -DOUTPUT=OUTPUT_XML
	USEMODULE += embunit_textui
else ifeq ($(OUTPUT),TEXT)
	CFLAGS += -DOUTPUT=OUTPUT_TEXT
	USEMODULE += embunit_textui
else ifeq ($(OUTPUT),COMPILER)
	CFLAGS += -DOUTPUT=OUTPUT_COMPILER
	USEMODULE += embunit_textui
endif

ifeq (, $(filter tests-%, $(MAKECMDGOALS)))
    UNIT_TESTS := $(shell find -mindepth 1 -maxdepth 1 -type d -name 'tests-*' -printf '%f ')
else
    UNIT_TESTS := $(filter tests-%, $(MAKECMDGOALS))
endif

include $(RIOTBASE)/Makefile.include

UNITTEST_LIBS := $(UNIT_TESTS:%=$(BINDIR)%.a)

all: $(UNITTEST_LIBS)
$(UNIT_TESTS): all

$(UNITTEST_LIBS): $(BINDIR)%.a:
	"$(MAKE)" -C $(CURDIR)/$*

charEMPTY :=
charSPACE := $(charEMPTY) $(charEMPTY)
charCOMMA := ,

ifeq (, $(UNIT_TESTS))
    CFLAGS += -DNO_TEST_SUITES
    $(warning There was no test suite specified!)
else
    CFLAGS += -DTEST_SUITES='$(subst $(charSPACE),$(charCOMMA),$(UNIT_TESTS:tests-%=%))'
endif

BASELIBS += $(UNITTEST_LIBS)
