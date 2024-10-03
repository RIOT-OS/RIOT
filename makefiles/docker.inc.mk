# This *MUST* be updated in lock-step with the riotbuild image in
# https://github.com/RIOT-OS/riotdocker. The idea is that when checking out
# a random RIOT merge commit, `make BUILD_IN_DOCKER=1` should always succeed.
#
# When the docker image is updated, checks at
# dist/tools/buildsystem_sanity_check/check.sh start complaining in CI, and
# provide the latest values to verify and fill in.
DOCKER_TESTED_IMAGE_ID := 1329f419ec1a045a5830361f288536a56a0671a3b0db216e469369b00719cdff
DOCKER_TESTED_IMAGE_REPO_DIGEST := d5a70c06703731ddfebb98e9227eb03a69f02c393d9e89bbbcd65d71f3ef056e

DOCKER_PULL_IDENTIFIER := docker.io/riot/riotbuild@sha256:$(DOCKER_TESTED_IMAGE_REPO_DIGEST)
DOCKER_IMAGE_DEFAULT := sha256:$(DOCKER_TESTED_IMAGE_ID)
DOCKER_AUTO_PULL ?= 1
export DOCKER_IMAGE ?= $(DOCKER_IMAGE_DEFAULT)
export DOCKER_BUILD_ROOT ?= /data/riotbuild
DOCKER_RIOTBASE ?= $(DOCKER_BUILD_ROOT)/riotbase

# These targets need to be run before docker can be run
DEPS_FOR_RUNNING_DOCKER :=

# Overwrite if you want to use `docker` with sudo
DOCKER ?= docker

# List of Docker-enabled make goals
export DOCKER_MAKECMDGOALS_POSSIBLE = \
  all \
  buildtest-indocker \
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

ifeq (0:1,$(INSIDE_DOCKER):$(BUILD_IN_DOCKER))
  ifeq ($(DOCKER_IMAGE),$(DOCKER_IMAGE_DEFAULT))
    IMAGE_PRESENT:=$(shell $(DOCKER) image inspect $(DOCKER_IMAGE) 2>/dev/null >/dev/null && echo 1 || echo 0)
    ifeq (0,$(IMAGE_PRESENT))
      $(warning Required docker image $(DOCKER_IMAGE) not installed)
      ifeq (1,$(DOCKER_AUTO_PULL))
        $(info Pulling required image automatically. You can disable this with DOCKER_AUTO_PULL=0)
        DEPS_FOR_RUNNING_DOCKER += docker-pull
      else
        $(info Building with latest available riotbuild image. You can pull the correct image automatically with DOCKER_AUTO_PULL=1)
        # The currently set DOCKER_IMAGE is not locally available, and the
        # user opted out to automatically pull it. Fall back to the
        # latest (locally) available riot/riotbuild image instead.
        export DOCKER_IMAGE := docker.io/riot/riotbuild:latest
      endif
    endif
  endif
endif

# Default target for building inside a Docker container if nothing was given
export DOCKER_MAKECMDGOALS ?= all
# List of all exported environment variables that shall be passed on to the
# Docker container, they will only be passed if they are set from the
# environment, not if they are only default Makefile values.
DOCKER_RIOT_CONFIG_VARIABLES := $(filter RIOT_CONFIG_%,$(.VARIABLES))
export DOCKER_ENV_VARS += \
  APPDIR \
  AR \
  AS \
  ASFLAGS \
  BINDIR \
  BINDIRBASE \
  BOARD \
  BOARDS \
  BUILDRELPATH \
  BUILDTEST_MCU_GROUP \
  BUILDTEST_VERBOSE \
  CC \
  CC_NOCOLOR \
  CFLAGS \
  CONTINUE_ON_EXPECTED_ERRORS \
  CPPMIX \
  CXX \
  CXXEXFLAGS \
  CXXUWFLAGS \
  $(DOCKER_RIOT_CONFIG_VARIABLES) \
  ELFFILE \
  FLASHFILE \
  HEXFILE \
  IOTLAB_NODE \
  LINK \
  LINKFLAGPREFIX \
  LINKFLAGS \
  LTO \
  OBJCOPY \
  OFLAGS \
  PARTICLE_MONOFIRMWARE \
  PICOLIBC \
  PREFIX \
  PROGRAMMER \
  QUIET \
  RIOT_CI_BUILD \
  RIOT_VERSION \
  RIOT_VERSION_CODE \
  SCANBUILD_ARGS \
  SCANBUILD_OUTPUTDIR \
  SIZE \
  TOOLCHAIN \
  UNDEF \
  WERROR \
  #

