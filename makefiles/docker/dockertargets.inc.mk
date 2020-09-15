# TODO:
#        - First document only in advanced-build-system-tricks until mature
#        - We do not care for handling `clean` and other targets in parallel

# currently the BOARD is not always passed to docker, we always want to
DOCKER_ENVIRONMENT_CMDLINE += --env BOARD=$(BOARD)

################################################################################

# TODO: is it a good separator?
DOCKER_TARGET_SEPARATOR ?= __

# TODO:
#       - DOCKER_IMAGE should execute 32bit for native
#       - DOCKER_IMAGE needs native toolchain for flashing
#       - DOCKER_IMAGE general cleanup
#       - Should it be an extension of riot/riotbuild
#       - IPV6?
# HACK:
#       - currently riot/riotbuild is enough to test native samr21-xpro
# DOCKER_TARGET_IMAGE ?= riot/riotflash:latest
DOCKER_TARGET_IMAGE ?= $(DOCKER_IMAGE)

# TODO:
#       - fedora doesn't have this
#       - how to handle in windows & mac
DOCKER_TARGET_GROUPS ?= dialout plugdev

# Unless using --privileged every device we want the container to have acces
# must me mapped. PORT maps the serial device and PROG_DEV maps the serial
# device, these are usually not the same.
DOCKER_TARGET_PORT_VARS ?= PROG_DEV PORT
DOCKER_TARGET_ENVIRONMENT += $(foreach var,$(DOCKER_TARGET_PORT_VARS),--env $(var)=$(realpath $($(var))))

# TODO:
#       - Add SERIAL_TERM, SERIAL_PROG
#       - Migrate all SERIAL_* to SERIAL_TERM, SERIAL_PROG
DOCKER_TARGET_SERIAL_VARS ?= SERIAL DEBUG_ADAPTER_ID JLINK_SERIAL
DOCKER_TARGET_ENVIRONMENT += $(foreach var,$(DOCKER_TARGET_SERIAL_VARS),--env $(var)=$($(var)))

# Export device variables for the target BOARD to docker
DOCKER_TARGET_FLAGS += $(DOCKER_TARGET_ENVIRONMENT)

# Need interactive for term
DOCKER_TARGET_FLAGS += --interactive

# This will map all /dev to the container, should not be needed
# DOCKER_TARGET_VOLUMES ?= $(call docker_volume,/dev,/dev)
# DOCKER_TARGET_FLAGS += $(DOCKER_TARGET_VOLUMES)

# --privileged can be added to succeed in any case without groups, /dev, device configuration
# DOCKER_TARGET_FLAGS += --privileged
DOCKER_TARGET_FLAGS += $(addprefix --group-add ,$(DOCKER_TARGET_GROUPS))
DOCKER_TARGET_FLAGS += $(foreach var,$(DOCKER_TARGET_PORT_VARS),$(call map_existing_device,$($(var))))
map_existing_device = $(addprefix --device=,$(wildcard $1))

# Allow setting make args from command line like '-j'
DOCKER_TARGET_MAKE_ARGS ?=

.PHONY: docker/%

# Use a separator to execute multiple targets on the same container.
DOCKER_TARGET_TARGETS = $(subst $(DOCKER_TARGET_SEPARATOR), ,$*)

docker/%:
	$(call docker_run_make,$(DOCKER_TARGET_TARGETS),$(DOCKER_TARGET_IMAGE),$(DOCKER_TARGET_FLAGS),$(DOCKER_TARGET_MAKE_ARGS))
