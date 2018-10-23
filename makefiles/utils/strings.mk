# String handling functions

# Return an empty string if the arguments are different (works only for single
# words
streq = $(filter $(1),$(2))

# Get the remaining words in a list (i.e. everything except the first)
rest = $(wordlist 2,$(words $(1)),$(1))

# Strings as truth values
# In make, empty strings function as a "false" value and non-empty ones
# as true.

# Return a string ("x") if the argument is empty and vice versa
strnot = $(if $(1),,x)

# Concatenate a list of strings
# Defined via "?=" because redefinition kill the recursive definition
concat ?= $(if $(1),$(firstword $(1))$(call concat,$(call rest,$(1))),)

# Intercalate: concatenate words in a list with a separator between each one.
intercal = $(firstword $(2))$(call concat,$(addprefix $(1),$(call rest,$(2))))
