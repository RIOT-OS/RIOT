# GIT_VERSION is git description + branch name if it not master
# Empty when not in a git repository
#
#   2019.10-devel-97-g3bcc72
#   2019.10-devel-97-g3bcc72-pr/git_version

GIT_DESCRIBE = $(call memoized,GIT_DESCRIBE,$(shell git --git-dir="$(RIOTBASE)/.git" describe --always --abbrev=4 2> /dev/null))
GIT_BRANCH = $(call memoized,GIT_BRANCH,$(shell git --git-dir="$(RIOTBASE)/.git" rev-parse --abbrev-ref HEAD))
_GIT_VERSION = $(GIT_DESCRIBE)$(addprefix -,$(GIT_BRANCH:master=))

GIT_VERSION ?= $(if $(GIT_DESCRIBE),$(_GIT_VERSION))
