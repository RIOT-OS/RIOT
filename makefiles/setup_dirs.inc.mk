# Setting EXTERNAL_BOARD_DIRS and EXTERNAL_MODULE_DIRS as command line argument
# is too messy to handle: Even when every path in EXTERNAL_BOARD_DIRS is turned
# into an absolute path using override, sub-makes will still get the original
# value. Using MAKEOVERRIDES has issues with spaces in the values, which are
# used as separator in EXTERNAL_BOARD_DIRS. So we just enforce setting the value
# either in a Makefile, or as environment variable.
ifeq ($(INSIDE_DOCKER),0)
  # In Docker absolute paths are always given, so only fail when not in docker
  ifeq ($(origin EXTERNAL_BOARD_DIRS),command line)
    $(error EXTERNAL_BOARD_DIRS must be passed as environment variable, and not as command line argument)
  endif
  ifeq ($(origin EXTERNAL_MODULE_DIRS),command line)
    $(error EXTERNAL_MODULE_DIRS must be passed as environment variable, and not as command line argument)
  endif
  ifeq ($(origin EXTERNAL_PKG_DIRS),command line)
    $(error EXTERNAL_PKG_DIRS must be passed as environment variable, and not as command line argument)
  endif
endif

# Deprecation of configuring 'RIOTBOARD'
ifneq ($(abspath $(RIOTBASE)/boards),$(abspath $(RIOTBOARD)))
  $(warning overriding RIOTBOARD for external boards is deprecated, use EXTERNAL_BOARD_DIRS instead)
  override RIOTBOARD      := $(abspath $(RIOTBOARD))
  __DIRECTORY_VARIABLES += RIOTBOARD
endif

# Make all paths absolute.
override RIOTBASE               := $(abspath $(RIOTBASE))
override RIOTCPU                := $(abspath $(RIOTCPU))
override RIOTMAKE               := $(abspath $(RIOTMAKE))
override RIOTPKG                := $(abspath $(RIOTPKG))
override RIOTTOOLS              := $(abspath $(RIOTTOOLS))
override RIOTPROJECT            := $(abspath $(RIOTPROJECT))
override APPDIR                 := $(abspath $(APPDIR))
override BUILD_DIR              := $(abspath $(BUILD_DIR))
override BINDIRBASE             := $(abspath $(BINDIRBASE))
override BINDIR                 := $(abspath $(BINDIR))
override PKGDIRBASE             := $(abspath $(PKGDIRBASE))
override DLCACHE_DIR            := $(abspath $(DLCACHE_DIR))
EXTERNAL_BOARD_DIRS             := $(foreach dir,$(EXTERNAL_BOARD_DIRS),$(abspath $(dir)))
EXTERNAL_MODULE_DIRS            := $(foreach dir,$(EXTERNAL_MODULE_DIRS),$(abspath $(dir)))
EXTERNAL_PKG_DIRS               := $(foreach dir,$(EXTERNAL_PKG_DIRS),$(abspath $(dir)))

ifneq ($(RIOT_CI_BUILD),1)
  ifneq ($(EXTERNAL_BOARD_DIRS),$(foreach dir,$(EXTERNAL_BOARD_DIRS),$(wildcard $(dir))))
    $(call echowarn,("Please note: At least one path in EXTERNAL_BOARD_DIRS does not exist."))
  endif
  ifneq ($(EXTERNAL_MODULE_DIRS),$(foreach dir,$(EXTERNAL_MODULE_DIRS),$(wildcard $(dir))))
    $(call echowarn,("Please note: At least one path in EXTERNAL_MODULE_DIRS does not exist"))
  endif
  ifneq ($(EXTERNAL_PKG_DIRS),$(foreach dir,$(EXTERNAL_PKG_DIRS),$(wildcard $(dir))))
    $(call echowarn,("Please note: At least one path in EXTERNAL_PKG_DIRS does not exist"))
  endif
endif

# Print error messages when these false friends have been set instead of
# `EXTERNAL_BOARD_DIRS`/`EXTERNAL_MODULE_DIRS`/`EXTERNAL_PKG_DIRS`.
ifneq (,$(EXTERNAL_BOARDS_DIRS))
  $(error You have set 'EXTERNAL_BOARDS_DIRS', did you mean 'EXTERNAL_BOARD_DIRS'?)
endif
ifneq (,$(EXTERNAL_BOARDS_DIR))
  $(error You have set 'EXTERNAL_BOARD_DIR', did you mean 'EXTERNAL_BOARD_DIRS'?)
endif
ifneq (,$(EXTERNAL_BOARD_DIR))
  $(error You have set 'EXTERNAL_BOARD_DIR', did you mean 'EXTERNAL_BOARD_DIRS'?)
endif

ifneq (,$(EXTERNAL_MODULES_DIRS))
  $(error You have set 'EXTERNAL_MODULES_DIRS', did you mean 'EXTERNAL_MODULE_DIRS'?)
endif
ifneq (,$(EXTERNAL_MODULES_DIR))
  $(error You hava set 'EXTERNAL_MODULES_DIR', did you mean 'EXTERNAL_MODULE_DIRS'?)
endif
ifneq (,$(EXTERNAL_MODULE_DIR))
  $(error You have set 'EXTERNAL_MODULE_DIR', did you mean 'EXTERNAL_MODULE_DIRS'?)
endif

ifneq (,$(EXTERNAL_PKGS_DIRS))
  $(error You have set 'EXTERNAL_PKGS_DIRS', did you mean 'EXTERNAL_PKG_DIRS'?)
endif
ifneq (,$(EXTERNAL_PKGS_DIR))
  $(error You have set 'EXTERNAL_PKG_DIR', did you mean 'EXTERNAL_PKG_DIRS'?)
endif
ifneq (,$(EXTERNAL_PKG_DIR))
  $(error You have set 'EXTERNAL_PKG_DIR', did you mean 'EXTERNAL_PKG_DIRS'?)
endif

# Ensure that all directories are set and don't contain spaces.
ifneq (, $(filter-out 1, $(foreach v,$(__DIRECTORY_VARIABLES),$(words $($(v))))))
  $(info Aborting compilation for your safety.)
  $(info Related variables = $(__DIRECTORY_VARIABLES))
  $(error Make sure no path override is empty or contains spaces!)
endif
