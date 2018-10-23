# Utilities to produce errors inside Make
# Use this functions to produce friendlier error messages.

include $(dir $(lastword $(MAKEFILE_LIST)))/strings.mk

# Produce an error if the value is empty
#
# Parameters
#	value: a string which should not be empty
#	message: The error message to display.
# Returns:
#	the first argument, if it is not empty.
ensure_value = $(if $(1),$(1),$(error $(2)))

# Produce an error if the value is empty
# This is similar to ensure_value, but always returns an empty string.
#
# Parameters
#	value: a string which should not be empty
#	message: The error message to display.
# Returns:
#	empty string
assert = $(if $(1),,$(error $(2)))

# Produce an error if the value is NOT empty
# This is the opposite of assert
#
# Parameters
#	value: a string which should be empty
#	message: The error message to display.
# Returns:
#	empty string
assert_not = $(if $(1),$(error $(2)),)

# Produce an error if the values are not equal
#
# Parameters
#	a: first string to match
#	b: second string to match
#	message (optional): The error message to display. If the message is empty a
#					default message is shown.
# Returns:
#	empty string
assert_eq = $(if $(call streq,$(1),$(2)),,$(error $(if $(3),$(3),Expected "$(2)" and got "$(1)")))
