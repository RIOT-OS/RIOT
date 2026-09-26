# Make only version of string functions
#
# This replaces the pattern of using ':= $(shell echo $(var) | tr 'a-z-' 'A-Z_)'
# On local tests the make version was ~100 times faster than the shell one

# Converts $1 to lowercase
#   $1: string to convert
#   Return: $1 converted to lowercase
lowercase = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))

# Converts $1 to uppercase
#   $1: string to convert
#   Return: $1 converted to uppercase
uppercase = $(subst a,A,$(subst b,B,$(subst c,C,$(subst d,D,$(subst e,E,$(subst f,F,$(subst g,G,$(subst h,H,$(subst i,I,$(subst j,J,$(subst k,K,$(subst l,L,$(subst m,M,$(subst n,N,$(subst o,O,$(subst p,P,$(subst q,Q,$(subst r,R,$(subst s,S,$(subst t,T,$(subst u,U,$(subst v,V,$(subst w,W,$(subst x,X,$(subst y,Y,$(subst z,Z,$1))))))))))))))))))))))))))

# Converts $1 to uppercase and replaces '-' with '_'
#   $1: string to convert
#   Return: $1 converted to uppercase, with '-' replaced by '_'
uppercase_and_underscore = $(call uppercase,$(subst -,_,$1))

