FLASHFILE ?= $(HEXFILE)

IMAGE_OFFSET ?= $(ROM_OFFSET)

FLASHER ?= $(RIOTTOOLS)/uf2/uf2conv.py
FFLAGS  ?= $(UF2CONV_FLAGS) $(FLASHFILE) --base $(IMAGE_OFFSET)

PREFLASH_DELAY ?= 2

# Enforce FLASHFILE formats that include address information.
# This ensures correct address offsets even if the flashfile was compile with a different
# UF2_SOFTDEV value (and thus ROM_OFFSET) than the one that is currently set.
flashfile-format-check:
	@if ! echo "$(FLASHFILE)" | grep -E '\.(hex|elf)$$'; then \
		echo "Please use a FLASHFILE format that includes address information (HEX or ELF)."; \
		exit 1; \
	fi


# Execute the SoftDevice check. It verifies that the SoftDevice is present if
# needed, that the versions match and that it is not accidentally overridden.
uf2-softdevice-check:
	@UF2_SOFTDEV=$(UF2_SOFTDEV) $(RIOTTOOLS)/uf2/nrf52_softdevice_check.sh
	@if [ $$? -ne 0 ]; then \
		exit 1; \
  fi

ifeq ($(RIOTTOOLS)/uf2/uf2conv.py,$(FLASHER))
  FLASHDEPS += $(FLASHER)
  FLASHDEPS += flashfile-format-check
  FLASHDEPS += uf2-softdevice-check
endif

# The check for the SoftDevice should not run parallel to the preflash-sleep,
# otherwise the device won't be in bootloader mode yet when trying to check
# the SoftDevice version.
.NOTPARALLEL: uf2-softdevice-check
