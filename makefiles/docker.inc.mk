export DOCKER_IMAGE ?= riot/riotbuild:latest
export DOCKER_BUILD_ROOT ?= /data/riotbuild
export DOCKER_FLAGS ?= --rm
# List of Docker-enabled make goals
export DOCKER_MAKECMDGOALS_POSSIBLE = \
  all \
  buildtest \
  scan-build \
  scan-build-analyze \
  tests-% \
  #
export DOCKER_MAKECMDGOALS = $(filter $(DOCKER_MAKECMDGOALS_POSSIBLE),$(MAKECMDGOALS))

# Docker creates the files .dockerinit and .dockerenv in the root directory of
# the container, we check for the files to determine if we are inside a container.
ifneq (,$(wildcard /.dockerinit /.dockerenv))
  export INSIDE_DOCKER := 1
else
  export INSIDE_DOCKER := 0
endif

# Default target for building inside a Docker container if nothing was given
export DOCKER_MAKECMDGOALS ?= all
# List of all exported environment variables that shall be passed on to the
# Docker container, they will only be passed if they are set from the
# environment, not if they are only default Makefile values.
export DOCKER_ENV_VARS = \
  APPDIR \
  AR \
  ARFLAGS \
  AS \
  ASFLAGS \
  BINDIR \
  BINDIRBASE \
  BOARD \
  BUILDRELPATH \
  BUILDTEST_MCU_GROUP \
  BUILDTEST_VERBOSE \
  CC \
  CFLAGS \
  CPPMIX \
  CXX \
  CXXEXFLAGS \
  CXXUWFLAGS \
  ELFFILE \
  HEXFILE \
  LINK \
  LINKFLAGPREFIX \
  LINKFLAGS \
  LTO \
  OBJCOPY \
  OFLAGS \
  PREFIX \
  QUIET \
  WERROR \
  RIOT_CI_BUILD \
  RIOT_VERSION \
  SCANBUILD_ARGS \
  SCANBUILD_OUTPUTDIR \
  SIZE \
  TOOLCHAIN \
  UNDEF \
  #

# Find which variables were set using the command line or the environment and
# pass those to Docker.
# DOCKER_ENVIRONMENT_CMDLINE must be immediately assigned (:=) or otherwise some
# of the environment variables will be overwritten by Makefile.include and their
# origin is changed to "file"
DOCKER_ENVIRONMENT_CMDLINE := $(foreach varname,$(DOCKER_ENV_VARS), \
  $(if $(filter environment command,$(origin $(varname))), \
  -e '$(varname)=$(subst ','\'',$($(varname)))', \
  ))
DOCKER_ENVIRONMENT_CMDLINE := $(strip $(DOCKER_ENVIRONMENT_CMDLINE))
# The variables set on the command line will also be passed on the command line
# in Docker
DOCKER_OVERRIDE_CMDLINE := $(foreach varname,$(DOCKER_ENV_VARS), \
    $(if $(filter command,$(origin $(varname))), \
    '$(varname)=$($(varname))', \
    ))
DOCKER_OVERRIDE_CMDLINE := $(strip $(DOCKER_OVERRIDE_CMDLINE))

# Overwrite if you want to use `docker` with sudo
DOCKER ?= docker

# Mounted volumes and exported environment variables

# Add GIT_CACHE_DIR if the directory exists
DOCKER_VOLUMES_AND_ENV += $(if $(wildcard $(GIT_CACHE_DIR)),-v $(GIT_CACHE_DIR):$(DOCKER_BUILD_ROOT)/gitcache)
DOCKER_VOLUMES_AND_ENV += $(if $(wildcard $(GIT_CACHE_DIR)),-e GIT_CACHE_DIR=$(DOCKER_BUILD_ROOT)/gitcache)

# Handle worktree by mounting the git common dir in the same location
_is_git_worktree = $(shell grep '^gitdir: ' $(RIOTBASE)/.git 2>/dev/null)
GIT_WORKTREE_COMMONDIR = $(abspath $(shell git rev-parse --git-common-dir))
DOCKER_VOLUMES_AND_ENV += $(if $(_is_git_worktree),-v $(GIT_WORKTREE_COMMONDIR):$(GIT_WORKTREE_COMMONDIR))

# Resolve symlink of /etc/localtime to its real path
# This is a workaround for docker on macOS, for more information see:
# https://github.com/docker/for-mac/issues/2396
ETC_LOCALTIME = $(realpath /etc/localtime)

# This will execute `make $(DOCKER_MAKECMDGOALS)` inside a Docker container.
# We do not push the regular $(MAKECMDGOALS) to the container's make command in
# order to only perform building inside the container and defer executing any
# extra commands such as flashing or debugging until after leaving the
# container.
# The `flash`, `term`, `debugserver` etc. targets usually require access to
# hardware which may not be reachable from inside the container.
..in-docker-container:
	@$(COLOR_ECHO) '$(COLOR_GREEN)Launching build container using image "$(DOCKER_IMAGE)".$(COLOR_RESET)'
	@# HACK: Handle directory creation here until it is provided globally
	$(Q)mkdir -p $(BUILD_DIR)
	$(DOCKER) run $(DOCKER_FLAGS) -t -u "$$(id -u)" \
	    -v '$(RIOTBASE):$(DOCKER_BUILD_ROOT)/riotbase' \
	    -v '$(BUILD_DIR):$(DOCKER_BUILD_ROOT)/build' \
	    -v '$(RIOTCPU):$(DOCKER_BUILD_ROOT)/riotcpu' \
	    -v '$(RIOTBOARD):$(DOCKER_BUILD_ROOT)/riotboard' \
	    -v '$(RIOTMAKE):$(DOCKER_BUILD_ROOT)/riotmake' \
	    -v '$(RIOTPROJECT):$(DOCKER_BUILD_ROOT)/riotproject' \
	    -v '$(ETC_LOCALTIME):/etc/localtime:ro' \
	    -e 'RIOTBASE=$(DOCKER_BUILD_ROOT)/riotbase' \
	    -e 'BUILD_DIR=$(DOCKER_BUILD_ROOT)/build' \
	    -e 'CCACHE_BASEDIR=$(DOCKER_BUILD_ROOT)/riotbase' \
	    -e 'RIOTCPU=$(DOCKER_BUILD_ROOT)/riotcpu' \
	    -e 'RIOTBOARD=$(DOCKER_BUILD_ROOT)/riotboard' \
	    -e 'RIOTMAKE=$(DOCKER_BUILD_ROOT)/riotmake' \
	    -e 'RIOTPROJECT=$(DOCKER_BUILD_ROOT)/riotproject' \
	    $(DOCKER_VOLUMES_AND_ENV) \
	    $(DOCKER_ENVIRONMENT_CMDLINE) \
	    -w '$(DOCKER_BUILD_ROOT)/riotproject/$(BUILDRELPATH)' \
	    '$(DOCKER_IMAGE)' make $(DOCKER_MAKECMDGOALS) $(DOCKER_OVERRIDE_CMDLINE)
