include variables.mk
include strings.mk

STRING_LOWER = abcdefghijklmnopqrstuvwxyz-123456789
STRING_UPPER = ABCDEFGHIJKLMNOPQRSTUVWXYZ-123456789
STRING_MACRO = ABCDEFGHIJKLMNOPQRSTUVWXYZ_123456789

test-lowercase:
	$(Q)test "$(STRING_LOWER)" = "$(call lowercase,$(STRING_UPPER))" || { echo ERROR: "$(STRING_LOWER)" != "$(call lowercase,$(STRING_UPPER))"; exit 1; }

test-uppercase:
	$(Q)test "$(STRING_UPPER)" = "$(call uppercase,$(STRING_LOWER))" || { echo ERROR: "$(STRING_UPPER)" != "$(call uppercase,$(STRING_LOWER))"; exit 1; }

test-uppercase_and_underscore:
	$(Q)test "$(STRING_MACRO)" = "$(call uppercase_and_underscore,$(STRING_LOWER))" || { echo ERROR: "$(STRING_MACRO)" != "$(call uppercase_and_underscore,$(STRING_LOWER))"; exit 1; }

test-split_chars:
	$(Q)test "s a m d 2 1" = "$(call split_chars,samd21)"
	$(Q)test "x" = "$(call split_chars,x)"
	$(Q)test "a - b _ C 9" = "$(call split_chars,a-b_C9)"
	$(Q)test "" = "$(call split_chars,)"

test-first_chars:
	$(Q)test "samd21" = "$(call first_chars,6,samd21g18a)"
	$(Q)test "stm32f767" = "$(call first_chars,9,stm32f767zi)"
	$(Q)test "s" = "$(call first_chars,1,samd21)"
	$(Q)test "a-b" = "$(call first_chars,3,a-b_C9)"
	$(Q)test "samd21" = "$(call first_chars,6,samd21)"
	$(Q)test "samd21" = "$(call first_chars,20,samd21)"
	$(Q)test "" = "$(call first_chars,3,)"

test-chars_from:
	$(Q)test "128a1" = "$(call chars_from,8,atxmega128a1)"
	$(Q)test "256a3bu" = "$(call chars_from,8,atxmega256a3bu)"
	$(Q)test "samd21" = "$(call chars_from,1,samd21)"
	$(Q)test "b_C9" = "$(call chars_from,3,a-b_C9)"
	$(Q)test "1" = "$(call chars_from,6,samd21)"
	$(Q)test "" = "$(call chars_from,20,samd21)"
	$(Q)test "" = "$(call chars_from,3,)"

TEST_VERSION_1 = 4.1
TEST_VERSION_2 = 3.10
TEST_VERSION_3 = 4.2.3
TEST_VERSION_4 = 4.19.3
TEST_VERSION_5 = 4.1.0

# Single component versions, as used for the compiler version checks
TEST_VERSION_6 = 8
TEST_VERSION_7 = 15

test-version_is_greater:
	$(Q)test 1 = "$(call version_is_greater,$(TEST_VERSION_1),$(TEST_VERSION_2))" || { echo ERROR: "$(TEST_VERSION_2)" \< "$(TEST_VERSION_1)"; exit 1; }
	$(Q)test 1 = "$(call version_is_greater,$(TEST_VERSION_3),$(TEST_VERSION_1))" || { echo ERROR: "$(TEST_VERSION_1)" \< "$(TEST_VERSION_3)"; exit 1; }
	$(Q)test 1 = "$(call version_is_greater,$(TEST_VERSION_4),$(TEST_VERSION_3))" || { echo ERROR: "$(TEST_VERSION_3)" \< "$(TEST_VERSION_4)"; exit 1; }
	$(Q)test "" = "$(call version_is_greater,$(TEST_VERSION_3),$(TEST_VERSION_4))" || { echo ERROR: Test should fail, "$(TEST_VERSION_4)" is not \< "$(TEST_VERSION_3)"; exit 1; }
	$(Q)test "" = "$(call version_is_greater,$(TEST_VERSION_1),$(TEST_VERSION_4))" || { echo ERROR: Test should fail, "$(TEST_VERSION_1)" is not \< "$(TEST_VERSION_4)"; exit 1; }
	$(Q)test 1 = "$(call version_is_greater,$(TEST_VERSION_7),$(TEST_VERSION_6))" || { echo ERROR: "$(TEST_VERSION_6)" \< "$(TEST_VERSION_7)"; exit 1; }
	$(Q)test "" = "$(call version_is_greater,$(TEST_VERSION_6),$(TEST_VERSION_7))" || { echo ERROR: Test should fail, "$(TEST_VERSION_7)" is not \< "$(TEST_VERSION_6)"; exit 1; }

