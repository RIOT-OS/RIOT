# Utilities to set variables and environment for targets
# The variables defined here cover edge cases in make string handling.
# These functions should help replacing immediate evaluation and global 'export'

comma := ,
blank :=
space := $(blank) $(blank)

# Evaluate a deferred variable only once on its first usage
# Uses after that will be as if it was an immediate evaluation
# This can replace using `:=` by default
#
# The goal is to use it for `shell` commands
#
# variable = $(call memoized,<variable>,<value>)
#
# Parameters
#  variable: name of the variable you set
#  value: value that should be set when evaluated
memoized = $2$(eval $1:=$2)


# Target specific export the variables for that target
#
# target-export-variables <target> <variables>
#
# Parameters
#   target: name of target
#   variables: the variables to export
#
# The variable will only be evaluated when executing the target as when
# doing export
target-export-variables = $(foreach var,$(2),$(call _target-export-variable,$1,$(var)))

# '$1: export $2' cannot be used alone
# By using '?=' the variable is evaluated at runtime only
_target-export-variable = $(eval $1: export $2?=)
