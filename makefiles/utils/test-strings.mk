include variables.mk
include strings.mk

STRING_LOWER = abcdefghijklmnopqrstuvwxyz-123456789
STRING_UPPER = ABCDEFGHIJKLMNOPQRSTUVWXYZ-123456789
STRING_MACRO = ABCDEFGHIJKLMNOPQRSTUVWXYZ_123456789

test-lowercase:
	$(Q)bash -c 'test "$(STRING_LOWER)" = "$(call lowercase,$(STRING_UPPER))" || { echo ERROR: "$(STRING_LOWER)" != "$(call lowercase,$(STRING_UPPER))"; exit 1; }'

test-uppercase:
	$(Q)bash -c 'test "$(STRING_UPPER)" = "$(call uppercase,$(STRING_LOWER))" || { echo ERROR: "$(STRING_UPPER)" != "$(call uppercase,$(STRING_LOWER))"; exit 1; }'

test-uppercase_and_underscore:
	$(Q)bash -c 'test "$(STRING_MACRO)" = "$(call uppercase_and_underscore,$(STRING_LOWER))" || { echo ERROR: "$(STRING_MACRO)" != "$(call uppercase_and_underscore,$(STRING_LOWER))"; exit 1; }'

TEST_VERSION_1 = 4.1
TEST_VERSION_2 = 3.10
TEST_VERSION_3 = 4.2.3
TEST_VERSION_4 = 4.19.3
TEST_VERSION_5 = 4.1.0

test-version_is_greater:
	$(Q)bash -c 'test 1 = "$(call version_is_greater,$(TEST_VERSION_1),$(TEST_VERSION_2))" || { echo ERROR: "$(TEST_VERSION_2)" \< "$(TEST_VERSION_1)"; exit 1; }'
	$(Q)bash -c 'test 1 = "$(call version_is_greater,$(TEST_VERSION_3),$(TEST_VERSION_1))" || { echo ERROR: "$(TEST_VERSION_1)" \< "$(TEST_VERSION_3)"; exit 1; }'
	$(Q)bash -c 'test 1 = "$(call version_is_greater,$(TEST_VERSION_4),$(TEST_VERSION_3))" || { echo ERROR: "$(TEST_VERSION_3)" \< "$(TEST_VERSION_4)"; exit 1; }'
	$(Q)bash -c 'test "" = "$(call version_is_greater,$(TEST_VERSION_3),$(TEST_VERSION_4))" || { echo ERROR: Test should fail, "$(TEST_VERSION_4)" is not \< "$(TEST_VERSION_3)"; exit 1; }'
	$(Q)bash -c 'test "" = "$(call version_is_greater,$(TEST_VERSION_1),$(TEST_VERSION_4))" || { echo ERROR: Test should fail, "$(TEST_VERSION_1)" is not \< "$(TEST_VERSION_4)"; exit 1; }'

test-version_is_greater_or_equal:
	$(Q)bash -c 'test 1 = "$(call version_is_greater_or_equal,$(TEST_VERSION_1),$(TEST_VERSION_5))" || { echo ERROR: "$(TEST_VERSION_5)" == "$(TEST_VERSION_1)"; exit 1; }'
	$(Q)bash -c 'test 1 = "$(call version_is_greater_or_equal,$(TEST_VERSION_1),$(TEST_VERSION_1))" || { echo ERROR: "$(TEST_VERSION_1)" == "$(TEST_VERSION_1)"; exit 1; }'
	$(Q)bash -c 'test 1 = "$(call version_is_greater_or_equal,$(TEST_VERSION_3),$(TEST_VERSION_3))" || { echo ERROR: "$(TEST_VERSION_3)" == "$(TEST_VERSION_3)"; exit 1; }'
	$(Q)bash -c 'test 1 = "$(call version_is_greater_or_equal,$(TEST_VERSION_3),$(TEST_VERSION_1))" || { echo ERROR: "$(TEST_VERSION_1)" \< "$(TEST_VERSION_3)"; exit 1; }'
	$(Q)bash -c 'test 1 = "$(call version_is_greater_or_equal,$(TEST_VERSION_4),$(TEST_VERSION_3))" || { echo ERROR: "$(TEST_VERSION_3)" \< "$(TEST_VERSION_4)"; exit 1; }'
	$(Q)bash -c 'test "" = "$(call version_is_greater_or_equal,$(TEST_VERSION_3),$(TEST_VERSION_4))" || { echo ERROR: Test should fail, "$(TEST_VERSION_4)" is not \< "$(TEST_VERSION_3)"; exit 1; }'
	$(Q)bash -c 'test "" = "$(call version_is_greater_or_equal,$(TEST_VERSION_1),$(TEST_VERSION_4))" || { echo ERROR: Test should fail, "$(TEST_VERSION_1)" is not \< "$(TEST_VERSION_4)"; exit 1; }'
