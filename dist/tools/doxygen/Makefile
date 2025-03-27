PKG_NAME=doxygen
PKG_LICENSE=GPL-2.0

# the PKG_VERSION is defined by the doc/doxygen/Makefile
ifeq (,$(filter clean distclean,$(MAKECMDGOALS)))
  ifeq (,$(PKG_VERSION))
    $(error Doxygen version undefined!)
  endif

  # If the requested and installed version mismatch, force a rebuild
  # by deleting the binary.
  ifneq (,$(shell test -f $(CURDIR)/doxygen && echo exists))
    ifneq ($(PKG_VERSION),$(shell $(CURDIR)/doxygen --version | cut -d ' ' -f1))
      $(shell rm $(CURDIR)/doxygen)
    endif
  endif
endif

# Try to download the binary if the platform is 'Linux x86_64' to avoid
# unnecessarily building Doxygen for CI.
ifeq (Linux x86_64,$(shell uname -m -s))
  # always build from source for the 'latest' version
  ifeq (latest,$(PKG_VERSION))
    BUILD_METHOD=build_source
  else
    # The URL has the following format (for version 1.13.2):
    # https://github.com/doxygen/doxygen/releases/download/Release_1_13_2/
    # doxygen-1.13.2.linux.bin.tar.gz
    BIN_PKG_URL=https://github.com/doxygen/doxygen/releases/download/Release_
    BIN_PKG_EXT=linux.bin.tar.gz
    BIN_PKG_TAR=$(PKG_NAME)-$(PKG_VERSION).$(BIN_PKG_EXT)
    BIN_PKG_DOWNLOAD_URL=$(BIN_PKG_URL)$(subst .,_,$(PKG_VERSION))/$(BIN_PKG_TAR)

    BIN_AVAILABLE := $(shell curl --head --silent --fail $(BIN_PKG_DOWNLOAD_URL) \
                      >/dev/null 2>&1 && echo yes || echo no)
    ifeq (yes,$(BIN_AVAILABLE))
      BUILD_METHOD=download_binary
    else
      BUILD_METHOD=build_source
    endif
  endif
endif

# fallback to building from source by default
BUILD_METHOD?=build_source

ifeq (build_source,$(BUILD_METHOD))
  PKG_URL=https://github.com/doxygen/doxygen
  PKG_EXT=
else
  PKG_URL=$(BIN_PKG_URL)
  PKG_EXT=$(BIN_PKG_EXT)
endif


.PHONY: clean distclean

all: $(CURDIR)/doxygen

$(CURDIR)/doxygen:
	@$(MAKE) $(BUILD_METHOD)

download_binary:
	@$(MAKE) clean
	@echo "[INFO] Downloading Doxygen binary..."
	@wget -q -O $(CURDIR)/$(BIN_PKG_TAR) $(BIN_PKG_DOWNLOAD_URL) || \
	  { echo "[ERROR] Failed to download binary"; exit 1; }
	@echo "[INFO] Unpacking Doxygen..."
	@tar -xf $(CURDIR)/$(BIN_PKG_TAR)
	@cp $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)/bin/doxygen $(CURDIR)

build_source:
	@$(MAKE) clean
	@echo "[INFO] Cloning and building Doxygen from source..."
	@if [ "$(PKG_VERSION)" = "latest" ]; then \
	    git clone --depth=1 $(PKG_URL).git $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION); \
	else \
	    git ls-remote --tags $(PKG_URL).git | grep -q "refs/tags/Release_$(subst .,_,$(PKG_VERSION))$$" || \
	    { echo "[ERROR] Specified version tag 'Release_$(subst .,_,$(PKG_VERSION))'" \
        "does not exist!"; exit 1; }; \
	    git clone --branch Release_$(subst .,_,$(PKG_VERSION)) --depth=1 \
	      $(PKG_URL).git $(CURDIR)/$(PKG_NAME); \
	fi
	cd $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION) && mkdir build && cd build && \
  cmake -G "Unix Makefiles" .. && make && cp bin/doxygen $(CURDIR)

clean distclean:
	rm -rf $(CURDIR)/$(PKG_NAME)*
