# GIT_VERSION is git description + branch name if it not master
# Empty when not in a git repository
#
#   2019.10-devel-97-g3bcc72
#   2019.10-devel-97-g3bcc72-pr/git_version

GIT_DESCRIBE = $(call memoized,GIT_DESCRIBE,$(shell git --git-dir="$(RIOTBASE)/.git" describe --always --abbrev=4 2> /dev/null))
GIT_BRANCH = $(call memoized,GIT_BRANCH,$(shell git --git-dir="$(RIOTBASE)/.git" rev-parse --abbrev-ref HEAD))
_GIT_VERSION = $(GIT_DESCRIBE)$(addprefix -,$(GIT_BRANCH:master=))

GIT_VERSION ?= $(if $(GIT_DESCRIBE),$(_GIT_VERSION))

# This variable is recursively expanded on purpose. It is only expanded when
# $(RIOT_VERSION) has not been set already. For CI builds, Makefile.include
# sets the version to 'buildtest' before this is used, so $(GIT_VERSION) is
# never expanded and two shell calls are saved. Outside of CI, $(GIT_VERSION)
# is memoized and will not be re-evaluated on every expansion.
RIOT_VERSION_GIT = $(or $(GIT_VERSION),'UNKNOWN (builddir: $(RIOTBASE))')
