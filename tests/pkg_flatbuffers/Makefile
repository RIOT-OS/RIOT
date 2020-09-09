include ../Makefile.tests_common

USEPKG += flatbuffers

# Skip warning raised when building monster_generated.h
CXXEXFLAGS += -Wno-type-limits

BUILDDEPS += monster_generated.h

include $(RIOTBASE)/Makefile.include

monster_generated.h: $(FLATC)
	$(Q)$(FLATC) --cpp $(CURDIR)/monster.fbs
