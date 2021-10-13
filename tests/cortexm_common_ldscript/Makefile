BOARD ?= samr21-xpro
include ../Makefile.tests_common


# Normally all boards using `cortexm_common/ldscripts/cortexm.ld` linkerscript
# Only tested on these ones for the moment
BOARD_WHITELIST += iotlab-a8-m3
BOARD_WHITELIST += iotlab-m3
BOARD_WHITELIST += samr21-xpro
# Normally all boards using `kinetis/ldscripts/kinetis.ld` linkerscript
BOARD_WHITELIST += frdm-kw41z
BOARD_WHITELIST += frdm-k64f
BOARD_WHITELIST += frdm-k22f
BOARD_WHITELIST += mulle
BOARD_WHITELIST += pba-d-01-kw2x
BOARD_WHITELIST += teensy31
# Boards using a bootloader and ROM_OFFSET by default
BOARD_WHITELIST += arduino-mkr1000
BOARD_WHITELIST += arduino-mkrfox1200
BOARD_WHITELIST += arduino-mkrzero
BOARD_WHITELIST += blackpill
BOARD_WHITELIST += blackpill-128kib
BOARD_WHITELIST += bluepill
BOARD_WHITELIST += bluepill-128kib
BOARD_WHITELIST += feather-m0
BOARD_WHITELIST += opencm904
BOARD_WHITELIST += spark-core

include $(RIOTBASE)/Makefile.include


# # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Compile time tests for ROM_OFFSET and FW_ROM_LENGTH #
# # # # # # # # # # # # # # # # # # # # # # # # # # # #

COMPILE_TESTS  = test-elffile-overflow test-elffile-fw_rom_length
COMPILE_TESTS += tests-offsets tests-fw_rom_len tests-rom-overflow

# The tests should only be executed in build environment
ifneq ($(BUILD_IN_DOCKER),1)
all: compile-tests
endif

compile-tests: $(COMPILE_TESTS)
.PHONY: compile-tests $(COMPILE_TESTS)


# iotlab-m3 defines ROM_LEN as 512K which is not handled by bash math operations
ROM_LEN_BYTES = $(shell printf "0x%x\n" $$(($(ROM_LEN:%K=%*1024))))


# test-elffile-overflow depends on $(BINFILE) to prevent:
# * ROM_LEN to be passed to $(ELFFILE) generation
# * dummy error message of wc not finding the .bin file (ELFFILE is not enough)
test-elffile-overflow: $(BINFILE) _test-elffile-overflow-runtest

.PHONY: _test-elffile-overflow-runtest
_test-elffile-overflow-runtest: ROM_LEN=$(firstword $(shell wc -c $(BINFILE)))-1+$(if $(ROM_OFFSET),$(ROM_OFFSET),0)
_test-elffile-overflow-runtest: $(BINFILE)
	$(Q)echo -n "Test rom offset 1 byte overflow detection: "
	$(Q)\
	{ $(_LINK) -o /dev/null  2>&1 | grep -q "region \`rom' overflowed by 1 byte" ; } \
	  && echo [OK] || { echo [ERROR] Compilation should have failed >&2; exit 1; }


# Test `ROM_OFFSET` is removed from firmware rom length if the board defines it
test-elffile-fw_rom_length: $(ELFFILE)
	$(Q)echo -n "Test rom offset subtracted from rom length in elffile: "
	$(Q)\
	if test -n "$(ROM_OFFSET)"; then \
	  TEST_FW_LEN=$$($(PREFIX)readelf --symbols $^ 2>/dev/null | awk '/_fw_rom_length/{printf "0x%s\n", $$2}'); \
	  EXPECT_FW_LEN=$$(printf "0x%08x" $$(( $(ROM_LEN_BYTES) - $(ROM_OFFSET) ))); \
	  if test $${TEST_FW_LEN} != $${EXPECT_FW_LEN}; then \
	    echo "[ERROR] Rom offset not taken into account for firmware length $${TEST_FW_LEN} != $${EXPECT_FW_LEN}" >&2; \
	    exit 1;\
	  fi ;\
	  echo [OK] ; \
	else \
	  echo "[SKIP](Reason: board does not have a ROM_OFFSET configured)" ;\
	fi


# Test elffiles must not have $(ELFFILE) prerequisite as target specific
# variables are used for configuration and they also apply to prerequisites.
#
# https://www.gnu.org/software/make/manual/make.html#Target_002dspecific

ELFFILES_DEPS = $(BASELIBS) FORCE


