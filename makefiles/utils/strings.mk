# Make only version of string functions
#
# This replaces the pattern of using ':= $(shell echo $(var) | tr 'a-z-' 'A-Z_)'
# On local tests the make version was ~100 times faster than the shell one

lowercase = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))
uppercase = $(subst a,A,$(subst b,B,$(subst c,C,$(subst d,D,$(subst e,E,$(subst f,F,$(subst g,G,$(subst h,H,$(subst i,I,$(subst j,J,$(subst k,K,$(subst l,L,$(subst m,M,$(subst n,N,$(subst o,O,$(subst p,P,$(subst q,Q,$(subst r,R,$(subst s,S,$(subst t,T,$(subst u,U,$(subst v,V,$(subst w,W,$(subst x,X,$(subst y,Y,$(subst z,Z,$1))))))))))))))))))))))))))
uppercase_and_underscore = $(call uppercase,$(subst -,_,$1))

# Splits the string $1 into a space separated list of its characters, e.g.
# 'samd21' becomes 's a m d 2 1'. Only alphanumeric characters, '_' and '-' are
# recognised, which covers the CPU model and family names this is used for.
split_chars = $(strip $(subst 0, 0,$(subst 1, 1,$(subst 2, 2,$(subst 3, 3,$(subst 4, 4,$(subst 5, 5,$(subst 6, 6,$(subst 7, 7,$(subst 8, 8,$(subst 9, 9,$(subst a, a,$(subst b, b,$(subst c, c,$(subst d, d,$(subst e, e,$(subst f, f,$(subst g, g,$(subst h, h,$(subst i, i,$(subst j, j,$(subst k, k,$(subst l, l,$(subst m, m,$(subst n, n,$(subst o, o,$(subst p, p,$(subst q, q,$(subst r, r,$(subst s, s,$(subst t, t,$(subst u, u,$(subst v, v,$(subst w, w,$(subst x, x,$(subst y, y,$(subst z, z,$(subst A, A,$(subst B, B,$(subst C, C,$(subst D, D,$(subst E, E,$(subst F, F,$(subst G, G,$(subst H, H,$(subst I, I,$(subst J, J,$(subst K, K,$(subst L, L,$(subst M, M,$(subst N, N,$(subst O, O,$(subst P, P,$(subst Q, Q,$(subst R, R,$(subst S, S,$(subst T, T,$(subst U, U,$(subst V, V,$(subst W, W,$(subst X, X,$(subst Y, Y,$(subst Z, Z,$(subst _, _,$(subst -, -,$1)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))

# Returns the first $1 characters of $2, like 'cut -c -$1' does.
first_chars = $(subst $(space),,$(wordlist 1,$1,$(call split_chars,$2)))

# Returns the characters of $2 from position $1 onwards, like 'cut -c $1-'.
chars_from = $(subst $(space),,$(wordlist $1,$(words $(call split_chars,$2)),$(call split_chars,$2)))

# Splits the decimal number $1 into a space separated list of its digits, e.g.
# '407' becomes '4 0 7'. This allows counting and comparing the length of
# numbers without having to do arithmetic.
_digits = $(subst 0, 0,$(subst 1, 1,$(subst 2, 2,$(subst 3, 3,$(subst 4, 4,$(subst 5, 5,$(subst 6, 6,$(subst 7, 7,$(subst 8, 8,$(subst 9, 9,$1)))))))))))

# Removes leading zeros from $1, keeping a single zero if nothing else remains.
_strip_zeros = $(if $(filter 0%,$1),$(if $(patsubst 0%,%,$1),$(call _strip_zeros,$(patsubst 0%,%,$1)),0),$1)

# Prepends zeros to $2 until it has at least $1 digits.
_pad_left = $(if $(word $1,$(call _digits,$2)),$2,$(call _pad_left,$1,0$2))

# Padds number $2 to $1 digits. If $2 is empty, zero will be printed instead.
_pad_number = $(call _pad_left,$1,$(call _strip_zeros,$(strip $(if $2,$2,0))))

# Gets major, minor, patch from 'major.minor.patch', e.g.: 4.2.1 by index
#   $1: index
#   $2: version
_version = $(word $1, $(subst ., ,$2))

# Returns padded version 'major.minor.patch' to 3 digits
# eg: 4.2.1 -> 004.002.001
#   $1: version
_padded_version = $(subst $(space),.,$(foreach var,1 2 3,$(call _pad_number,3,$(call _version,$(var),$1))))

# Checks if  $1 is greater than $2
#   $1, $2: The values to compare must be padded to the same length
#           otherwise when using '$(sort)' make will consider '2' larger
#           than '19'
#   Return 1 if $1 is greater than $2, nothing otherwise
_is_greater = $(if $(filter $1,$(firstword $(sort $1 $2))),,1)

# Checks if version $1 is greater than version $2
#   $1: version to check, '.' separated version 'major.minor.patch'
#   $2: minimum version, '.' separated version 'major.minor.patch'
#   Return 1 if $1 is greater than $2, nothing otherwise
version_is_greater = $(call _is_greater,$(call _padded_version,$1),\
                        $(call _padded_version,$2))

# Checks if version $1 is equal to version $2
#   $1,$2: version to check, '.' separated version 'major.minor.patch'
#   Return 1 if $1 equal to $2, nothing otherwise
_is_equal = $(if $(and $(findstring $(1),$(2)),$(findstring $(2),$(1))),1,)

# Checks if version $1 is greater or equal than version $2
#   $1: version to check, '.' separated version 'major.minor.patch'
#   $2: minimum version, '.' separated version 'major.minor.patch'
#   Return 1 if $1 is greater or equal than $2, nothing otherwise
version_is_greater_or_equal = $(or \
    $(call _is_greater,$(call _padded_version,$1),$(call _padded_version,$2)),\
    $(call _is_equal,$(call _padded_version,$1),$(call _padded_version,$2)))

# Returns the length of the decimal number $1 as a list of 'x', so that lengths
# can be compared with '$(word)' without doing arithmetic.
_width = $(foreach digit,$(call _digits,$1),x)

# Returns whichever of the two lists $1 and $2 holds the most entries.
_longer_list = $(if $(strip $2),$(if $(word $(words $2),$1),$1,$2),$(strip $1))

# Returns the length of the longest number in $1, as a list of 'x'.
_max_width = $(if $(strip $1),$(call _longer_list,$(call _width,$(firstword $1)),$(call _max_width,$(wordlist 2,$(words $1),$1))))

# Get the maximum number of the natural numbers given as $1
#   $1: A list of natural numbers separated by white space
#   Return: The value of the highest natural number in $1
#
# All numbers are padded to a common width first, otherwise '$(sort)' would
# consider '2' larger than '19'.
max_number = $(if $(strip $1),$(call _strip_zeros,$(lastword $(sort \
    $(foreach number,$(strip $1),\
        $(call _pad_number,$(words $(call _max_width,$(strip $1))),$(number)))))),0)
