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
