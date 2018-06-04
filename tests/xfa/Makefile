include ../Makefile.tests_common

include $(RIOTBASE)/Makefile.include

all: static-test
static-test: $(ELFFILE)
	$(Q)TEST_STARTADDR=$$($(OBJDUMP) -t $< | grep -E '\sxfatest_const$$' | awk '{ printf "0x%s", $$1}'); \
	TEST_ENDADDR=$$($(OBJDUMP) -t $< | grep -E '\sxfatest_const_end$$' | awk '{ printf "0x%s", $$1}'); \
	if test ! $$((TEST_STARTADDR)) -lt $$((TEST_ENDADDR)); then \
	  echo "Error: Static check of XFA linked const array failed, verify linker flags and try again" >&2; \
	  exit 1; \
	fi
