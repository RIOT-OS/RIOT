# Make only version of string functions
#
# This replaces the pattern of using ':= $(shell echo $(var) | tr 'a-z-' 'A-Z_)'
# On local tests the make version was ~100 times faster than the shell one

lowercase = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))
uppercase = $(subst a,A,$(subst b,B,$(subst c,C,$(subst d,D,$(subst e,E,$(subst f,F,$(subst g,G,$(subst h,H,$(subst i,I,$(subst j,J,$(subst k,K,$(subst l,L,$(subst m,M,$(subst n,N,$(subst o,O,$(subst p,P,$(subst q,Q,$(subst r,R,$(subst s,S,$(subst t,T,$(subst u,U,$(subst v,V,$(subst w,W,$(subst x,X,$(subst y,Y,$(subst z,Z,$1))))))))))))))))))))))))))
uppercase_and_underscore = $(call uppercase,$(subst -,_,$1))

# Padds number $2 to $1 digits. If $2 is empty, zero will be printed instead.
_pad_number = $(shell printf '%0$1d' $(if $2,$2,0))

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

# Get the maximum number of the natural numbers given as $1
#   $1: A list of natural numbers separated by white space
#   Return: The value of the highest natural number in $1
max_number = $(shell echo $1 | awk 'BEGIN{max=0} {for(i=1;i<=NF;i++){x=$$i; if (x > max){max = x}}} END{print max}')
