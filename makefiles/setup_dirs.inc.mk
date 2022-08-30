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

# Only warn users, not the CI.
ifneq ($(RIOT_CI_BUILD),1)
  # Do not warn when set from sub-make
  ifeq ($(MAKELEVEL),0)
    ifneq (,$(BOARDSDIR))
      $(warning Using BOARDSDIR is deprecated use EXTERNAL_BOARD_DIRS instead)
      $(info EXTERNAL_BOARD_DIRS can contain multiple folders separated by space)
    endif
  endif
endif

# Needed for backward compatibility:
ifneq (,$(BOARDSDIR))
  EXTERNAL_BOARD_DIRS += $(BOARDSDIR)
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
    $(info Please note: At least one path in EXTERNAL_BOARD_DIRS is not existing)
  endif
  ifneq ($(EXTERNAL_MODULE_DIRS),$(foreach dir,$(EXTERNAL_MODULE_DIRS),$(wildcard $(dir))))
    $(info Please note: At least one path in EXTERNAL_MODULE_DIRS is not existing)
  endif
  ifneq ($(EXTERNAL_PKG_DIRS),$(foreach dir,$(EXTERNAL_PKG_DIRS),$(wildcard $(dir))))
    $(info Please note: At least one path in EXTERNAL_PKG_DIRS is not existing)
  endif
endif

# Ensure that all directories are set and don't contain spaces.
ifneq (, $(filter-out 1, $(foreach v,$(__DIRECTORY_VARIABLES),$(words $($(v))))))
  $(info Aborting compilation for your safety.)
  $(info Related variables = $(__DIRECTORY_VARIABLES))
  $(error Make sure no path override is empty or contains spaces!)
endif
