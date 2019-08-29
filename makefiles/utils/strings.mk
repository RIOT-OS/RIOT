# String handling functions

# Return an empty string if the arguments are different (works only for single
# words
streq = $(filter $(1),$(2))

# Get the remaining words in a list (i.e. everything except the first)
# The upper limit is set to an unrealistically high number. The "correct" thing
# to do would be to count the words, but impacts performance, and nobody will
# ever have 2**31 words.
rest = $(wordlist 2,2147483647,$(1))

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

# Translate all words/characters in the first set to characters in the second set
# Parameters:
#    $1: space separated list of words or characters
#    $2: space separated list of words or characters, same length as $1
#    $3: string to translate
xlate ?= $(if $(1),$(call xlate,$(call rest,$(1)),$(call rest,$(2)),$(subst $(firstword $(1)),$(firstword $(2)),$(3))),$(3))

_UPPER_LETTERS = A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
_LOWER_LETTERS = a b c d e f g h i j k l m n o p q r s t u v w x y z


lowercase = $(call xlate,$(_UPPER_LETTERS),$(_LOWER_LETTERS),$(1))
uppercase = $(call xlate,$(_LOWER_LETTERS),$(_UPPER_LETTERS),$(1))

uppercase_and_underscore = $(call uppercase,$(subst -,_,$1))
