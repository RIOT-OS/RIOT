# Functions to manipulate filesystem paths

include $(dir $(lastword $(MAKEFILE_LIST)))/strings.mk

# Join components to make a path name
joinpath = $(call intercal,/,$(1))

# Split a path into its components
splitpath = $(subst /, ,$(1))

_putdots = $(patsubst %,..,$(1))
_relpath ?= $(if $(call streq,$(firstword $(1)),$(firstword $(2))),\
$(call _relpath,$(call rest,$(1)),$(call rest,$(2))),\
$(call _putdots,$(1)) $(2))

# Given a directory and a path (both absolute), calculate the path of the second
# argument relative to the first one.
relpath = $(call joinpath,$(call _relpath,$(call splitpath,$(1)),$(call splitpath,$(2))))