# Splits the string $1 into a space separated list of its characters, e.g.
# 'samd21' becomes 's a m d 2 1'. Only alphanumeric characters, '_' and '-' are
# recognised, which covers the CPU model and family names this is used for.
#   $1: string to split
#   Return: $1 as a space separated list of its characters
split_chars = $(strip $(subst 0, 0,$(subst 1, 1,$(subst 2, 2,$(subst 3, 3,$(subst 4, 4,$(subst 5, 5,$(subst 6, 6,$(subst 7, 7,$(subst 8, 8,$(subst 9, 9,$(subst a, a,$(subst b, b,$(subst c, c,$(subst d, d,$(subst e, e,$(subst f, f,$(subst g, g,$(subst h, h,$(subst i, i,$(subst j, j,$(subst k, k,$(subst l, l,$(subst m, m,$(subst n, n,$(subst o, o,$(subst p, p,$(subst q, q,$(subst r, r,$(subst s, s,$(subst t, t,$(subst u, u,$(subst v, v,$(subst w, w,$(subst x, x,$(subst y, y,$(subst z, z,$(subst A, A,$(subst B, B,$(subst C, C,$(subst D, D,$(subst E, E,$(subst F, F,$(subst G, G,$(subst H, H,$(subst I, I,$(subst J, J,$(subst K, K,$(subst L, L,$(subst M, M,$(subst N, N,$(subst O, O,$(subst P, P,$(subst Q, Q,$(subst R, R,$(subst S, S,$(subst T, T,$(subst U, U,$(subst V, V,$(subst W, W,$(subst X, X,$(subst Y, Y,$(subst Z, Z,$(subst _, _,$(subst -, -,$1)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))

# Returns the characters of $2 from position $1 onwards, like 'cut -c $1-'.
#   $1: position to start from
#   $2: string to extract characters from
#   Return: characters of $2 from position $1 onwards
chars_from = $(subst $(space),,$(wordlist $1,$(words $(call split_chars,$2)),$(call split_chars,$2)))

# Returns the first $1 characters of $2, like 'cut -c -$1' does.
#   $1: number of characters to return
#   $2: string to extract characters from
#   Return: the first $1 characters of $2
chars_upto = $(subst $(space),,$(wordlist 1,$1,$(call split_chars,$2)))

# Splits the decimal number $1 into a space separated list of its digits, e.g.
# '407' becomes '4 0 7'. This allows counting and comparing the length of
# numbers without having to do arithmetic.
#   $1: decimal number to split
#   Return: $1 as a space separated list of its digits
_digits = $(subst 0, 0,$(subst 1, 1,$(subst 2, 2,$(subst 3, 3,$(subst 4, 4,$(subst 5, 5,$(subst 6, 6,$(subst 7, 7,$(subst 8, 8,$(subst 9, 9,$1)))))))))))

# Removes the leading zeros from the decimal number $1, e.g. '007' becomes '7'
# and '000' becomes '0'.
#   $1: decimal number to strip
#   Return: $1 without leading zeros, or a single '0' if only zeros remain
_strip_zeros = $(if $(filter 0%,$1),$(if $(patsubst 0%,%,$1),$(call _strip_zeros,$(patsubst 0%,%,$1)),0),$1)

# Checks if every word of $1 is a decimal number, e.g. '2026 10 1' is returned
# as is, while '2026 10rc1' returns nothing.
#   $1: space separated list of words to check
#   Return: $1 if all of its words are decimal numbers, nothing otherwise
_numbers_only = $(if $(strip $(foreach part,$1,$(if $(call is_number,$(part)),,x))),,$1)

# Prepends zeros to the decimal number $2 until it has at least $1 digits, e.g.
# '7' padded to 3 digits becomes '007'. A number that already has $1 digits or
# more is returned unchanged.
#   $1: minimum number of digits
#   $2: decimal number to pad
#   Return: $2 with zeros prepended to at least $1 digits
_pad_left = $(if $(word $1,$(call _digits,$2)),$2,$(call _pad_left,$1,0$2))

# Pads the decimal number $2 to $1 digits, after removing its leading zeros,
# e.g. '0042' padded to 3 digits becomes '042'. An empty $2 is treated as '0'.
#   $1: minimum number of digits
#   $2: decimal number to pad
#   Return: $2 padded with zeros to at least $1 digits
_pad_number = $(call _pad_left,$1,$(call _strip_zeros,$(strip $(if $2,$2,0))))

# Checks if $1 is a single decimal number, ignoring surrounding whitespace.
#   $1: string to check
#   Return: $1 without surrounding whitespace if it is a single word that only
#           consists of digits, nothing otherwise
is_number = $(if $(filter 1,$(words $1)),$(if $(subst 0,,$(subst 1,,$(subst 2,,$(subst 3,,$(subst 4,,$(subst 5,,$(subst 6,,$(subst 7,,$(subst 8,,$(subst 9,,$(strip $1))))))))))),,$(strip $1)))

# Gets major, minor, patch from 'major.minor.patch', e.g.: 4.2.1 by index
#   $1: index
#   $2: version
#   Return: the part at index $1, or nothing if it is missing
_version = $(word $1, $(subst ., ,$2))

# Gets the '.' separated parts of a version string, without leading zeros.
# Everything after the first '-' is ignored, such that the parts of
# '2026.04.01-devel-123-gabcd' are '2026', '4' and '1'. Leading zeros are
# removed, such that a part is never interpreted as an octal number by the
# compiler.
#   $1: version string
#   Return: the parts, or nothing if any of them is not a decimal number
_version_parts = $(foreach part,$(call _numbers_only,$(subst ., ,$(firstword $(subst -, ,$1)))),$(call _strip_zeros,$(part)))

# Gets the version string as a comma separated 'major,minor,patch' tuple.
# Everything after the first '-' is ignored, a missing patch becomes '0' and
# any part after the patch is dropped, e.g. '2026.10-devel-123-gabcd' becomes
# '2026,10,0'. Leading zeros are removed, e.g. '2026.04.08' becomes
# '2026,4,8'.
#   $1: version string
#   Return: the tuple, or nothing if the version has no major and minor or is
#           malformed
version_tuple = $(strip $(foreach tuple,\
    $(subst $(space),$(comma),$(strip $(wordlist 1,3,$(call _version_parts,$1) 0))),\
    $(if $(word 3,$(subst $(comma),$(space),$(tuple))),$(tuple))))

# Joins the arguments $1..$8 with commas, after stripping their surrounding
# whitespace. Empty arguments in between are kept, e.g. '1,,3' stays '1,,3',
# but trailing empty arguments are dropped, because Make cannot distinguish
# them from arguments that were not passed at all.
#   $1..$8: arguments to join
#   Return: the comma separated arguments
_join_args = $(strip $1)$(if $(strip $2$3$4$5$6$7$8),$(comma)$(call _join_args,$2,$3,$4,$5,$6,$7,$8))

# Formats a macro invocation '$1($2,$3,...)', with the parentheses escaped such
# that it can be passed to the compiler through the shell, e.g. 'FOO\(1,2\)'.
#   $1: name of the macro
#   $2..$9: arguments of the macro (empty arguments in between are kept, but
#           trailing empty arguments are omitted)
#   Return: the escaped macro invocation, or nothing if $2 is empty
format_macro = $(if $(strip $2),$1\($(call _join_args,$2,$3,$4,$5,$6,$7,$8,$9)\))

# Returns padded version 'major.minor.patch' to 3 digits, e.g. '4.2.1' is
# padded as '004.002.001'.
#   $1: version
#   Return: the padded version
_padded_version = $(subst $(space),.,$(foreach var,1 2 3,$(call _pad_number,3,$(call _version,$(var),$1))))

# Checks if  $1 is greater than $2
#   $1, $2: The values to compare must be padded to the same length
#           otherwise when using '$(sort)' make will consider '2' larger
#           than '19'
#   Return: 1 if $1 is greater than $2, nothing otherwise
_is_greater = $(if $(filter $1,$(firstword $(sort $1 $2))),,1)

# Checks if version $1 is greater than version $2
#   $1: version to check, '.' separated version 'major.minor.patch'
#   $2: minimum version, '.' separated version 'major.minor.patch'
#   Return: 1 if $1 is greater than $2, nothing otherwise
version_is_greater = $(call _is_greater,$(call _padded_version,$1),\
                        $(call _padded_version,$2))

# Checks if version $1 is equal to version $2
#   $1,$2: version to check, '.' separated version 'major.minor.patch'
#   Return: 1 if $1 equal to $2, nothing otherwise
_is_equal = $(if $(and $(findstring $(1),$(2)),$(findstring $(2),$(1))),1,)

# Checks if version $1 is greater or equal than version $2
#   $1: version to check, '.' separated version 'major.minor.patch'
#   $2: minimum version, '.' separated version 'major.minor.patch'
#   Return: 1 if $1 is greater or equal than $2, nothing otherwise
version_is_greater_or_equal = $(or \
    $(call _is_greater,$(call _padded_version,$1),$(call _padded_version,$2)),\
    $(call _is_equal,$(call _padded_version,$1),$(call _padded_version,$2)))

# Returns the length of the decimal number $1 as a list of 'x', so that lengths
# can be compared with '$(word)' without doing arithmetic, e.g. '407' becomes
# 'x x x'.
#   $1: decimal number to measure
#   Return: one 'x' for every digit of $1
_width = $(foreach digit,$(call _digits,$1),x)

# Returns whichever of the two lists $1 and $2 holds the most words.
#   $1: first list
#   $2: second list
#   Return: the longer of both lists, or $1 if both are equally long
_longer_list = $(if $(strip $2),$(if $(word $(words $2),$1),$1,$2),$(strip $1))

# Returns the length of the longest decimal number in $1, e.g. '7 19 407'
# becomes 'x x x'.
#   $1: space separated list of decimal numbers
#   Return: one 'x' for every digit of the longest number, or nothing if $1 is
#           empty
_max_width = $(if $(strip $1),$(call _longer_list,$(call _width,$(firstword $1)),$(call _max_width,$(wordlist 2,$(words $1),$1))))

# Get the maximum number of the natural numbers given as $1.
#   $1: A list of natural numbers separated by white space
#   Return: The value of the highest natural number in $1
#
# All numbers are padded to a common width first, otherwise '$(sort)' would
# consider '2' larger than '19'.
max_number = $(if $(strip $1),$(call _strip_zeros,$(lastword $(sort \
    $(foreach number,$(strip $1),\
        $(call _pad_number,$(words $(call _max_width,$(strip $1))),$(number)))))),0)
