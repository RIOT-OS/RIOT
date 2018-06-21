PKG_NAME = nordic_softdevice_ble
PKG_VERSION = a53641a
PKG_FILE_BASE = nRF5_SDK_15.0.0_$(PKG_VERSION)
PKG_FILE = $(PKG_FILE_BASE).zip
PKG_URL = https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v15.x.x/$(PKG_FILE)
PKG_MD5 = 252b8bc8bc30f70ea5f9f22d2f3bb43b
PKG_LICENSE = nordic-bsd
PKG_DIR=$(CURDIR)
PKG_BUILDDIR=$(PKGDIRBASE)/$(PKG_NAME)
PKG_SRCDIR=$(PKG_BUILDDIR)/src

ifeq (,$(NRF_SD_VERS))
$(error "Nordic Softdevice not supported for $(CPU) ($(CPU_FAM))")
endif

SOFTDEVICE := components/softdevice/$(NRF_SD_VERS)/hex/$(NRF_SD_VERS)_nrf52_6.0.0_softdevice.hex
MODULE_MAKEFILE := $(PKG_DIR)/Makefile.module

.PHONY: all prepare clean distclean

all: $(BINDIR)/softdevice.hex

prepare: $(PKG_SRCDIR)/.extracted

$(BINDIR)/softdevice.hex: $(PKG_SRCDIR)/.extracted
	cp $(PKG_SRCDIR)/$(SOFTDEVICE) $@

$(PKG_SRCDIR)/.extracted: $(PKG_BUILDDIR)/$(PKG_FILE)
	rm -rf $(@D)
	mkdir -p $(@D)
	cd $(@D) && $(UNZIP_HERE) $(PKG_BUILDDIR)/$(PKG_FILE)
	mv $(@D)/$(PKG_FILE_BASE)/* $(@D)
	rmdir $(@D)/$(PKG_FILE_BASE)

# setup RIOT module makefiles for nordic SDK components

# ble_conn_params.c uses nRF5 SDK timers, which we otherwise don't need
	echo "MODULE=nrf5_ble_common"	    > $(PKG_SRCDIR)/components/ble/common/Makefile
	echo 'SRC := $$(filter-out ble_conn_params.c,$$(wildcard *.c))' \
					   >> $(PKG_SRCDIR)/components/ble/common/Makefile
	cat $(MODULE_MAKEFILE)		   >> $(PKG_SRCDIR)/components/ble/common/Makefile

	echo "MODULE=nrf5_ble_gatt"	    > $(PKG_SRCDIR)/components/ble/nrf_ble_gatt/Makefile
	cat $(MODULE_MAKEFILE)		   >> $(PKG_SRCDIR)/components/ble/nrf_ble_gatt/Makefile

	echo "MODULE=nrf5_ble_ipsp"	    > $(PKG_SRCDIR)/components/ble/ble_services/ble_ipsp/Makefile
	cat $(MODULE_MAKEFILE)		   >> $(PKG_SRCDIR)/components/ble/ble_services/ble_ipsp/Makefile

	echo "MODULE=nrf5_libraries_strerror" \
	                                    > $(PKG_SRCDIR)/components/libraries/strerror/Makefile
	cat $(MODULE_MAKEFILE)		   >> $(PKG_SRCDIR)/components/libraries/strerror/Makefile

	echo "MODULE=nrf5_libraries_util"   > $(PKG_SRCDIR)/components/libraries/util/Makefile
	echo 'SRC := $$(filter-out app_error_handler_iar.c app_error_handler_keil.c,$$(wildcard *.c))' \
					   >> $(PKG_SRCDIR)/components/libraries/util/Makefile
	cat $(MODULE_MAKEFILE)		   >> $(PKG_SRCDIR)/components/libraries/util/Makefile

	echo "MODULE=nrf5_section_vars"	    > $(PKG_SRCDIR)/components/libraries/experimental_section_vars/Makefile
	cat $(MODULE_MAKEFILE)		   >> $(PKG_SRCDIR)/components/libraries/experimental_section_vars/Makefile

	echo "MODULE=nrf5_softdevice_common"> $(PKG_SRCDIR)/components/softdevice/common/Makefile
	echo 'SRC := $$(filter-out nrf_sdh_freertos.c,$$(wildcard *.c))' \
					   >> $(PKG_SRCDIR)/components/softdevice/common/Makefile
	cat $(MODULE_MAKEFILE)		   >> $(PKG_SRCDIR)/components/softdevice/common/Makefile

	for PATCH in $(PKG_DIR)/patches/*; do \
		patch -d $(PKG_SRCDIR) --verbose -p1 < $$PATCH; \
	done

	touch $@

$(PKG_BUILDDIR)/$(PKG_FILE):
	@mkdir -p $(@D)
	$(Q)$(DLCACHE) $(PKG_URL) $(PKG_MD5) $@

clean::
	rm -rf $(PKG_SRCDIR)/

distclean::
	rm -rf $(PKG_BUILDDIR)/
