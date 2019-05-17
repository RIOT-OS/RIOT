# Utilities to set variables and environment for targets
# These functions should help replacing immediate evaluation and global 'export'


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