# List of all exported environment variables that shall be passed on to the
# Docker container since they might have been set through the command line
# and environment.
# Their origin cannot be checked since they are often redefined or overriden
# in Makefile/Makefile.include, etc. and their origin is changed to file
export DOCKER_ENV_VARS_ALWAYS += \
  DISABLE_MODULE \
  DEFAULT_MODULE \
  FEATURES_REQUIRED \
  FEATURES_BLACKLIST \
  FEATURES_OPTIONAL \
  USEMODULE \
  USEPKG \
 #

# Find which variables were set using the command line or the environment and
# pass those to Docker.
# DOCKER_ENVIRONMENT_CMDLINE must be immediately assigned (:=) or otherwise some
# of the environment variables will be overwritten by Makefile.include and their
# origin is changed to "file"
DOCKER_ENVIRONMENT_CMDLINE_AUTO := $(foreach varname,$(DOCKER_ENV_VARS), \
  $(if $(filter environment command,$(origin $(varname))), \
  -e '$(varname)=$(subst ','\'',$($(varname)))', \
  ))
DOCKER_ENVIRONMENT_CMDLINE += $(strip $(DOCKER_ENVIRONMENT_CMDLINE_AUTO))

# Pass variables potentially set through command line or environment
# DOCKER_ENVIRONMENT_CMDLINE_ALWAYS is immediately assigned since this only wants
# to capture variables set via the environment or command line. This will still
# include variables set with '=' or '+=' in the application Makefile since these
# are included before evaluating DOCKER_ENVIRONMENT_CMDLINE_ALWAYS
DOCKER_ENVIRONMENT_CMDLINE_ALWAYS := $(foreach varname,$(DOCKER_ENV_VARS_ALWAYS), \
  -e '$(varname)=$(subst ','\'',$(sort $($(varname))))')
DOCKER_ENVIRONMENT_CMDLINE += $(strip $(DOCKER_ENVIRONMENT_CMDLINE_ALWAYS))

# The variables set on the command line will also be passed on the command line
# in Docker
DOCKER_OVERRIDE_CMDLINE_AUTO := $(foreach varname,$(DOCKER_ENV_VARS), \
    $(if $(filter command,$(origin $(varname))), \
    '$(varname)=$($(varname))', \
    ))
DOCKER_OVERRIDE_CMDLINE += $(strip $(DOCKER_OVERRIDE_CMDLINE_AUTO))

_docker_is_podman = $(shell $(DOCKER) --version | grep podman 2>/dev/null)

# Set default run flags:
# - allocate a pseudo-tty
# - remove container on exit
# - set username/UID to executor
DOCKER_USER ?= $$(id -u)
DOCKER_USER_OPT = $(if $(_docker_is_podman),--userns keep-id,--user $(DOCKER_USER))
DOCKER_RUN_FLAGS ?= --rm --tty $(DOCKER_USER_OPT)

# allow setting make args from command line like '-j'
DOCKER_MAKE_ARGS ?=

# Resolve symlink of /etc/localtime to its real path
# This is a workaround for docker on macOS, for more information see:
# https://github.com/docker/for-mac/issues/2396
ETC_LOCALTIME = $(realpath /etc/localtime)

# Fetch the number of jobs from the MAKEFLAGS
# With $MAKE_VERSION < 4.2, the amount of parallelism is not available in
# $MAKEFLAGS, only that parallelism is requested. So only -j, even if
# something like -j3 is specified. This can be unexpected and dangerous
# in older make so don't enable parallelism if $MAKE_VERSION < 4.2
MAKE_JOBS_NEEDS = 4.2.0
MAKE_VERSION_OK = $(call memoized,MAKE_VERSION_OK,$(call \
    version_is_greater_or_equal,$(MAKE_VERSION),$(MAKE_JOBS_NEEDS)))
DOCKER_MAKE_JOBS = $(if $(MAKE_VERSION_OK),$(filter -j%,$(MAKEFLAGS)),)
DOCKER_MAKE_ARGS += $(DOCKER_MAKE_JOBS)

# # # # # # # # # # # # # # # #
# Directory mapping functions #
# # # # # # # # # # # # # # # #
#
# This part handles mapping and mounting directories variables from the
# host system in the docker container.
#
# In the container all directories are mapped in subdirectories of
# `DOCKER_BUILD_ROOT` (`/data/riotbuild` by default).
#
#
# The RIOT directory `RIOTBASE` is mounted to `DOCKER_RIOTBASE`
# (`DOCKER_BUILD_ROOT/riotbase` by default).
#
# For other directories variables:
#
# * if the directory is contained within the `RIOT` repository,
#   the variable is mapped to a path inside `DOCKER_RIOTBASE` in the container.
#
# * if the directory is not contained in the `RIOT` repository,
#   the directory must be mounted in the countainer.
#   The variable and directory are mapped to a path outside `DOCKER_RIOTBASE`.
#   Some variables have hardwritten mapping directories (`RIOTCPU` for example),
#   and other have a mapping directory based on their directory name.


# Test if a directory is a subdirectory of `RIOTBASE`
#
#   dir_is_outside_riotbase <directory>
#
# $1 = directory
# Returns: a non empty value if it is True
#
# From env:
#  * RIOTBASE
#
# The terminating '/' in patsubst is important to match $1 == $(RIOTBASE)
# It also handles relative directories

define dir_is_outside_riotbase
$(filter $(abspath $1)/,$(patsubst $(RIOTBASE)/%,%,$(abspath $1)/))
endef

# Mapping of directores inside docker
#
# Return the path of directories from the host within the container
#
#   path_in_docker <directories> <map base directory|> <mapname|>
#
# $1 = directories (can be a list of relative directories)
# $2 = docker remap base directory (defaults to DOCKER_BUILD_ROOT)
# $3 = mapname (defaults to each directory name).
#      If provided $1 must only contain one directory.
# Returns: the path the directory would have in docker
#
# For each directory:
#  * if inside $(RIOTBASE), returns $(DOCKER_RIOTBASE)/<relative_path_in_riotbase>
#  * if outside $(RIOTBASE), returns <docker remapbase>/<mapname>
#
# From env:
#  * RIOTBASE
#  * DOCKER_RIOTBASE
#  * DOCKER_BUILD_ROOT

path_in_docker = $(foreach d,$1,$(strip $(call _dir_path_in_docker,$d,$2,$3)))
define _dir_path_in_docker
      $(if $(call dir_is_outside_riotbase,$1),\
        $(if $2,$2,$(DOCKER_BUILD_ROOT))/$(if $3,$3,$(notdir $(abspath $1))),\
        $(patsubst %/,%,$(patsubst $(RIOTBASE)/%,$(DOCKER_RIOTBASE)/%,$(abspath $1)/)))
endef

# Volume mapping and environment arguments
#
# Docker arguments for mapping directories:
#
# * volume mapping for each directory not in RIOT
# * remap environment variable directories to the docker ones
#
#
#   docker_volume_and_env  <path_in_docker_args|...>
#     docker_volumes_mapping and docker_environ_mapping on different lines
#
#   docker_volumes_mapping <path_in_docker_args|...>
#     Command line argument for mapping volumes, if it should be mounted
#       -v 'directory:docker_directory'
#
#   docker_environ_mapping <path_in_docker_args|...>
#     Command line argument for mapping environment variables
#       -e variable=docker_directory
#
#   docker_cmdline_mapping <path_in_docker_args|...>
#     Command line argument for mapping environment variables
#       variable=docker_directory
#
# Arguments are the same as 'path_in_docker'
# If the 'directories' variable is empty, it will not be exported to docker

# docker_volume command line arguments. Allows giving volume mount options.
# By default 'DOCKER_VOLUME_OPTIONS'. Argument option ignore the default.
DOCKER_VOLUME_OPTIONS ?= delegated
docker_volume = -v '$1:$2$(addprefix :,$(or $3,$(DOCKER_VOLUME_OPTIONS)))'

docker_volume_and_env = $(strip $(call _docker_volume_and_env,$1,$2,$3))
define _docker_volume_and_env
  $(call docker_volumes_mapping,$($1),$2,$3)
  $(call docker_environ_mapping,$1,$2,$3)
endef
docker_volumes_mapping = $(foreach d,$1,$(call _docker_volume_mapping,$d,$2,$3))
_docker_volume_mapping = $(if $1,$(if $(call dir_is_outside_riotbase,$1),$(call docker_volume,$(abspath $1),$(call path_in_docker,$1,$2,$3))))
docker_environ_mapping = $(addprefix -e ,$(call docker_cmdline_mapping,$1,$2,$3))
docker_cmdline_mapping = $(if $($1),'$1=$(call path_in_docker,$($1),$2,$3)')


# Application directory relative to either riotbase or riotproject
DOCKER_RIOTPROJECT = $(call path_in_docker,$(RIOTPROJECT),,riotproject)
DOCKER_APPDIR = $(DOCKER_RIOTPROJECT)/$(BUILDRELPATH)


# Directory mapping in docker and directories environment variable configuration
DOCKER_VOLUMES_AND_ENV += $(call docker_volume,$(ETC_LOCALTIME),/etc/localtime,ro)
DOCKER_VOLUMES_AND_ENV += $(call docker_volume,$(RIOTBASE),$(DOCKER_RIOTBASE))
# Selective components of Cargo to ensure crates are not re-downloaded (and
# subsequently rebuilt) on every run. Not using all of ~/.cargo as ~/.cargo/bin
# would be run by Cargo and that'd be very weird.
DOCKER_VOLUMES_AND_ENV += $(call docker_volume,$(HOME)/.cargo/registry,$(DOCKER_BUILD_ROOT)/.cargo/registry)
DOCKER_VOLUMES_AND_ENV += $(call docker_volume,$(HOME)/.cargo/git,$(DOCKER_BUILD_ROOT)/.cargo/git)
DOCKER_VOLUMES_AND_ENV += -e 'RIOTBASE=$(DOCKER_RIOTBASE)'
DOCKER_VOLUMES_AND_ENV += -e 'CCACHE_BASEDIR=$(DOCKER_RIOTBASE)'

DOCKER_VOLUMES_AND_ENV += $(call docker_volume_and_env,BUILD_DIR,,build)

# Prevent recursive invocation of docker by explicitely disabling docker via env variable,
# overwriting potential default in application Makefile
DOCKER_VOLUMES_AND_ENV += $(call docker_volume_and_env,BUILD_IN_DOCKER,,0)

DOCKER_VOLUMES_AND_ENV += $(call docker_volume_and_env,RIOTPROJECT,,riotproject)
DOCKER_VOLUMES_AND_ENV += $(call docker_volume_and_env,RIOTCPU,,riotcpu)
DOCKER_VOLUMES_AND_ENV += $(call docker_volume_and_env,RIOTBOARD,,riotboard)
DOCKER_VOLUMES_AND_ENV += $(call docker_volume_and_env,RIOTMAKE,,riotmake)

# Add GIT_CACHE_DIR if the directory exists
DOCKER_VOLUMES_AND_ENV += $(if $(wildcard $(GIT_CACHE_DIR)),$(call docker_volume,$(GIT_CACHE_DIR),$(DOCKER_BUILD_ROOT)/gitcache))
DOCKER_VOLUMES_AND_ENV += $(if $(wildcard $(GIT_CACHE_DIR)),-e 'GIT_CACHE_DIR=$(DOCKER_BUILD_ROOT)/gitcache')

# Remap external module directories.
#
# This remaps directories from EXTERNAL_MODULE_DIRS to subdirectories of
# $(DOCKER_BUILD_ROOT)/external
#
# Remapped directories must all have different basenames
#
# Limitation: If a directory is inside RIOTPROJECT and not in RIOT it is
# remapped anyway instead of loading from inside RIOTPROJECT.
#
# As EXTERNAL_MODULE_DIRS should ignore the 'Makefile' configuration, they must
# be set using command line variable settings to not be modified within docker.
DOCKER_VOLUMES_AND_ENV += $(call docker_volumes_mapping,$(EXTERNAL_MODULE_DIRS),$(DOCKER_BUILD_ROOT)/external,)
DOCKER_OVERRIDE_CMDLINE += $(call docker_cmdline_mapping,EXTERNAL_MODULE_DIRS,$(DOCKER_BUILD_ROOT)/external,)

# Remap 'EXTERNAL_BOARD_DIRS' if they are external
DOCKER_VOLUMES_AND_ENV += $(call docker_volumes_mapping,$(EXTERNAL_BOARD_DIRS),$(DOCKER_BUILD_ROOT)/external,)
# Value is overridden from command line if it is not the default value
# This allows handling even if the value is set in the 'Makefile'.
DOCKER_OVERRIDE_CMDLINE += $(call docker_cmdline_mapping,EXTERNAL_BOARD_DIRS,$(DOCKER_BUILD_ROOT)/external,)

# External module directories sanity check:
#
# Detect if there are remapped directories with the same name as it is not handled.
# Having EXTERNAL_MODULE_DIRS = /path/to/dir/name \
#                               /another/directory/also/called/name
# would lead to both being mapped to '$(DOCKER_BUILD_ROOT)/external/name'
_mounted_dirs = $(foreach d,$(EXTERNAL_MODULE_DIRS),$(if $(call dir_is_outside_riotbase,$(d)),$(d)))
ifneq ($(words $(sort $(notdir $(_mounted_dirs)))),$(words $(sort $(_mounted_dirs))))
  $(warning Mounted EXTERNAL_MODULE_DIRS: $(_mounted_dirs))
  $(error Mapping EXTERNAL_MODULE_DIRS in docker is not supported for directories with the same name)
endif

# Handle worktree by mounting the git common dir in the same location
_is_git_worktree = $(shell grep '^gitdir: ' $(RIOTBASE)/.git 2>/dev/null)
GIT_WORKTREE_COMMONDIR = $(abspath $(shell git rev-parse --git-common-dir))
DOCKER_VOLUMES_AND_ENV += $(if $(_is_git_worktree),$(call docker_volume,$(GIT_WORKTREE_COMMONDIR),$(GIT_WORKTREE_COMMONDIR)))

# Run a make command in a docker container
#   $1: make targets to run in docker
#   $2: docker image to use
#   $3: additional docker flags for specific targets
#   $4: additional make args like '-j'
docker_run_make = \
	$(DOCKER) run $(DOCKER_RUN_FLAGS) \
	$(DOCKER_VOLUMES_AND_ENV) \
	$(DOCKER_ENVIRONMENT_CMDLINE) \
	$3 \
	-w '$(DOCKER_APPDIR)' '$2' \
	$(MAKE) $(DOCKER_OVERRIDE_CMDLINE) $4 $1

# This target pulls the docker image required for BUILD_IN_DOCKER
.PHONY: docker-pull
docker-pull:
	$(DOCKER) pull '$(DOCKER_PULL_IDENTIFIER)'

# This will execute `make $(DOCKER_MAKECMDGOALS)` inside a Docker container.
# We do not push the regular $(MAKECMDGOALS) to the container's make command in
# order to only perform building inside the container and defer executing any
# extra commands such as flashing or debugging until after leaving the
# container.
# The `flash`, `term`, `debugserver` etc. targets usually require access to
# hardware which may not be reachable from inside the container.
..in-docker-container: $(DEPS_FOR_RUNNING_DOCKER)
	@$(COLOR_ECHO) '$(COLOR_GREEN)Launching build container using image "$(DOCKER_IMAGE)".$(COLOR_RESET)'
	$(call docker_run_make,$(DOCKER_MAKECMDGOALS),$(DOCKER_IMAGE),,$(DOCKER_MAKE_ARGS))
