PKG_NAME=nrfx
PKG_URL=https://github.com/NordicSemiconductor/nrfx
# Release 3.14.0
PKG_VERSION=11f57e578c7feea13f21c79ea0efab2630ac68c7
PKG_LICENSE=BSD-3-Clause

# This package serves two purposes. It can be added with `USEPKG += nrfx` just
# like normal, but it is also called by `cpu/nrf5x_common/Makefile.include` to
# fetch the nRF5x vendor headers.
# If the full `nrfx` package is used, this will always cause a full checkout,
# otherwise the vendor headers will be fetched with a sparse checkout,
# since the repository is multiple 100MBs big.
NRFX_HEADERS_nrf51  = mdk/nrf51.h
NRFX_HEADERS_nrf51 += mdk/nrf51_bitfields.h
NRFX_HEADERS_nrf51 += mdk/nrf51822_peripherals.h

NRFX_HEADERS_nrf52  = mdk/nrf52.h
NRFX_HEADERS_nrf52 += mdk/nrf52_bitfields.h

NRFX_HEADERS_nrf52 += mdk/nrf52810.h
NRFX_HEADERS_nrf52 += mdk/nrf52810_bitfields.h
NRFX_HEADERS_nrf52 += mdk/nrf52810_name_change.h
NRFX_HEADERS_nrf52 += mdk/nrf52810_peripherals.h

NRFX_HEADERS_nrf52 += mdk/nrf52811.h
NRFX_HEADERS_nrf52 += mdk/nrf52811_bitfields.h
NRFX_HEADERS_nrf52 += mdk/nrf52811_peripherals.h

NRFX_HEADERS_nrf52 += mdk/nrf52832_peripherals.h

NRFX_HEADERS_nrf52 += mdk/nrf52833.h
NRFX_HEADERS_nrf52 += mdk/nrf52833_bitfields.h
NRFX_HEADERS_nrf52 += mdk/nrf52833_peripherals.h

NRFX_HEADERS_nrf52 += mdk/nrf52840.h
NRFX_HEADERS_nrf52 += mdk/nrf52840_bitfields.h
NRFX_HEADERS_nrf52 += mdk/nrf52840_name_change.h
NRFX_HEADERS_nrf52 += mdk/nrf52840_peripherals.h

NRFX_HEADERS_nrf53  = mdk/nrf5340_application.h
NRFX_HEADERS_nrf53 += mdk/nrf5340_application_bitfields.h
NRFX_HEADERS_nrf53 += mdk/nrf5340_application_name_change.h
NRFX_HEADERS_nrf53 += mdk/nrf5340_application_peripherals.h
NRFX_HEADERS_nrf53 += mdk/nrf5340_network.h
NRFX_HEADERS_nrf53 += mdk/nrf5340_network_bitfields.h
NRFX_HEADERS_nrf53 += mdk/nrf5340_network_name_change.h
NRFX_HEADERS_nrf53 += mdk/nrf5340_network_peripherals.h

# The sparse checkout always contains the headers of all CPU families, so that
# switching between CPU families does not require fetching the package again.
NRFX_SPARSE_PATHS = $(NRFX_HEADERS_nrf51) $(NRFX_HEADERS_nrf52) $(NRFX_HEADERS_nrf53)

# A full checkout contains the vendor headers as well. If one is already
# present, keep it instead of replacing it with a sparse checkout.
# It is present when `nrfx.sparse` exists but is empty.
ifneq (,$(wildcard $(PKGDIRBASE)/nrfx.sparse))
  ifeq (,$(strip $(file <$(PKGDIRBASE)/nrfx.sparse)))
    NRFX_FULL_CHECKOUT_PRESENT = 1
  endif
endif

# The sparse paths have to be set before calling `pkg/pkg.mk`
ifeq (,$(filter nrfx_full,$(USEMODULE))$(NRFX_FULL_CHECKOUT_PRESENT))
  PKG_SPARSE_PATHS = $(NRFX_SPARSE_PATHS)
endif

include $(RIOTBASE)/pkg/pkg.mk

VENDOR_FOLDER = $(PKG_BUILD_DIR)/vendor
# Only the vendor headers of the selected CPU family are copied.
NRFX_VENDOR_HEADERS := $(addprefix $(VENDOR_FOLDER)/, $(notdir $(NRFX_HEADERS_$(CPU_FAM))))

# Always copy the vendor headers in the preparation step. When we
# use nrfx, we can safely assume that we are using an nRF microcontroller that needs
# vendor headers too.
# Note: This has to be done in the preparation step, not in the `all` target
#       because the `all` target builds the package during the compilation run,
#       but at that point we already have to have them in place.
$(PKG_PREPARED): $(VENDOR_FOLDER) $(NRFX_VENDOR_HEADERS)

# Make sure the folder REALLY exists before trying to copy to it.
$(VENDOR_FOLDER): | $(PKG_PATCHED)
	$(Q)mkdir -p $(VENDOR_FOLDER)

# The package has to be done with the patching before we try to copy the headers.
$(VENDOR_FOLDER)/%: $(PKG_PATCHED) | $(VENDOR_FOLDER)
	@cp $(PKG_SOURCE_DIR)/mdk/$* $@

all:
