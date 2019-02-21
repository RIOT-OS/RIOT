include checks.mk

test-ensure_value:
	@test "$@" = "$(call ensure_value,$@,"This should not fail")"

test-ensure_value-negative:
	@echo $(call ensure_value,$^,"This should fail")
