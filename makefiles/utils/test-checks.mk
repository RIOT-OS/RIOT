include checks.mk

test-ensure_value:
	@test "$@" = "$(call ensure_value,$@,"This should not fail")"

test-ensure_value-negative:
	@echo $(call ensure_value,$^,"This should fail")

test-assert:
	@test -z "$(call assert,$@,"This should not fail")"

test-assert-negative:
	@test -z "$(call assert,$^,"This should fail")"

test-assert_not:
	@test -z "$(call assert_not,$^,"This should not fail")"

test-assert_not-negative:
	@test -z "$(call assert_not,$@,"This should fail")"

test-assert_eq:
	@test -z "$(call assert_eq,x,x,"This should not fail")"

test-assert_eq-negative:
	@test -z "$(call assert_eq,z,x,"This should fail")"