# Compile elf files with different ROM_OFFSET
# and verify the offset is taken into account

OFFSETS_TESTS = 0x1000 0x2000
tests-offsets: $(OFFSETS_TESTS:%=test-offset_%)

.PHONY: test-offset_%
# Match the 'code' section. It is usually '.text' but is '.vector' on `kinetis`.
# * [ 1] .text             PROGBITS        08001000 001000 001ef8 ...
# * [ 1] .vector           PROGBITS        00001000 001000 000400 ...
# So match with the section [ 1] being `PROGBITS`. Adapt if new cases appear.
CODE_SECTION = \[ 1\] \.[a-zA-Z_.]* *PROGBITS
test-offset_%: $(BINDIR)/$(APPLICATION)_offset_%.elf
	$(Q)echo -n "Test compilation with offset $*: "
	$(Q)\
	TEST_START_ADDR=$$($(PREFIX)readelf --section-headers $^ 2>/dev/null | awk '/$(CODE_SECTION)/{printf "0x%s\n", $$5}'); \
	EXPECT_START_ADDR=$$(printf "0x%08x" $$(( $(ROM_START_ADDR) + $* ))); \
	if test $${TEST_START_ADDR} != $${EXPECT_START_ADDR}; then \
	  echo "[ERROR] Linker offset not used $${TEST_START_ADDR} != $${EXPECT_START_ADDR}" >&2; \
	  exit 1;\
	fi
	$(Q)echo [OK]

$(BINDIR)/$(APPLICATION)_offset_%.elf: ROM_OFFSET=$*
$(BINDIR)/$(APPLICATION)_offset_%.elf: $(ELFFILES_DEPS)
	$(Q)$(_LINK) -o $@
.PRECIOUS: $(BINDIR)/$(APPLICATION)_offset_%.elf


# Compile elf files with FW_ROM_LEN and verify the length is taken into account
#   I arbitrarily do 'half' size because I needed to take a value and
#   that it is similar to what is required for doing dual firmware in rom ota

tests-fw_rom_len: test-fw_len_half_rom

.PHONY: test-fw_len_half_rom
test-fw_len_half_rom: $(BINDIR)/$(APPLICATION)_fw_len_half_rom.elf
	$(Q)echo -n "Test compilation with half ROM length: "
	$(Q)\
	TEST_FW_LEN=$$($(PREFIX)readelf --symbols $^ 2>/dev/null | awk '/_fw_rom_length/{printf "0x%s\n", $$2}'); \
	EXPECT_FW_LEN=$$(printf "0x%08x" $$(( $(ROM_LEN_BYTES) / 2 ))); \
	if test $${TEST_FW_LEN} != $${EXPECT_FW_LEN}; then \
	  echo "[ERROR] Linker firmware length not used $${TEST_FW_LEN} != $${EXPECT_FW_LEN}" >&2; \
	  exit 1;\
	fi
	$(Q)echo [OK]
$(BINDIR)/$(APPLICATION)_fw_len_half_rom.elf: FW_ROM_LEN=$$(($(ROM_LEN_BYTES)/2))
$(BINDIR)/$(APPLICATION)_fw_len_half_rom.elf: $(ELFFILES_DEPS)
	$(Q)$(_LINK) -o $@
.PRECIOUS: $(BINDIR)/$(APPLICATION)_fw_len_half_rom.elf


# Test FW_ROM_LEN overflow detection

OVERFLOW_TESTS = too_big_for_rom offset_and_romlen
tests-rom-overflow: $(OVERFLOW_TESTS:%=test-assert_overflow_%)


# Simple FW_ROM_LEN overflow
test-assert_overflow_too_big_for_rom: FW_ROM_LEN=$$(($(ROM_LEN_BYTES) + 1))

# ROM_OFFSET and FW_ROM_LEN set ROM_LEN
test-assert_overflow_offset_and_romlen: ROM_OFFSET=0x1000
test-assert_overflow_offset_and_romlen: FW_ROM_LEN=$(ROM_LEN_BYTES)

.PHONY: test-assert_overflow_%
test-assert_overflow_%: $(ELFFILES_DEPS)
	$(Q) echo -n "Test ROM overflow detection ($*): "
	$(Q)\
	{ $(_LINK) -o /dev/null 2>&1 | grep -q 'Specified firmware size does not fit in ROM' ; } \
	  && echo [OK] || { echo [ERROR] Compilation should have failed >&2; exit 1; }
