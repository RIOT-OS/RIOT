.PHONY: info-applications info-applications-supported-boards

# fallback so empty RIOTBASE won't lead to "/examples/"
RIOTBASE ?= .

# Define the list of examples subdirectories that container application directories
EXAMPLES_APPLICATIONS_SUBDIRS :=  \
    ble                       \
    coap                      \
    cord                      \
    dtls                      \
    gnrc                      \
    icn                       \
    lora                      \
    mqtt                      \
    posix                     \
    #
EXAMPLES_APPLICATIONS_SUBDIRS := $(addprefix examples/,$(EXAMPLES_APPLICATIONS_SUBDIRS))

# Define the list of tests sudirectories that contain application directories
TEST_APPLICATIONS_SUBDIRS :=  \
    arch                      \
    bench                     \
    boards                    \
    bootloaders               \
    build_system              \
    core                      \
    cpp                       \
    drivers                   \
    periph                    \
    pkg                       \
    sys                       \
    sys/net                   \
    #
TEST_APPLICATIONS_SUBDIRS := $(addprefix tests/,$(TEST_APPLICATIONS_SUBDIRS))

# Prepare the list of application directories
APPLICATION_DIRS :=                   \
    fuzzing                           \
    bootloaders                       \
    examples                          \
    tests                             \
    $(EXAMPLES_APPLICATIONS_SUBDIRS)  \
    $(TEST_APPLICATIONS_SUBDIRS)      \
    #

APPLICATION_DIRS := $(addprefix $(RIOTBASE)/,$(APPLICATION_DIRS))
APPLICATION_DIRS := $(addsuffix /*/Makefile,$(APPLICATION_DIRS))

# 1. use wildcard to find Makefiles
# 2. use patsubst to drop trailing "/"
# 3. use patsubst to drop possible leading "./"
# 4. sort
APPLICATION_DIRS := $(sort $(patsubst ./%,%,$(patsubst %/,%,$(dir $(wildcard $(APPLICATION_DIRS))))))

info-applications:
	@for dir in $(APPLICATION_DIRS); do echo $$dir; done

# All applications / board output of `info-boards-supported`.
info-applications-supported-boards:
	@for dir in $(APPLICATION_DIRS); do \
	  $(MAKE) --no-print-directory -C $${dir} info-boards-supported 2>/dev/null | xargs -n 1 echo $${dir}; \
	done
# BOARDS values from 'boards.inc.mk' to only evaluate it once
info-applications-supported-boards: export BOARDS ?=
