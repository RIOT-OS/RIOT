# Test filesystem path functions

include paths.mk
include checks.mk

test-relpath:
	$(call assert_eq,$(call relpath,/a/b/c/d/e,/a/b/X/c/d/e),../../../X/c/d/e)
	$(call assert_eq,$(call relpath,/1/2/3/5/6/7,/),../../../../../..)
	$(call assert_eq,$(call relpath,/,/1/2/3/5/6/),1/2/3/5/6)
	$(call assert_eq,$(call relpath,/a/b/c,/a/b/X/k),../X/k)
	@# these fail but it should not matter because $1 should always be a dir
	@# and $2 a file.
	@# Asserting that the result is empty at least protects us from a wrong
	@# result and is even useful considering how relpath is used.
	$(call assert_not,$(call relpath,/,/),"Expected empty relative path")
	$(call assert_not,$(call relpath,/1/2/3,/1/2/3),"Expected empty relative path")
	@true

# Use coreutils to calculate a relative path
# This will only work if coreutils is installed
sys_relpath = $(shell realpath -m --relative-to=$(1) $(2))

test-relpath-against-sys:
	$(call assert_eq,$(call relpath,/1/2/3/5/6/7,/1/2/3/4/5/6/7),$(call sys_relpath,/1/2/3/5/6/7,/1/2/3/4/5/6/7),)
	$(call assert_eq,$(call relpath,/1/2/3/5/6/7,/),$(call sys_relpath,/1/2/3/5/6/7,/),)
	$(call assert_eq,$(call relpath,/,/1/2/3/5/6/),$(call sys_relpath,/,/1/2/3/5/6/),)
	@true