test-version_is_greater_or_equal:
	$(Q)test 1 = "$(call version_is_greater_or_equal,$(TEST_VERSION_1),$(TEST_VERSION_5))" || { echo ERROR: "$(TEST_VERSION_5)" == "$(TEST_VERSION_1)"; exit 1; }
	$(Q)test 1 = "$(call version_is_greater_or_equal,$(TEST_VERSION_1),$(TEST_VERSION_1))" || { echo ERROR: "$(TEST_VERSION_1)" == "$(TEST_VERSION_1)"; exit 1; }
	$(Q)test 1 = "$(call version_is_greater_or_equal,$(TEST_VERSION_3),$(TEST_VERSION_3))" || { echo ERROR: "$(TEST_VERSION_3)" == "$(TEST_VERSION_3)"; exit 1; }
	$(Q)test 1 = "$(call version_is_greater_or_equal,$(TEST_VERSION_3),$(TEST_VERSION_1))" || { echo ERROR: "$(TEST_VERSION_1)" \< "$(TEST_VERSION_3)"; exit 1; }
	$(Q)test 1 = "$(call version_is_greater_or_equal,$(TEST_VERSION_4),$(TEST_VERSION_3))" || { echo ERROR: "$(TEST_VERSION_3)" \< "$(TEST_VERSION_4)"; exit 1; }
	$(Q)test "" = "$(call version_is_greater_or_equal,$(TEST_VERSION_3),$(TEST_VERSION_4))" || { echo ERROR: Test should fail, "$(TEST_VERSION_4)" is not \< "$(TEST_VERSION_3)"; exit 1; }
	$(Q)test "" = "$(call version_is_greater_or_equal,$(TEST_VERSION_1),$(TEST_VERSION_4))" || { echo ERROR: Test should fail, "$(TEST_VERSION_1)" is not \< "$(TEST_VERSION_4)"; exit 1; }
	$(Q)test 1 = "$(call version_is_greater_or_equal,$(TEST_VERSION_7),$(TEST_VERSION_6))" || { echo ERROR: "$(TEST_VERSION_6)" \< "$(TEST_VERSION_7)"; exit 1; }
	$(Q)test 1 = "$(call version_is_greater_or_equal,$(TEST_VERSION_6),$(TEST_VERSION_6))" || { echo ERROR: "$(TEST_VERSION_6)" == "$(TEST_VERSION_6)"; exit 1; }
	$(Q)test "" = "$(call version_is_greater_or_equal,$(TEST_VERSION_6),$(TEST_VERSION_7))" || { echo ERROR: Test should fail, "$(TEST_VERSION_7)" is not \< "$(TEST_VERSION_6)"; exit 1; }

test-max_number:
	$(Q)test "8" = "$(call max_number, 7 4 8 0 1 3)"
	$(Q)test "42" = "$(call max_number, 42 4 8 0 1 3)"
	$(Q)test "1337" = "$(call max_number, 42 4 8 0 1 1337)"
	$(Q)test "0" = "$(call max_number, 0 0 0 0 0)"
	$(Q)test "13" = "$(call max_number, 13)"
	$(Q)test "0" = "$(call max_number,)"
	$(Q)test "10" = "$(call max_number, 9 10)"
	$(Q)test "19" = "$(call max_number, 2 19)"
	$(Q)test "100" = "$(call max_number, 99 100)"
	$(Q)test "1000" = "$(call max_number, 1000 999)"
	$(Q)test "100" = "$(call max_number, 100 20 3)"
	$(Q)test "8" = "$(call max_number, 007 8)"
